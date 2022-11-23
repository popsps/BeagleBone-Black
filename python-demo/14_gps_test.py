import serial
import Adafruit_BBIO.UART as UART
UART.setup("UART1")
serial = serial.Serial('/dev/ttyO4', 9600)
print('start reading...')
while (1):
    while serial.inWaiting() == 0:
        pass
    try:
        NMEA = serial.readline()
        # print(NMEA)
        print(NMEA.decode())
    except Exception as e:
        print(e)
