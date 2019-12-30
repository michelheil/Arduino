## Sensor Description
The QMC5883L is a multi-chip three-axis magnetic sensor. This
surface -mount, small sized chip has integrated magnetic sensors with signal condition ASIC, targeted for high precision applications such as compassing, navigation and gaming in drone, robot, mobile and personal hand-held devices.

The QMC5883L is based on our state-of-the-art, high resolution,
magneto-resistive technology licensed from Honeywell AMR technology.
Along with custom-designed 16-bit ADC ASIC, it offers the advantages of low noise, high accuracy, low power consumption, offset cancellation and temperature compensation. QMC5883L enables 1° to 2° compass heading accuracy. The I2C serial bus allows for easy interface.

The QMC5883L is in a 3x3x0.9mm3 surface mount 16-pin land grid
array (LGA) package.

### FEATURES
* 3-Axis Magneto-Resistive Sensors in a 3x3x0.9mm3 Land Grid Array Package (LGA), guaranteed to operate over an extended temperature range of -40 °C to +85 °C.
* 16 Bit ADC With Low Noise AMR Sensors Achieves 5 Milli-Gauss Field Resolution
* Wide Magnetic Field Range (±8 Gauss)
* Temperature Compensated Data Output and Temperature Output
* I2C Interface with Standard and Fast Modes.
* Wide Range Operation Voltage (2.16V To 3.6V) and Low Power Consumption (75uA)
* Lead Free Package Construction
* Software And Algorithm Support Available

### BENEFIT
* Small Size for Highly Integrated Products. Signals Have
Been Digitized And Calibrated.
* Enables 1° To 2° Degree Compass Heading Accuracy, Allows for Navigation and LBS Applications
* Maximizes Sensor’s Full Dynamic Range and Resolution
* Automatically Maintains Sensor’s Sensitivity Under Wide Operating Temperature Range
* High-Speed Interfaces for Fast Data Communications. Maximum 200Hz Data Output Rate
* Compatible with Battery Powered Applications
* RoHS Compliance
* Compassing Heading, Hard Iron, Soft Iron, and Auto Calibration Libraries Available


Source: [maxim integrated data sheet](https://datasheets.maximintegrated.com/en/ds/DS3231.pdf)


### Link to data sheet
[DataSheet used for my Projects](https://github.com/michelheil/Arduino/lib/myGY271/datasheet)

### Pictures of sensor
<img src="https://github.com/michelheil/Arduino/blob/master/lib/myDS3231/pictures/DS3231_Front.jpg" width="250"> <img src="https://github.com/michelheil/Arduino/blob/master/lib/myDS3231/pictures/DS3231_Back.jpg" width="250">

### Other Links and References



## Library Documentation

### Dependencies
* <util/twi.h>
* <stdlib.h>
* <util/delay.h>
* <string.h>
* "myLOG.h"
* "myTWI.h"

### Init function



### APIs
Initializes DS3231 through the initialisation of TWI

```void DS3231_init(void);```

Request current seconds

```uint8_t DS3231_getSeconds(void);```

Request current minutes

```uint8_t DS3231_getMinutes(void);```

Request current hours

```uint8_t DS3231_getHours(void);```

Request current Seconds, Minutes, and Hours

```void DS3231_getTime(uint8_t * resPointer);```

Get time in format HH:mm:ss

```void DS3231_getTimeString(char * resPointer);```

Request current Date, Month, and Year

```void DS3231_getDMY(uint8_t * resPointer);```

Get date, month, year in format yyyy-DD-mm

```void DS3231_getDMYString(char * resPointer);```

Request current Seconds, Minutes, Hours, Day, Date, Month, and Year

```void DS3231_getTimestamp(uint8_t * resPointer);```

Get timestamp in format yyyy-MM-dd'T'HH:mm:ss

```void DS3231_getTimestampString(char * resPointer);```

Get day as String

```void DS3231_getDayString(char * resPointer);```

Combine the bits of the DS3231 register bytes into decimal numbers

```uint8_t DS3231_combineRegisterBits(uint8_t rawData);```


#### Helper Functions
ToDo: DS3231_combineRegisterBits als helper functions 


## Example
Motivation and what to do
### Picture of wiring
Fritzing
### Dependencies
Which other utils are required

