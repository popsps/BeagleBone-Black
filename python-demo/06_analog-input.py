

"""
Analog Inputs: Pins marked with AIN: AIN0, AIN1, AIN2, AIN3, AIN4, AIN5, AIN6
on the P9 header.

These inputs only want 0 - 1.8 volts. Do not put 3.3/5 volts on them. 

P9_32 VDD_ADC is a 1.8 volts pin.
P9_34 GND_ADC goes as ground with VDD_ADC 1.8 volts.

So in this example we're using P9_32 as refrence, P9_34 as ground, 
and P9_33 as the center tap (analog input).
"""

import Adafruit_BBIO.ADC as ADC
import time

AIN4 = "P9_33"
VDD_ADC = "P9_32"
GND_ADC = "P9_34"

GPIO.cleanup()
GPIO.setup(topButton, GPIO.IN)
GPIO.setup(bottomButton, GPIO.IN)

while (1):
  potValue = ADC.read(AIN4)
  print("The Potentiometer value is: ", potValue)
  time.sleep(.5)

print("Goodby, Come again soon")
GPIO.cleanup()
    
    
