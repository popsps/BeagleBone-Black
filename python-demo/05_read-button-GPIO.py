# Digital reads allow pins to read true or false, or high or low (binary)
# When doing digital reads from the pins that we have setup as inputs(P9_13, P9_15 in this case),
# the BeagleBone black wants to operate at 3.3 volts and
# so we need to make sure that we're refrencing things to 3.3 volts and not 5 volts and so we're very careful that
# our rail is established by pin P9_3 not P9_5

# p9_1 is ground. So we're making a 3.3 volt rail. use 1000ohm pull down resistor or above. 
# but not below that.

import Adafruit_BBIO.GPIO as GPIO
import time

topButton = "P9_11"
bottomButton = "P9_13"

GPIO.cleanup()
GPIO.setup(topButton, GPIO.IN)
GPIO.setup(bottomButton, GPIO.IN)

while (1):
  if (GPIO.input(topButton)):
    print("Top Button is pressed")
  if(GPIO.input(bottomButton)):
    print("Bottom Button is pressed")
  if GPIO.input(topButton) and GPIO.input(bottomButton):
    break;
  time.sleep(.2)

print("Goodby, Come again soon")
GPIO.cleanup()
    
    
