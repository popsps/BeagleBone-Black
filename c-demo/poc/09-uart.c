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

#define UPDATE_10_sec "$PMTK220,10000*2F\r\n"  // Update Every 10 Seconds
#define UPDATE_5_sec "$PMTK220,5000*1B\r\n"    // Update Every 5 Seconds
#define UPDATE_1_sec "$PMTK220,1000*1F\r\n"    // Update Every One Second
#define UPDATE_200_msec "$PMTK220,200*2C\r\n"  // Update Every 200 Milliseconds
// This set is used to set the rate the GPS takes measurements
#define MEAS_10_sec "$PMTK300,10000,0,0,0,0*2C\r\n"  // Measure every 10 seconds
#define MEAS_5_sec "$PMTK300,5000,0,0,0,0*18\r\n"    // Measure every 5 seconds
#define MEAS_1_sec "$PMTK300,1000,0,0,0,0*1C\r\n"    // Measure once a second
#define MEAS_200_msec "$PMTK300,200,0,0,0,0*2F\r\n"  // Measure 5 times a second
// Set the Baud Rate of GPS
#define BAUD_57600 "$PMTK251,57600*2C\r\n"  // Set Baud Rate at 57600
// default
#define BAUD_9600 "$PMTK251,9600*17\r\n"  // Set 9600 Baud Rate
// Commands for which NMEA Sentences are sent
// Send only the GPRMC Sentence
#define GPRMC_ONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
// Send GPRMC AND GPGGA Sentences
#define GPRMC_GPGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define SEND_ALL "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"      // Send All Sentences
#define SEND_NOTHING "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"  // Send Nothing

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
  options.c_iflag = IGNPAR | ICRNL;  // ignore partity errors, CR -> newline
  // TCIFLUSH flushes data received but not read.
  tcflush(file, TCIFLUSH);             // discard file information not transmitted
  tcsetattr(file, TCSANOW, &options);  // changes occur immmediately
  // send the string plus the null character
  // printf("sending %s:%d\n", argv[1], strlen(argv[1] + 1));
  // count = write(file, argv[1], strlen(argv[1]) + 1);
  // count = fprintf(f 09p, "\n");
  // count = fprintf(fp, "%s", argv[1]);
  printf("sleep thread for letting uart to catch up...\n");
  sleep(1);
  unsigned char transmit[256] = "$PMTK220,5000*1B\r\n";  // the string to send
  count = write(file, &transmit, strlen(transmit) + 1);
  sleep(1);
  strcpy(transmit, "$PMTK300,5000,0,0,0,0*18\r\n");
  count = write(file, &transmit, strlen(transmit) + 1);
  sleep(1);
  // strcpy(transmit, "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
  // count = write(file, &transmit, strlen(transmit) + 1);
  if (count < 0) {  // send the string
    perror("Failed to write to the output\n");
    return -1;
  } else {
    printf("writing serial configuration was successfull.\n");
  }
  sleep(2);  // required to make flush work, for some reason
  // TCIOFLUS flushes both data received but not read, and data written but not transmitted.
  tcflush(file, TCIOFLUSH);
  usleep(100000);
  // fseek(fp, 0, SEEK_SET);
  unsigned char receive[1024] = {0};  // declare a buffer for receiving data
  unsigned char buffer[1024] = {0};
  unsigned char output[1024] = {0};
  printf("Reading from UART:\n");
  count = 0;
  int c = 0;
  int i = 0;

  memset(buffer, 0, sizeof(buffer));

  while (1) {
    count = read(file, buffer + i, 1);
    if (count <= 0) {
      sleep(1);
    } else {
      if (buffer[i] == '\n') {
        time_t current_time;
        struct tm* utcTimeInfo;
        current_time = time(0);
        utcTimeInfo = gmtime(&current_time);
        char timeInfoBuffer[25] = {0};
        strftime(timeInfoBuffer, sizeof(timeInfoBuffer), "%Y-%m-%d %H:%M:%S", utcTimeInfo);
        if (buffer != NULL && buffer[0] != '\0') {
          buffer[i] = '\0';
          printf("%s - %s\n", timeInfoBuffer, buffer);
          // printf("%s", buffer);
        }
        // tcflush(file, TCIOFLUSH);
        memset(buffer, 0, sizeof(buffer));
        i = 0;
      } else {
        i += 1;
      }
    }
  }
  printf("buffer read: %s; count: %d\n", buffer, count);
  close(file);
  // fclose(fp);
  // printf("char: %hhX; len: %d\n", nmea[0], strlen(nmea));
  printf("Finished sending the message, exiting %d.\n", count);
  return 0;
}

// while (1) {
//   count = read(file, (void*)buffer, 5);
//   if (count < 0) {  // receive the data
//     perror("Failed to read from the input\n");
//     return -1;
//   }
//   if (count > 0) {
//     printf("rec: [%d]: %s\n", count, receive);
//     snprintf(output, sizeof(output), LDR_PATH "%s%s", output, buffer);
//     strncat(output, buffer, strlen(buffer));
//     memset(buffer, 0, sizeof(buffer));
//   }
// }