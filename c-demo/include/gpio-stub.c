#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gpio.h"

int gpio_export(unsigned int gpio) {
  return 0;
}

int gpio_unexport(unsigned int gpio) { return 0; }

int gpio_set_direction(unsigned int gpio, PIN_DIRECTION out_flag) { return 0; }
int gpio_set_value(unsigned int gpio, PIN_VALUE value) { return 0; }