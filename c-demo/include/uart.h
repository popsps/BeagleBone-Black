/**
 * libary for working with GPIO pinout in Beaglebone.
 */
#ifndef UART_H
#define UART_H

/***************************************************************
 * Constants
 ***************************************************************/

#define UART_PATH "/dev/ttyO"

#define UPDATE_10_SEC "$PMTK220,10000*2F\r\n"  // Update Every 10 Seconds
#define UPDATE_5_SEC "$PMTK220,5000*1B\r\n"    // Update Every 5 Seconds
#define UPDATE_1_SEC "$PMTK220,1000*1F\r\n"    // Update Every One Second
#define UPDATE_200_MSEC "$PMTK220,200*2C\r\n"  // Update Every 200 Milliseconds
// This set is used to set the rate the GPS takes measurements
#define MEAS_10_SEC "$PMTK300,10000,0,0,0,0*2C\r\n"  // Measure every 10 seconds
#define MEAS_5_SEC "$PMTK300,5000,0,0,0,0*18\r\n"    // Measure every 5 seconds
#define MEAS_1_SEC "$PMTK300,1000,0,0,0,0*1C\r\n"    // Measure once a second
#define MEAS_200_MSEC "$PMTK300,200,0,0,0,0*2F\r\n"  // Measure 5 times a second
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

/***************************************************************
 * Functions
 ***************************************************************/
int uart_init(int pin);
int uart_close();
int serial_flush();
char* serial_read_line();
int serial_write(char* payload);
#endif