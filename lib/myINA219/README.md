## Sensor Description
The INA219 is a current shunt and power monitor with an I2C- or SMBUS-compatible interface. The device monitors both shunt voltage drop and bus supply voltage, with programmable conversion times and filtering. A programmable calibration value, combined with an internal multiplier, enables direct readouts of current in amperes. An additional multiplying register calculates power in watts. The I2C- or SMBUS-compatible interface features 16 programmable addresses.

The INA219 is available in two grades: A and B. The B grade version has higher accuracy and higher precision specifications.

The INA219 senses across shunts on buses that can vary from 0 to 26 V. The device uses a single 3- to 5.5-V supply, drawing a maximum of 1 mA of supply current. The INA219 operates from –40°C to 125°C.

### Features
* Senses Bus Voltages from 0 to 26 V
* Reports Current, Voltage, and Power
* 16 Programmable Addresses
* High Accuracy: 0.5% (Maximum) Over
* Temperature (INA219B)
* Filtering Options
* Calibration Registers
* SOT23-8 and SOIC-8 Packages
 
Source: [Texas Instruments data sheet](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=2ahUKEwjcxO7Kj9bmAhVCyaQKHZTPDpoQFjAAegQIAhAC&url=http%3A%2F%2Fwww.ti.com%2Flit%2Fgpn%2Fina219&usg=AOvVaw1Fu2NvgdXtLu0A4rZoCf6m)


### Link to data sheet
[DataSheet used for my Projects](https://github.com/michelheil/Arduino/lib/myINA219/datasheet)

### Pictures of sensor
<img src="https://github.com/michelheil/Arduino/blob/master/lib/myDS3231/pictures/DS3231_Front.jpg" width="250"> <img src="https://github.com/michelheil/Arduino/blob/master/lib/myDS3231/pictures/DS3231_Back.jpg" width="250">

### Other Links and References
[Adafruit Link](https://www.adafruit.com/product/904)


## Library Documentation

### Dependencies
* <util/twi.h>
* <stdlib.h>
* <util/delay.h>
* <string.h>
* "myLOG.h"
* "myTWI.h"

### Init function
Follow these [instructions](https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/arduino-usage) 
to set up DS3231 for first time


### APIs


#### Helper Functions
ToDo: DS3231_combineRegisterBits als helper functions 


## Example
Motivation and what to do
### Picture of wiring
Fritzing
### Dependencies
Which other utils are required

