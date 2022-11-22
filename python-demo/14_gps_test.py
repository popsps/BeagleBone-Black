import serial
import Adafruit_BBIO.UART as UART
UART.setup("UART1")
serial = serial.Serial('/dev/ttyO4', 9600)
while (1):
    print('start reading...')
    while serial.inWaiting() == 0:
        pass
    NMEA = serial.readline()
    print(NMEA)
    print(NMEA.decode())
    print(str(NMEA))
