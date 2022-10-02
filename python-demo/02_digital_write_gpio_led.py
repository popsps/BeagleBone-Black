# working with p9_11: UART4_RXD (Digital Pin)
# working with p9_12: GPIO_60 (Digital Pin)
# P9_2: Ground
# python3 digital_write_gpio.py 

import Adafruit_BBIO.GPIO as GPIO
import time

topOutPin = "P9_12"
bottomOutPin = "P9_11"
GPIO.setup(topOutPin, GPIO.OUT)
GPIO.setup(bottomOutPin, GPIO.OUT)

for i in range(0, 7):
    GPIO.output(bottomOutPin, GPIO.LOW)
    GPIO.output(topOutPin, GPIO.HIGH)
    time.sleep(1)
    GPIO.output(bottomOutPin, GPIO.HIGH)
    GPIO.output(topOutPin, GPIO.LOW)
    time.sleep(1)

GPIO.output(topOutPin, GPIO.LOW)
GPIO.output(bottomOutPin, GPIO.LOW)
GPIO.cleanup()

