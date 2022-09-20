
# working with p9_12: GPIO_60 (Digital Pin)
# P9_2: Ground
# python3 digital_write_gpio.py 

import Adafruit_BBIO.GPIO as GPIO
import time

outPin = "P9_12"
GPIO.setup(outPin, GPIO.OUT)

for i in range(0, 5):
    GPIO.output(outPin, GPIO.HIGH)
    time.sleep(3)
    GPIO.output(outPin, GPIO.LOW)

GPIO.cleanup()
