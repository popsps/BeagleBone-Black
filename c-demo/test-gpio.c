#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "gpio.h"
#include "pins.h"


// GPIO1_28 = 1 * 32 + 18 = 60 P9_12

// GPIO0_15 = 0 * 32 + 15 = 15 P9_24
unsigned int BOTTOM_GPIO = 15;

int main(int argc, char* argv[]) {
  printf("Testing the GPIO pins\n");
  gpio_export(P9_12);
  gpio_set_direction(P9_12, OUTPUT_PIN);
  gpio_export(P9_11);
  gpio_set_direction(P9_11, OUTPUT_PIN);

  for (size_t i = 0; i < 5; i++) {
    gpio_set_value(P9_12, HIGH);
    gpio_set_value(P9_11, LOW);
    sleep(1);
    gpio_set_value(P9_12, LOW);
    gpio_set_value(P9_11, HIGH);
    // usleep(200000);
    sleep(1);
  }
  gpio_set_value(P9_12, LOW);
  gpio_set_value(P9_11, LOW);
  gpio_unexport(P9_12);
  gpio_unexport(P9_11);

  printf("Testing the GPIO pins Done\n");
}