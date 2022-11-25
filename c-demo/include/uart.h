/**
 * libary for working with GPIO pinout in Beaglebone.
 */
#ifndef UART_H
#define UART_H

/***************************************************************
 * Constants
 ***************************************************************/

#define UART_PATH "/dev/ttyO"


/***************************************************************
 * Functions
 ***************************************************************/
int uart_init(int pin);
int uart_close();
char* serial_read_line();
int serial_write(char* payload);
#endif