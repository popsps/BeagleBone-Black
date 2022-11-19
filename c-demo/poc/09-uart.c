#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include <unistd.h>
// sudo apt-get install minicom
// sudo minicom -b 115200 -D /dev/tty4
// echo "Using echo" >> /dev/tty4
int main(int argc, char* argv[]) {
  int file, count;
  if (argc != 2) {
    printf("Please pass a message string to send, exiting!\n");
    return -2;
  }
  file = open("/dev/tty4", O_RDWR | O_NOCTTY | O_NDELAY);
  if (file < 0) {
    perror("UART: Failed to open the device.\n");
    return -1;
  }
  struct termios options;
  tcgetattr(file, &options);
  options.c_cflag = B1152000 | CS8 | CREAD | CLOCAL;
  options.c_iflag = IGNPAR | ICRNL;
  tcflush(file, TCIFLUSH);
  tcsetattr(file, TCSANOW, &options);
  // send the string plus the null character
  count = write(file, argv[1], strlen(argv[1] + 1));
  if (count < 0) {
    perror("UART Failed to write to the output.\n");
    return -1;
  }
  close(file);
  printf("Finished sending the message, exiting %d.\n", count);
  return 0;
}