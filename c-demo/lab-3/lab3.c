#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bbsignal.h"
#include "gpio.h"
#include "logging.h"
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

#define RG_WAIT_TIME 30
#define Y_WAIT_TIME 4
#define SENSOR_ACTIVATION_TIME 2

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
  pthread_rwlock_t rwlock;
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
static pthread_t intersection_thread;
static int action = 0;

void* handle_sensors(void* ptr);
void* handle_intersection(void* ptr);

pthread_rwlock_t signal_rwlock = PTHREAD_RWLOCK_INITIALIZER;

int main(int argc, char* argv[]) {
  pthread_t current_thread = pthread_self();
  shell_print(
      BBLACK,
      "[THREAD%ld]: Lab3 Simple Intersection with Opposing Traffic Lights",
      current_thread);

  // init signal handler for SIGUSR1, SIGINT, and SIGTSTP
  register_signal_handler(sig_handler);
  // init signal blocker
  register_sigpromask();

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
  pthread_rwlock_init(&signal1->rwlock, NULL);

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
  pthread_rwlock_init(&signal2->rwlock, NULL);

  // clean up and unset any previous state of GPIOs and re-initialize them again
  clean_up(signal1);
  clean_up(signal2);
  initilize(signal1);
  initilize(signal2);
  unset_signal(signal1);
  unset_signal(signal2);

  // thread responsible for handling the general intersection logic
  pthread_create(&intersection_thread, NULL, handle_intersection, NULL);
  // thread responsible for handling sensor logic
  pthread_create(&sensor_thread, NULL, handle_sensors, NULL);

  pthread_join(intersection_thread, NULL);
  pthread_join(sensor_thread, NULL);

  shell_print(
      BBLACK,
      "[THREAD%ld]: Lab3 Simple Intersection with Opposing Traffic Lights Done",
      current_thread);
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
  pthread_rwlock_wrlock(&signal->rwlock);
  signal->isRed = 1;
  signal->isYellow = 0;
  signal->isGreen = 0;
  gpio_set_value(signal->red_light, HIGH);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, LOW);
  pthread_rwlock_unlock(&signal->rwlock);
}
void make_signal_yellow(trafic_signal* signal) {
  pthread_rwlock_wrlock(&signal->rwlock);
  signal->isRed = 0;
  signal->isYellow = 1;
  signal->isGreen = 0;
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, HIGH);
  gpio_set_value(signal->green_light, LOW);
  pthread_rwlock_unlock(&signal->rwlock);
}
void make_signal_green(trafic_signal* signal) {
  pthread_rwlock_wrlock(&signal->rwlock);
  signal->isRed = 0;
  signal->isYellow = 0;
  signal->isGreen = 1;
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, HIGH);
  pthread_rwlock_unlock(&signal->rwlock);
}
void unset_signal(trafic_signal* signal) {
  pthread_rwlock_wrlock(&signal->rwlock);
  signal->isRed = 0;
  signal->isYellow = 0;
  signal->isGreen = 0;
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, LOW);
  gpio_set_value(signal->sensor, LOW);
  pthread_rwlock_unlock(&signal->rwlock);
}

/**
 * handle OS signals
 * handle SIGINT CTRL+C
 * handle SIGTSTP CTRL+Z
 * handle SIGUSR1 custom signal to bypass sleep on a thread
 **/
void sig_handler(int sig) {
  pthread_t current_thread = pthread_self();
  if (sig == SIGINT) {
    shell_write(BPURPLE, "[THREAD%ld]: Recived SIGINT", current_thread);
    unset_signal(signal1);
    unset_signal(signal2);
    shell_write(BPURPLE, "[THREAD%ld]: Pins are cleaned up.", current_thread);
    _exit(EXIT_SUCCESS);
  } else if (sig == SIGTSTP) {
    shell_write(BPURPLE, "[THREAD%ld]: Recived SIGTSTP", current_thread);
    unset_signal(signal1);
    unset_signal(signal2);
    shell_write(BPURPLE, "[THREAD%ld]: Pins are cleaned up.", current_thread);
    _exit(EXIT_SUCCESS);
  } else if (sig == SIGUSR1) {
    shell_write(BPURPLE, "[THREAD%ld]: Recived SIGUSR1", current_thread);
  }
}

/**
 * handle the intersection logic for two signals.
 **/
void* handle_intersection(void* ptr) {
  // start the signals for intersection
  shell_print(KDEF, "[THREAD%ld]: Starting the intersecion thread...",
              intersection_thread);
  sleep(1);
  while (1) {
    pthread_rwlock_wrlock(&signal_rwlock);
    action = (action > 8) ? 1 : action + 1;
    int _action = action;
    pthread_rwlock_unlock(&signal_rwlock);
    switch (_action) {
      case 1:
        make_signal_green(signal1);
        make_signal_red(signal2);
        shell_print(KDEF, "[THREAD%ld]: { Signal1: GREEN }, { Signal2: RED }",
                    intersection_thread);
        break;
      case 2:
        sleep(RG_WAIT_TIME);
        break;
      case 3:
        make_signal_yellow(signal1);
        shell_print(KDEF, "[THREAD%ld]: { Signal1: YELLOW }, { Signal2: RED }",
                    intersection_thread);
        break;
      case 4:
        sleep(Y_WAIT_TIME);
        break;
      case 5:
        make_signal_red(signal1);
        make_signal_green(signal2);
        shell_print(KDEF, "[THREAD%ld]: { Signal1: RED }, { Signal2: GREEN }",
                    intersection_thread);
        break;
      case 6:
        sleep(RG_WAIT_TIME);
        break;
      case 7:
        make_signal_yellow(signal2);
        shell_print(KDEF, "[THREAD%ld]: { Signal1: RED }, { Signal2: YELLOW }",
                    intersection_thread);
        break;
      case 8:
        sleep(Y_WAIT_TIME);
        break;
      default:
        break;
    }
    usleep(200000);
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
  shell_print(BRED, "[THREAD%ld]: starting sensor thread...", sensor_thread);
  time_t base = time(0);
  time_t now = base;
  while (1) {
    int isSensor1Pressed = gpio_get_value(signal1->sensor);
    int isSensor2Pressed = gpio_get_value(signal2->sensor);
    pthread_rwlock_rdlock(&signal1->rwlock);
    int isSignal1Red = signal1->isRed;
    pthread_rwlock_unlock(&signal1->rwlock);
    pthread_rwlock_rdlock(&signal2->rwlock);
    int isSignal2Red = signal2->isRed;
    pthread_rwlock_unlock(&signal2->rwlock);
    now = time(0);

    // handle the sensor logic for the first signal
    if (isSensor1Pressed && isSignal1Red) {
      if (!signal1->isPressed) {
        shell_print(BRED, "[THREAD%ld]: GPIO INPUT_1 is pressed %d",
                    sensor_thread, isSensor1Pressed);
        signal1->isPressed = 1;
        base = now;
      }
    } else {  // if isSensor1Pressed = 0 (not pressed)
      signal1->isPressed = 0;
      signal1->sensor_activated = 0;
    }
    if (isSensor1Pressed && isSignal1Red &&
        now - base >= SENSOR_ACTIVATION_TIME) {
      if (!signal1->sensor_activated) {
        signal1->sensor_activated = 1;
        shell_print(
            BRED,
            "[THREAD%ld]: GPIO INPUT_1 is held for %d seconds and activated",
            sensor_thread, SENSOR_ACTIVATION_TIME);
        pthread_rwlock_wrlock(&signal_rwlock);
        // this technically set to action=7
        action = 6;
        pthread_rwlock_unlock(&signal_rwlock);
        shell_print(BRED, "[THREAD%ld]: Loading...", sensor_thread,
                    SENSOR_ACTIVATION_TIME);
        usleep(500000);
        // if the signal-thread is sleep it, wake it up
        pthread_kill(intersection_thread, SIGUSR1);
      }
    }

    // handle the sensor logic for the second signal
    if (isSensor2Pressed && isSignal2Red) {
      if (!signal2->isPressed) {
        shell_print(BRED, "[THREAD%ld]: GPIO INPUT_2 is pressed %d",
                    sensor_thread, isSensor2Pressed);
        signal2->isPressed = 1;
        base = now;
      }
    } else {
      signal2->isPressed = 0;
      signal2->sensor_activated = 0;
    }
    if (isSensor2Pressed && isSignal2Red &&
        now - base >= SENSOR_ACTIVATION_TIME) {
      if (!signal2->sensor_activated) {
        signal2->sensor_activated = 1;
        shell_print(
            BRED,
            "[THREAD%ld]: GPIO INPUT_2 is held for %d seconds and activated",
            sensor_thread, SENSOR_ACTIVATION_TIME);
        pthread_rwlock_wrlock(&signal_rwlock);
        // this technically set to action=3
        action = 2;
        pthread_rwlock_unlock(&signal_rwlock);
        shell_print(BRED, "[THREAD%ld]: Loading...", sensor_thread,
                    SENSOR_ACTIVATION_TIME);
        usleep(500000);
        // if the signal-thread is sleep it, wake it up
        pthread_kill(intersection_thread, SIGUSR1);
      }
    }
    usleep(200000);
  }
}