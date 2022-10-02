

"""
The PWM pins controll the duty_cycle based on the number of push buttons pressed. 
if button 1 pressed DC will be increased. If button 2 is pressed DC will be decreased.
P9_2 DGND is Ground.
P9_4 VDD 3.3V supplies 3.3 volts to the circuit.
P9_23 Digital input
P9_27 Digital input
P9_14 PWM

(digital_read, duty_cycle): (0, 0)
after the digital_read pushed 10 times we want duty_cyle to be 100
(digital_read, duty_cycle): (10, 100)
Our formula: duty_cycle = c**BP - b where c and b are constants
Our formula: duty_cycle = 1.58647**BP - 1
"""

import Adafruit_BBIO.PWM as PWM
import Adafruit_BBIO.GPIO as GPIO
import time
import signal
import sys


def signal_handler(sig, frame):
    print("Program exited with SIGINT", sig)
    PWM.stop(PWM14)
    PWM.cleanup()
    sys.exit(0)


signal.signal(signal.SIGINT, signal_handler)

digitalInput1 = "P9_23"
digitalInput2 = "P9_27"

GPIO.cleanup()
GPIO.setup(digitalInput1, GPIO.IN)
GPIO.setup(digitalInput2, GPIO.IN)

PWM14 = "P9_14"


PWM.start(PWM14, 0, 1000)
# PWM.set_duty_cycle(topOutPin, DC)

BP = 0
while (1):
    if (GPIO.input(digitalInput1) and BP < 10):
        BP += 1
        print("Button digitalInput1 is pressed", BP)
    if (GPIO.input(digitalInput2) and BP < 0):
        BP -= 1
        print("Button digitalInput2 is pressed", BP)
    duty_cycle = 1.5864**(BP)-1
    PWM.set_duty_cycle(PWM14, duty_cycle)
    print("[BP, duty_cycle] is: [{0}, {1}]".format(BP, duty_cycle))

    time.sleep(.2)


print("Goodbye, Come again soon")
