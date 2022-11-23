#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include <unistd.h>

/**
 * Enabling UART
 *
 * BB-UART1-00A0 => maps UART1 to P9.24 (Tx) and P9.26 (Rx)
 * BB-UART2-00A0 => maps UART2 to P9.21 (Tx) and P9.22 (Rx)

 * You need to edit /boot/uEnv.txt:

 * Change these two lines:

 * #uboot_overlay_addr6=/lib/firmware/<file6>.dtbo
 * #uboot_overlay_addr7=/lib/firmware/<file7>.dtbo

 * to

 * uboot_overlay_addr6=/lib/firmware/BB-UART4-00A0.dtbo
 * uboot_overlay_addr7=/lib/firmware/BB-UART2-00A0.dtbo

 * and then reboot.
 *
 * The UARTs are connected to /dev/ttySn:
 * UART0 (console header) /dev/ttyS0 -- note: /sbin/agetty is running on this port
 * UART1 P9.24,P9.26 /dev/ttyS1
 * UART2 P9.21,P9.22 /dev/ttyS2
 * UART3 P9.42 (tx only) /dev/ttyS3
 * UART4 P9.13,P9.11 /dev/ttyS4
 * UART5 P8.37,P8.38 /dev/ttyS5
 * ls -l ttyO*
 * echo "Using echo" >> /dev/ttyO4
 * sudo apt-get install minicom
 * sudo minicom -b 115200 -D /dev/ttyO4
 * sudo picocom -b 115200 /dev/ttyO4
 * groups $USER if not dialout then sudo gpasswd --add $USER dialout
 * sudo chmod 666 /dev/ttyO4
*/
int main(int argc, char* argv[]) {
  int file, count;
  if (argc != 2) {
    printf("Please pass a message string to send, exiting!\n");
    return -2;
  }
  file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NDELAY);
  // FILE* fp = fopen("/dev/ttyO4", "r+");
  if (file < 0) {
    perror("UART: Failed to open the device.\n");
    return -1;
  }
  struct termios options;
  tcgetattr(file, &options);
  // options.c_cflag = B1152000 | CS8 | CREAD | CLOCAL;
  // Set up the communications options:
  // 9600 baud, 8-bit, enable receiver, no modem control lines
  options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
  options.c_iflag = IGNPAR | ICRNL;    // ignore partity errors, CR -> newline
  tcflush(file, TCIFLUSH);             // discard file information not transmitted
  tcsetattr(file, TCSANOW, &options);  // changes occur immmediately
  // send the string plus the null character
  // printf("sending %s:%d\n", argv[1], strlen(argv[1] + 1));
  // count = write(file, argv[1], strlen(argv[1]) + 1);
  // count = fprintf(f 09p, "\n");
  // count = fprintf(fp, "%s", argv[1]);
  unsigned char transmit[18] = "Hello BeagleBone!";  // the string to send
  if ((count = write(file, &transmit, 18)) < 0) {    // send the string
    perror("Failed to write to the output\n");
    return -1;
  }
  usleep(100000);
  // fseek(fp, 0, SEEK_SET);
  unsigned char receive[100];  // declare a buffer for receiving data
  printf("Reading from UART:\n");
  count = 0;
  int i = 0;

  while (1) {
    count = read(file, (void*)receive, 100);
    if (count < 0) {  // receive the data
      perror("Failed to read from the input\n");
      return -1;
    }
    if (count == 0)
      printf("There was no data available to read!\n");
    else {
      printf("The following was read in [%d]: %s\n", count, receive);
    }
  }

  unsigned char buffer[1024] = {0};

  while (1) {
    count = read(file, buffer + i, 1);
    if (buffer[i] == '\n') {
      printf("buffer read: %s", buffer);
      memset(buffer, 0, sizeof(buffer));
      i = 0;
    } else {
      i += count;
    }
    // if (count > 0) {
    //   printf("buffer read count: %c %d\n", buffer[i], count);
    // }
  }
  printf("buffer read: %s; count: %d\n", buffer, count);
  close(file);
  // fclose(fp);
  printf("Finished sending the message, exiting %d.\n", count);
  return 0;
}