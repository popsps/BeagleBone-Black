/**
 * libary for working with GPIO pinout in Beaglebone.
 */
#ifndef GPIO_H
#define GPIO_H

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/***************************************************************
 * Constants
 ***************************************************************/

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64

typedef enum PIN_DIRECTION_ENUM { INPUT_PIN = 0, OUTPUT_PIN = 1 } PIN_DIRECTION;
typedef enum PIN_VALUE_ENUM { LOW = 0, HIGH = 1 } PIN_VALUE;

/***************************************************************
 * Functions
 ***************************************************************/

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_direction(unsigned int gpio, PIN_DIRECTION out_flag);
int gpio_set_value(unsigned int gpio, PIN_VALUE value);
int gpio_get_value(unsigned int gpio);
char* gpio_get_direction(unsigned int gpio);
int gpio_set_edge(unsigned int gpio, char* edge);
int gpio_fd_open(unsigned int gpio);
int gpio_fd_close(int fd);

#endif