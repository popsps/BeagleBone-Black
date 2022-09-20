#ifndef GPIO_H
#define GPIO_H

/***************************************************************
 * Constants
 ***************************************************************/

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64

typedef enum PIN_DIRECTION { INPUT_PIN = 0, OUTPUT_PIN = 1 } PIN_DIRECTION;
typedef enum PIN_VALUE { LOW = 0, HIGH = 1 } PIN_VALUE;

/***************************************************************
 * Functions
 ***************************************************************/

/**
 * Initialize gpio_no to the board. This will create a directoy called
 * gpiogpio_no. For example gpio60.
 **/
int gpio_export(unsigned int gpio_no);
int gpio_unexport(unsigned int gpio_no);
int gpio_set_direction(unsigned int gpio_no, PIN_DIRECTION out_flag);
int gpio_set_value(unsigned int gpio_no, PIN_VALUE value);
int gpio_get_value(unsigned int gpio_no, unsigned int* value);
int gpio_set_edge(unsigned int gpio_no, char* edge);
int gpio_fd_open(unsigned int gpio_no);
int gpio_fd_close(int fd);

#endif