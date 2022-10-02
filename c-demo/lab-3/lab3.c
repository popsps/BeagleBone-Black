#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
typedef struct trafic_signal_struct {
  char* name;
  unsigned int red_light;
  unsigned int yellow_light;
  unsigned int green_light;
  unsigned int sensor;
} trafic_signal;

void cleanUp(trafic_signal* signal);
void initilize(trafic_signal* signal);
void unset_signal(trafic_signal* signal);
void makeSignalRed(trafic_signal* signal);
void makeSignalYellow(trafic_signal* signal);
void makeSignalGreen(trafic_signal* signal);
void sig_handler(int sig);

static trafic_signal signal1;
static trafic_signal signal2;

int main(int argc, char* argv[]) {
  printf("CS 695 Lab3 Simple Intersection with Opposing Traffic Lights\n");

  // init pins that are being used for trafic signal 1
  signal1.name = "SIGNAL1";
  signal1.red_light = P9_11;
  signal1.yellow_light = P9_12;
  signal1.green_light = P9_13;
  signal1.sensor = P9_15;

  // init pins that are being used for trafic signal 2
  signal2.name = "SIGNAL2";
  signal2.red_light = P9_23;
  signal2.yellow_light = P9_24;
  signal2.green_light = P9_26;
  signal2.sensor = P9_17;

  // init signal handler
  registerSignals(sig_handler);

  cleanUp(&signal1);
  cleanUp(&signal2);
  initilize(&signal1);
  initilize(&signal2);
  unset_signal(&signal1);
  unset_signal(&signal2);

  // start the signals for intersection
  sleep(1);
  do {
    // testing should be removed modified
    int gpv = gpio_get_value(signal1.sensor);
    if (gpv) {
      printf("GPIO PIN_15 is pressed %d\n", gpv);
    }
    makeSignalGreen(&signal1);
    makeSignalRed(&signal2);
    sleep(20);
    makeSignalYellow(&signal1);
    sleep(2.5);
    makeSignalRed(&signal1);
    makeSignalGreen(&signal2);
    sleep(20);
    makeSignalYellow(&signal2);
    sleep(2.5);
  } while (1);

  // cleanUp(&signal1);
  // cleanUp(&signal2);
  printf("CS 695 Lab3 Simple Intersection with Opposing Traffic Lights Done\n");
  return 0;
}

void cleanUp(trafic_signal* signal) {
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

void makeSignalRed(trafic_signal* signal) {
  gpio_set_value(signal->red_light, HIGH);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, LOW);
}
void makeSignalYellow(trafic_signal* signal) {
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, HIGH);
  gpio_set_value(signal->green_light, LOW);
}
void makeSignalGreen(trafic_signal* signal) {
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, HIGH);
}
void unset_signal(trafic_signal* signal) {
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, LOW);
  gpio_set_value(signal->sensor, LOW);
}
void sig_handler(int sig) {
  if (sig == SIGINT) {
    shell_write("Recived SIGINT");
    unset_signal(&signal1);
    unset_signal(&signal2);
    shell_write("Pins are cleaned up.");
    _exit(EXIT_SUCCESS);
  } else if (sig == SIGTSTP) {
    shell_write("Recived SIGTSTP");
    unset_signal(&signal1);
    unset_signal(&signal2);
    shell_write("Pins are cleaned up.");
    _exit(EXIT_SUCCESS);
  }
}