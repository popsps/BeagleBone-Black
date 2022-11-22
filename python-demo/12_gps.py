'''
$GPGGA,194530.000,3051.8007,N,10035.9989,W,1,4,2.18,746.4,M,-22.2,M,,*6B
 
$GPRMC,194530.000,A,3051.8007,N,10035.9989,W,1.49,111.67,310714,,,A*74
type,time,active or void,latitude,hemisphere(N/S), longitude, hemisphere, knots(speed) 

3051.8007
30 deg(rest)
51.8007 min(last 7)

My Latitude: 30 Degrees 51.8066 minuets N
My Longitude: 100 Degrees 36.0005 minuets W
-->

Google earth: 30 51.8066, -100 36.0005

'''
import logging
import serial

# allows you to use serial ports
import Adafruit_BBIO.UART as UART
import time

# Initialize UART1
UART.setup("UART1")
# Initialize Serial Port
# ser = serial.Serial('/dev/ttyO1', 9600)
ser = serial.Serial('/dev/ttyO4', 9600)


class GPS:
    def __init__(self) -> None:
        # This sets up variables for useful commands.
        # This set is used to set the rate the GPS reports
        self.UPDATE_10_sec = "$PMTK220,10000*2F\r\n".encode()  # Update Every 10 Seconds
        self.UPDATE_5_sec = "$PMTK220,5000*1B\r\n".encode()   # Update Every 5 Seconds
        self.UPDATE_1_sec = "$PMTK220,1000*1F\r\n" .encode()  # Update Every One Second
        self.UPDATE_200_msec = "$PMTK220,200*2C\r\n".encode()   # Update Every 200 Milliseconds
        # This set is used to set the rate the GPS takes measurements
        self.MEAS_10_sec = "$PMTK300,10000,0,0,0,0*2C\r\n" .encode()  # Measure every 10 seconds
        self.MEAS_5_sec = "$PMTK300,5000,0,0,0,0*18\r\n".encode()   # Measure every 5 seconds
        self.MEAS_1_sec = "$PMTK300,1000,0,0,0,0*1C\r\n".encode()   # Measure once a second
        self.MEAS_200_msec = "$PMTK300,200,0,0,0,0*2F\r\n" .encode()  # Measure 5 times a second
        # Set the Baud Rate of GPS
        self.BAUD_57600 = "$PMTK251,57600*2C\r\n".encode()   # Set Baud Rate at 57600
        # default
        self.BAUD_9600 = "$PMTK251,9600*17\r\n".encode()   # Set 9600 Baud Rate
        # Commands for which NMEA Sentences are sent
        # Send only the GPRMC Sentence
        self.GPRMC_ONLY = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n".encode() 
        # Send GPRMC AND GPGGA Sentences
        self.GPRMC_GPGGA = "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n".encode() 
        self.SEND_ALL = "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n".encode()   # Send All Sentences
        self.SEND_NOTHING = "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n".encode()   # Send Nothing

    def read(self):
        # clearing serial port
        ser.flushInput()
        ser.flushInput()
        while (ser.inWaiting() == 0):
            pass
        while (ser.inWaiting() == 0):
            pass
        self.NMEA1: str = ser.readline().decode()
        while (ser.inWaiting() == 0):
            pass
        self.NMEA2: str = ser.readline().decode()
        NMEA1_array = self.NMEA1.split(',')
        NMEA2_array = self.NMEA2.split(',')
        if (NMEA1_array[0] == '$GPRMC'):
            self.timeUTC = NMEA1_array[1][:-8]+':' + \
                NMEA1_array[1][-8:-6]+':'+NMEA1_array[1][-6:-4]
            self.latDeg = NMEA1_array[3][:-7]
            self.latMin = NMEA1_array[3][-7:]
            self.latHem = NMEA1_array[4]
            self.lonDeg = NMEA1_array[5][:-7]
            self.lonMin = NMEA1_array[5][-7:]
            self.lonHem = NMEA1_array[6]
            self.knots = NMEA1_array[7]
        if NMEA1_array[0] == '$GPGGA':
            self.fix = NMEA1_array[6]
            self.altitude = NMEA1_array[9]
            self.sats = NMEA1_array[7]
        if NMEA2_array[0] == '$GPRMC':
            self.timeUTC = NMEA2_array[1][:-8]+':' + \
                NMEA1_array[1][-8:-6]+':'+NMEA1_array[1][-6:-4]
            self.latDeg = NMEA2_array[3][:-7]
            self.latMin = NMEA2_array[3][-7:]
            self.latHem = NMEA2_array[4]
            self.lonDeg = NMEA2_array[5][:-7]
            self.lonMin = NMEA2_array[5][-7:]
            self.lonHem = NMEA2_array[6]
            self.knots = NMEA2_array[7]
        if NMEA2_array[0] == '$GPGGA':
            self.fix = NMEA2_array[6]
            self.altitude = NMEA2_array[9]
            # how many satellites we are looking at
            self.sats = NMEA2_array[7]


myGPS = GPS()
print("Initializing GPS...")

# communication speed
ser.write(myGPS.BAUD_57600)
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
GPSData = open('./GPS_DATA/01.dat', 'w+')
while (1):
    myGPS.read()
    try:
        print(myGPS.NMEA1)
        print(myGPS.NMEA2)
        if myGPS.fix != 0:
            print('Universal Time: ', myGPS.timeUTC)
            print('You are Tracking: ', myGPS.sats, ' satellites')
            print('My Latitude: ', myGPS.latDeg, 'Degrees ',
                  myGPS.latMin, ' minutes ', myGPS.latHem)
            print('My Longitude: ', myGPS.lonDeg, 'Degrees ',
                  myGPS.lonMin, ' minutes ', myGPS.lonHem)
            print('My Speed: ', myGPS.knots)
            print('My Altitude: ', myGPS.altitude)
            latDec = float(myGPS.latDeg) + float(myGPS.latMin) / 60.0
            lonDec = float(myGPS.lonDeg) + float(myGPS.lonMin) / 60.0
            if myGPS.latHem == 'S':
                latDec = -1 * latDec
            if myGPS.latHem == 'W':
                lonDec = -1 * lonDec
            alt = myGPS.altitude
            out = str(lonDec) + ', ' + str(latDec) + ', ' + alt
            print(out)
            GPSData.write(out)
    except Exception as e:
        logging.error(e)
        GPSData.close()
GPSData.close()
