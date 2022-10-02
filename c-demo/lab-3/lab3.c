#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bbsignal.h"
#include "gpio.h"
#include "pins.h"

/**
 * Each side gets green light for 2 minutes. The transition to the red light
 * should have a 5 second yellow signal.
 *
 * Using GPIO 9_11 and GPIO 9_12 GPIO 9_13 as out pins for signal 1
 * Using GPIO 9_23 and GPIO 9_24 GPIO 9_26 as out pins for signal 2
 * Using GPIO 9_15 as in pin for signal 1
 * Using GPIO 9_41 as int pin for signal 2
 *
 * One thread for each of the traffic signals
 * Add a wait sensor sensor at each direction
 * there should be one thread for these as well Implement this with a
 * push-button If a light is red but the corresponding wait sensor is triggered
 * (i.e. the button is pushed and held) for 5 seconds,
 * the light should cycle to green without waiting the full 2 minutes
 **/

#define RG_WAIT_TIME 10
#define Y_WAIT_TIME 2

typedef struct trafic_signal_struct {
  char* name;
  unsigned int red_light;
  unsigned int yellow_light;
  unsigned int green_light;
  unsigned int sensor;
  int isRed;
  int isGreen;
  int isYellow;
  int isPressed;
  int sensor_activated;
} trafic_signal;

void clean_up(trafic_signal* signal);
void initilize(trafic_signal* signal);
void unset_signal(trafic_signal* signal);
void make_signal_red(trafic_signal* signal);
void make_signal_yellow(trafic_signal* signal);
void make_signal_green(trafic_signal* signal);
void sig_handler(int sig);

static trafic_signal* signal1;
static trafic_signal* signal2;
static pthread_t sensor_thread;

void* handle_sensors(void* ptr);

pthread_mutex_t signal_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t signal_cond_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t signal_cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]) {
  printf("CS 695 Lab3 Simple Intersection with Opposing Traffic Lights\n");

  // init pins that are being used for trafic signal 1
  // initializing threads. Each thread will handle a signal.
  signal1 = malloc(sizeof(trafic_signal));
  signal1->name = "SIGNAL-1";
  signal1->red_light = P9_11;
  signal1->yellow_light = P9_12;
  signal1->green_light = P9_13;
  signal1->sensor = P9_15;
  signal1->isPressed = 0;
  signal1->isRed = 0;
  signal1->isYellow = 0;
  signal1->isGreen = 0;
  signal1->sensor_activated = 0;

  // init pins that are being used for trafic signal 2
  signal2 = malloc(sizeof(trafic_signal));
  signal2->name = "SIGNAL-2";
  signal2->red_light = P9_23;
  signal2->yellow_light = P9_24;
  signal2->green_light = P9_26;
  signal2->sensor = P9_27;
  signal2->isPressed = 0;
  signal2->isRed = 0;
  signal2->isYellow = 0;
  signal2->isGreen = 0;
  signal2->sensor_activated = 0;

  pthread_create(&sensor_thread, NULL, handle_sensors, NULL);

  // init signal handler
  registerSignals(sig_handler);

  clean_up(signal1);
  clean_up(signal2);
  initilize(signal1);
  initilize(signal2);
  unset_signal(signal1);
  unset_signal(signal2);

  // start the signals for intersection
  printf("[CS-699] Startinting Intersecion signals...\n");
  sleep(1);
  while (1) {
    pthread_mutex_lock(&signal_mutex);
    make_signal_green(signal1);
    make_signal_red(signal2);
    pthread_mutex_unlock(&signal_mutex);

    sleep(RG_WAIT_TIME);

    pthread_mutex_lock(&signal_mutex);
    make_signal_yellow(signal1);
    pthread_mutex_unlock(&signal_mutex);

    sleep(Y_WAIT_TIME);

    pthread_mutex_lock(&signal_mutex);
    make_signal_red(signal1);
    make_signal_green(signal2);
    pthread_mutex_unlock(&signal_mutex);

    sleep(RG_WAIT_TIME);

    pthread_mutex_lock(&signal_mutex);
    make_signal_yellow(signal2);
    pthread_mutex_unlock(&signal_mutex);

    sleep(Y_WAIT_TIME);
  }
  // pthread_join(signal1->thread, NULL);
  // pthread_join(signal2->thread, NULL);
  printf(
      "[CS-699] Lab3 Simple Intersection with Opposing Traffic Lights Done\n");
  return 0;
}

void clean_up(trafic_signal* signal) {
  gpio_unexport(signal->red_light);
  gpio_unexport(signal->yellow_light);
  gpio_unexport(signal->green_light);
  gpio_unexport(signal->sensor);
}
void initilize(trafic_signal* signal) {
  gpio_export(signal->red_light);
  gpio_set_direction(signal->red_light, OUTPUT_PIN);
  gpio_set_value(signal->red_light, LOW);

  gpio_export(signal->yellow_light);
  gpio_set_direction(signal->yellow_light, OUTPUT_PIN);
  gpio_set_value(signal->yellow_light, LOW);

  gpio_export(signal->green_light);
  gpio_set_direction(signal->green_light, OUTPUT_PIN);
  gpio_set_value(signal->green_light, LOW);

  gpio_export(signal->sensor);
  gpio_set_direction(signal->sensor, INPUT_PIN);
  gpio_set_value(signal->sensor, LOW);
}

void make_signal_red(trafic_signal* signal) {
  signal->isRed = 1;
  signal->isYellow = 0;
  signal->isGreen = 0;
  gpio_set_value(signal->red_light, HIGH);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, LOW);
}
void make_signal_yellow(trafic_signal* signal) {
  signal->isRed = 0;
  signal->isYellow = 1;
  signal->isGreen = 0;
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, HIGH);
  gpio_set_value(signal->green_light, LOW);
}
void make_signal_green(trafic_signal* signal) {
  signal->isRed = 0;
  signal->isYellow = 0;
  signal->isGreen = 1;
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, HIGH);
}
void unset_signal(trafic_signal* signal) {
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, LOW);
  gpio_set_value(signal->sensor, LOW);
  signal->isRed = 0;
  signal->isYellow = 0;
  signal->isGreen = 0;
}
void sig_handler(int sig) {
  if (sig == SIGINT) {
    shell_write("Recived SIGINT");
    unset_signal(signal1);
    unset_signal(signal2);
    shell_write("Pins are cleaned up.");
    _exit(EXIT_SUCCESS);
  } else if (sig == SIGTSTP) {
    shell_write("Recived SIGTSTP");
    unset_signal(signal1);
    unset_signal(signal2);
    shell_write("Pins are cleaned up.");
    _exit(EXIT_SUCCESS);
  }
}

/**
 * If the light is red and the sensor detected
 * a presence of a car waiting(push button pressed for 5 seconds)
 * then after 5 seconds or the remaining time of 2 min which ever comes first
 *  this signal cycle to green and the other signal cycle to red
 **/
void* handle_sensors(void* ptr) {
  // trafic_signal* signal = (trafic_signal*)ptr;
  printf("[THREAD-%ld] starts sensor thread working...\n", sensor_thread);
  time_t base = time(0);
  time_t now = base;
  while (1) {
    int gpv1 = gpio_get_value(signal1->sensor);
    int gpv2 = gpio_get_value(signal2->sensor);
    now = time(0);
    if (gpv1 && signal1->isRed) {
      printf("GPIO PIN_15 is pressed %d\n", gpv1);
      if (!signal1->isPressed) {
        signal1->isPressed = 1;
        base = now;
      }
    } else {
      signal1->isPressed = 0;
      signal1->sensor_activated = 0;
    }
    if (gpv1 && signal1->isRed && now - base >= 2) {
      printf("GPIO PIN_15 is hold for 2 seconds and activated\n");
      // make_signal_yellow(signal2);
      // sleep(Y_WAIT_TIME);
      // make_signal_red(signal2);
      // make_signal_green(signal1);
      // signal the main thread to start waiting and running the counter
    }

    if (gpv2 && signal2->isRed) {
      printf("GPIO PIN_27 is pressed %d\n", gpv2);
      // make_signal_yellow(signal1);
      // sleep(Y_WAIT_TIME);
      // make_signal_red(signal1);
      // make_signal_green(signal2);
      // signal the main thread to start waiting and running the counter
    } else {
      signal2->isPressed = 0;
      signal2->sensor_activated = 0;
    }
    usleep(200000);
  }
}