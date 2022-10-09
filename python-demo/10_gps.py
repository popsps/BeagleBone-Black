import serial

# allows you to use serial ports
import Adafruit_BBIO.UART as UART
import time

'''

'''

UART.setup("UART1")
GPS = serial.Serial('/dev/ttyO1', 9600)

while (1):
    # while there is no data on GPS
    while (GPS.inWaiting() == 0):
        pass
    NMEA = GPS.readline()
    print(NMEA)
