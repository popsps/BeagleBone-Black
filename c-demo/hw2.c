#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "gpio.h"
#include "pins.h"

/**
 * Using GPIO 9_23 and GPIO 9_24 GPIO 2_25
 **/
typedef struct trafic_signal_struct {
  int red_light;
  int yellow_light;
  int green_light;
} trafic_signal;

void cleanUp(trafic_signal* signal);
void initilize(trafic_signal* signal);
void makeSignalRed(trafic_signal* signal);
void makeSignalYellow(trafic_signal* signal);
void makeSignalGreen(trafic_signal* signal);

int main(int argc, char* argv[]) {
  printf("CS 695 HW2 Simple Intersection with Opposing Traffic Lights\n");
  trafic_signal signal1;
  trafic_signal signal2;

  // init pins that are being used for each trafic signal 1
  signal1.red_light = P9_11;
  signal1.yellow_light = P9_12;
  signal1.green_light = P9_13;

  // init pins that are being used for each trafic signal 1
  signal2.red_light = P9_23;
  signal2.yellow_light = P9_24;
  signal2.green_light = P9_26;

  cleanUp(&signal1);
  cleanUp(&signal2);
  initilize(&signal1);
  initilize(&signal2);

  for (size_t i = 0; i < 5; i++) {
    sleep(2);
    makeSignalGreen(&signal1);
    makeSignalRed(&signal2);
    sleep(2);
    makeSignalYellow(&signal1);
    makeSignalYellow(&signal2);
    sleep(2);
    makeSignalRed(&signal1);
    makeSignalGreen(&signal2);
    sleep(2);
  }
  // unset all GPIO pins
  gpio_set_value(signal1.red_light, LOW);
  gpio_set_value(signal1.yellow_light, LOW);
  gpio_set_value(signal1.green_light, LOW);

  gpio_set_value(signal2.red_light, LOW);
  gpio_set_value(signal2.yellow_light, LOW);
  gpio_set_value(signal2.green_light, LOW);

  printf("Clean up Pins...\n");
  sleep(4);
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

void makeSignalRed(trafic_signal* signal){
  gpio_set_value(signal->red_light, HIGH);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, LOW);
}
void makeSignalYellow(trafic_signal* signal){
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, HIGH);
  gpio_set_value(signal->green_light, LOW);
}
void makeSignalGreen(trafic_signal* signal){
  gpio_set_value(signal->red_light, LOW);
  gpio_set_value(signal->yellow_light, LOW);
  gpio_set_value(signal->green_light, HIGH);
}