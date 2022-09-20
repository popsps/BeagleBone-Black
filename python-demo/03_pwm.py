import Adafruit_BBIO.PWM as PWM
import time

myPWM = "P8_13"
# duty cycle 0 percent * 3.3v, frequency: 1000 Hz
PWM.start(myPWM, 0, 1000)
# Duty cycle from 0% to 100%

for i in range(0, 5):
    DC = input("Enter Duty cycle: ")
    PWM.set_duty_cycle(myPWM, DC)

for i in range(0, 5):
    V = input("Enter Voltage[0 - 3.365]: ")
    DC = V/3.365 * 100
    PWM.set_duty_cycle(myPWM, DC)

PWM.stop(myPWM)
PWM.cleanup()
