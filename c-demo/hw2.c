#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gpio.h"
#include "pins.h"
#include "signal.h"

/**
 * Each side gets green light for 2 minutes. The transition to the red light
 * should have a 5 second yellow signal.
 *
 * Using GPIO 9_11 and GPIO 9_12 GPIO 2_13 for signal 1
 * Using GPIO 9_23 and GPIO 9_24 GPIO 2_26 for signal 2
 **/
typedef struct trafic_signal_struct {
  int red_light;
  int yellow_light;
  int green_light;
} trafic_signal;

void cleanUp(trafic_signal* signal);
void initilize(trafic_signal* signal);
void unset_signal(trafic_signal* signal);
void makeSignalRed(trafic_signal* signal);
void makeSignalYellow(trafic_signal* signal);
void makeSignalGreen(trafic_signal* signal);
void sig_handler(int sig);

trafic_signal signal1;
trafic_signal signal2;

int main(int argc, char* argv[]) {
  printf("CS 695 HW2 Simple Intersection with Opposing Traffic Lights\n");

  // init pins that are being used for each trafic signal 1
  signal1.red_light = P9_11;
  signal1.yellow_light = P9_12;
  signal1.green_light = P9_13;

  // init pins that are being used for each trafic signal 1
  signal2.red_light = P9_23;
  signal2.yellow_light = P9_24;
  signal2.green_light = P9_26;

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
    makeSignalGreen(&signal1);
    makeSignalRed(&signal2);
    sleep(10);
    makeSignalYellow(&signal1);
    makeSignalYellow(&signal2);
    sleep(2.5);
    makeSignalRed(&signal1);
    makeSignalGreen(&signal2);
    sleep(10);
    makeSignalYellow(&signal1);
    makeSignalYellow(&signal2);
    sleep(2.5);
  } while (1);

  // cleanUp(&signal1);
  // cleanUp(&signal2);
  printf("CS 695 HW2 Simple Intersection with Opposing Traffic Lights Done\n");
  return 0;
}

void cleanUp(trafic_signal* signal) {
  gpio_unexport(signal->red_light);
  gpio_unexport(signal->yellow_light);
  gpio_unexport(signal->green_light);
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