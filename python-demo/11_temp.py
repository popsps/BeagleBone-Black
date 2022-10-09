import Adafruit_BBIO.ADC as ADC
import time

'''
- connect pin 1 (left) to power between 2.7V and 5.5V. (P9_3: VDD 3.3V)
- connect pin 3 (right) to ground. in this case (P9_34: GNDA_ADC)
- connect pin 2 to analog in the microcontroller. (P9_40: AIN1)

How to read the analog temperature:
0V at -50C
1.75V at 125C
Temp C = 100 * (reading in V) - 50

Temp in °C = [(Vout in mV) - 500] / 10

Output range: 0.1V (-40°C) to 2.0V (150°C) but accuracy decreases after 125°C
Power supply: 2.7V to 5.5V only, 0.05 mA current draw
'''


sensor_pin = 'P9_40'

ADC.setup()

while True:
    reading = ADC.read(sensor_pin)
    millivolts = reading * 1800  # 1.8V reference = 1800 mV
    temp_c = (millivolts - 500) / 10
    temp_f = (temp_c * 9/5) + 32
    print('mv=%d C=%d F=%d' % (millivolts, temp_c, temp_f))
    time.sleep(1)
