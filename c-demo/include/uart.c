#include "uart.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include <unistd.h>

/**
 * read uart 1, 2, 3, 4
 */
int uart_init(int pin) {
  char path[200];
  snprintf(path, sizeof(path), UART_PATH "%d", pin);
  fd = open(path, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    perror("UART: Failed to open the device.\n");
    return -1;
  }
  struct termios serials;
  if (tcgetattr(fd, &serials) != 0) {
    perror("Unable to retrieve port attributes.\n");
    return -1;
  }
  if (cfsetispeed(&serials, B115200) < 0) {
    perror("Input baud rate not successfully set.\n");
  }
  serials.c_iflag = IGNPAR;
  serials.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
  serials.c_iflag = IGNPAR;  // ignore partity errors, CR -> newline
  // serials.c_iflag = IGNPAR | ICRNL;  // ignore partity errors, CR -> newline
  serials.c_oflag = 0;
  serials.c_lflag = 0;
  // serials.c_cc[VMIN] = 45;  // works for 1
  // serials.c_cc[VTIME] = 0;
  // serials.c_cflag = B1152000 | CS8 | CREAD | CLOCAL;
  // Set up the communications serials:
  // 9600 baud, 8-bit, enable receiver, no modem control lines

  tcflush(fd, TCIFLUSH);             // discard file information not transmitted
  tcsetattr(fd, TCSANOW, &serials);  // changes occur immmediately
  return 1;
}

int uart_close() {
  int status = close(fd);
  if (status < 0) {
    perror("UART: Failed to close the device.\n");
  }
  return status;
}

char* serial_read_line() {
  int count = 0;
  const int max_size = 1024;
  int i = 0;
  char* buffer = malloc(sizeof(char) * max_size);

  memset(buffer, 0, sizeof(char) * max_size);
  // printf("I'm here serial_read_line 1 %s\n", buffer);
  while (count <= 0) {
    char c;
    count = read(fd, (void*)(&c), 1);
    if (c == '\n') {
      buffer[i] = '\0';
      return buffer;
    } else {
      buffer[i] = c;
      i += 1;
    }
  }
  if (count <= 0) {
    return NULL;
  }
}

int serial_write(char* payload) {
  int count = write(fd, payload, strlen(payload) + 1);
  if (count < 0) {
    perror("UART: Failed to write to the device.\n");
  }
  return count;
}

// if (strstr(request, "favicon") != NULL) {
//     // contains
// }