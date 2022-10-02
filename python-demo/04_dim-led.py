import Adafruit_BBIO.PWM as PWM
import time

# User purple pins for PWM

topOutPin = "P9_14"
bottomOutPin = "P9_22"
# duty cycle 0 percent * 3.3v, frequency: 1000 Hz
PWM.start(topOutPin, 0, 1000)
PWM.start(bottomOutPin, 0, 1000)

for i in range(0, 5):
    V = float(input("Enter Voltage for the top LED[0 - 3.365]: "))
    DC = V/3.365 * 100
    PWM.set_duty_cycle(topOutPin, DC)
    V = float(input("Enter Voltage for the bottom LED[0 - 3.365]: "))
    DC = V/3.365 * 100
    PWM.set_duty_cycle(bottomOutPin, DC)

time.sleep(1)
PWM.stop(topOutPin)
PWM.stop(bottomOutPin)
PWM.cleanup()
