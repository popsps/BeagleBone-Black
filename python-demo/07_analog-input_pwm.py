

"""
P9_32 VDD_ADC is a 1.8 volts pin.
P9_34 GND_ADC goes as ground with VDD_ADC 1.8 volts.
P9_33 Analog input
Analog inputs [0, 1] maps to PWM Duty cycle [0, 100] 

(analog_read, duty_cycle): (0, 0), (1, 100)
Our formula: duty_cycle = c**analog_read - b where c and b are constants
Our formula: duty_cycle = 101**analog_read - 1
"""

import Adafruit_BBIO.ADC as ADC
import Adafruit_BBIO.PWM as PWM
import time
import signal
import sys


def signal_handler(sig, frame):
    print("Program exited with SIGINT", sig)
    PWM.stop(PWM14)
    PWM.cleanup()
    sys.exit(0)


signal.signal(signal.SIGINT, signal_handler)
AIN4 = "P9_33"
PWM14 = "P9_14"
VDD_ADC = "P9_32"
GND_ADC = "P9_34"
ADC.setup()
PWM.start(PWM14, 0, 1000)
# PWM.set_duty_cycle(topOutPin, DC)

while (1):
    # read as percentage [0, 1]
    potentiometer_value = float(ADC.read(AIN4))
    duty_cycle = 101**potentiometer_value - 1
    print("[potentiometer_value, duty_cycle] is: [{0}, {1}]".format(
        potentiometer_value, duty_cycle))
    PWM.set_duty_cycle(PWM14, duty_cycle)
    time.sleep(.5)

print("Goodby, Come again soon")
