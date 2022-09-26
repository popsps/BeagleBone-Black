
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gpio.h"
#include "pins.h"
#include "signal.h"

/**
 * Using GPIO 9_23 and GPIO 9_24 GPIO 2_25
 **/
typedef struct trafic_signal_struct {
  char* name;
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

static trafic_signal signal1;
static trafic_signal signal2;

void sig_handler(int sig) {
  if (sig == SIGINT) {
    // printf("SIGINT\n");
    shell_write("Recived SIGINT");
    unset_signal(&signal1);
    unset_signal(&signal2);
    shell_write("Pins are cleaned up.");
    _exit(EXIT_SUCCESS);
  } else if (sig == SIGTSTP) {
    printf("SIGTSTP\n");
    _exit(EXIT_SUCCESS);
  }
}

int main(int argc, char* argv[]) {
  printf("CS 695 HW2 Simple Intersection with Opposing Traffic Lights\n");
  // init pins that are being used for each trafic signal 1
  signal1.name = "Signal 1";
  signal1.red_light = P9_11;
  signal1.yellow_light = P9_12;
  signal1.green_light = P9_13;

  // init pins that are being used for each trafic signal 2
  signal2.name = "Signal 2";
  signal2.red_light = P9_23;
  signal2.yellow_light = P9_24;
  signal2.green_light = P9_26;

  // init signal handler
  registerSignals(sig_handler);

  cleanUp(&signal1);
  cleanUp(&signal2);
  initilize(&signal1);
  initilize(&signal2);
  printf("**************************************\n");
  for (size_t i = 0; i < 5; i++) {
    makeSignalGreen(&signal1);
    printf("\n");
    makeSignalRed(&signal2);
    printf("**************************************\n");
    sleep(2 * 60);
    makeSignalYellow(&signal1);
    printf("\n");
    makeSignalYellow(&signal2);
    printf("**************************************\n");
    sleep(5);
    makeSignalRed(&signal1);
    printf("\n");
    makeSignalGreen(&signal2);
    printf("**************************************\n");
    sleep(2 * 60);
    makeSignalYellow(&signal1);
    printf("\n");
    makeSignalYellow(&signal2);
    printf("**************************************\n");
    sleep(5);
  }

  // set all Used GPIO pins to low
  unset_signal(&signal1);
  unset_signal(&signal2);

  printf("Clean up Pins...\n");
  // cleanUp(&signal1);
  // cleanUp(&signal2);
  printf("CS 695 HW2 Simple Intersection with Opposing Traffic Lights Done\n");
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
  printf("%s Red light is on\n", signal->name);
  printf("%s Yellow light is off\n", signal->name);
  printf("%s Green light is off\n", signal->name);
}
void makeSignalYellow(trafic_signal* signal) {
  printf("%s Red light is off\n", signal->name);
  printf("%s Yellow light is on\n", signal->name);
  printf("%s Green light is off\n", signal->name);
}
void makeSignalGreen(trafic_signal* signal) {
  printf("%s Red light is off\n", signal->name);
  printf("%s Yellow light is off\n", signal->name);
  printf("%s Green light is on\n", signal->name);
}
void unset_signal(trafic_signal* signal) {
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, LOW);
}
