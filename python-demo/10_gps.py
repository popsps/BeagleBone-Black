'''

'''
import serial

# allows you to use serial ports
import Adafruit_BBIO.UART as UART
import time

# Initialize UART1
UART.setup("UART1")
# Initialize Serial Port
ser = serial.Serial('/dev/ttyO1', 9600)


class GPS:
    def __init__(self) -> None:
        # This sets up variables for useful commands.
        # This set is used to set the rate the GPS reports
        UPDATE_10_sec = "$PMTK220,10000*2F\r\n"  # Update Every 10 Seconds
        UPDATE_5_sec = "$PMTK220,5000*1B\r\n"  # Update Every 5 Seconds
        UPDATE_1_sec = "$PMTK220,1000*1F\r\n"  # Update Every One Second
        UPDATE_200_msec = "$PMTK220,200*2C\r\n"  # Update Every 200 Milliseconds
        # This set is used to set the rate the GPS takes measurements
        MEAS_10_sec = "$PMTK300,10000,0,0,0,0*2C\r\n"  # Measure every 10 seconds
        MEAS_5_sec = "$PMTK300,5000,0,0,0,0*18\r\n"  # Measure every 5 seconds
        MEAS_1_sec = "$PMTK300,1000,0,0,0,0*1C\r\n"  # Measure once a second
        MEAS_200_msec = "$PMTK300,200,0,0,0,0*2F\r\n"  # Measure 5 times a second
        # Set the Baud Rate of GPS
        BAUD_57600 = "$PMTK251,57600*2C\r\n"  # Set Baud Rate at 57600
        # default
        BAUD_9600 = "$PMTK251,9600*17\r\n"  # Set 9600 Baud Rate
        # Commands for which NMEA Sentences are sent
        # Send only the GPRMC Sentence
        GPRMC_ONLY = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
        # Send GPRMC AND GPGGA Sentences
        GPRMC_GPGGA = "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
        SEND_ALL = "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"  # Send All Sentences
        SEND_NOTHING = "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"  # Send Nothing


myGPS = GPS()
print("Initializing GPS...")
ser.flushInput()
# communication speed
ser.write(BAUD_57600)
time.sleep(1)
# serial port is capable of transferring a maximum of 57600 bits per second
ser.baudrate = 57600
# How often it reports
ser.write(myGPS.UPDATE_200_msec)
time.sleep(1)
# How often the GPS makes measurements
ser.write(myGPS.MEAS_200_msec)
time.sleep(1)
# ser.write(myGPS.GPRMC_ONLY)
ser.write(myGPS.GPRMC_GPGGA)
time.sleep(1)
print("GPS is Initialized")
while (1):
    # clear buffers
    ser.flushInput()
    ser.flushInput()
    # while there is no data on GPS
    while (ser.inWaiting() == 0):
        pass
    NMEA1 = ser.readline()
    while (ser.inWaiting() == 0):
        pass
    NMEA2 = ser.readline()
    print(NMEA1)
    print(NMEA2)
