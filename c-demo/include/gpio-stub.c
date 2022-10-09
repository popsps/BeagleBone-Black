/**
 * Stub implemention of GPIO.c. this should be used for testings.
*/
#include "gpio.h"

int gpio_export(unsigned int gpio) { return 0; }

int gpio_unexport(unsigned int gpio) { return 0; }

int gpio_set_direction(unsigned int gpio, PIN_DIRECTION out_flag) { return 0; }
int gpio_set_value(unsigned int gpio, PIN_VALUE value) { return 0; }

int gpio_get_value(unsigned int gpio) {
  int isHold = 0;
  char buffer[MAX_BUF] = {0};
  if (fgets(buffer, MAX_BUF, stdin) != NULL) {
    isHold = atoi(buffer);
  }
  return isHold;
}

char* gpio_get_direction(unsigned int gpio) { return NULL; }