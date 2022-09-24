#include "gpio.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int gpio_export(unsigned int gpio) {
  int fd, len = 0;
  char path[MAX_BUF];

  fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY | O_TRUNC);
  if (fd < 0) {
    perror("gpio/export error");
    return EXIT_FAILURE;
  }
  len = snprintf(path, sizeof(path), "%d", gpio);
  write(fd, path, len);
  close(fd);
  return 0;
}

int gpio_unexport(unsigned int gpio) {
  int fd, len = 0;
  char path[MAX_BUF];

  fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY | O_TRUNC);
  if (fd < 0) {
    perror("gpio/unexport error");
    return EXIT_FAILURE;
  }
  len = snprintf(path, sizeof(path), "%d", gpio);
  write(fd, path, len);
  close(fd);
  return 0;
}

int gpio_set_direction(unsigned int gpio, PIN_DIRECTION out_flag) {
  char path[MAX_BUF];
  snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
  int fd = open(path, O_WRONLY);
  if (fd < 0) {
    perror("gpio/direction");
    return fd;
  }
  if (out_flag == OUTPUT_PIN)
    write(fd, "out", 4);
  else
    write(fd, "in", 3);
  close(fd);
  return 0;
}
int gpio_set_value(unsigned int gpio, PIN_VALUE value) {
  char path[MAX_BUF];
  snprintf(path, sizeof(path), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
  int fd = open(path, O_WRONLY);
  if (fd < 0) {
    perror("gpio/set-value");
    return fd;
  }
  if (value == LOW)
    write(fd, "0", 2);
  else
    write(fd, "1", 2);
  close(fd);
  return 0;
}