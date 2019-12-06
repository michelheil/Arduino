## Sensor Description
The DS3231 is a low-cost, extremely accurate I2C real-time clock (RTC) with an integrated temperature-
compensated crystal oscillator (TCXO) and crystal. The device incorporates a battery input, and maintains
accurate timekeeping when main power to the device is interrupted. The integration of the crystal resonator
enhances the long-term accuracy of the device as well as reduces the piece-part count in a manufacturing line.
The DS3231 is available in commercial and industrial temperature ranges, and is offered in a 16-pin, 300-mil
SO package. 

The RTC maintains seconds, minutes, hours, day, date, month, and year information. The date at the end of the
month is automatically adjusted for months with fewer than 31 days, including corrections for leap year. The
clock operates in either the 24-hour or 12-hour format with an AM/PM indicator. Two programmable time-of-day
alarms and a programmable square-wave output are provided. Address and data are transferred serially
through an I2C bidirectional bus. 

A precision temperature-compensated voltage reference and comparator circuit monitors the status of V CC to
detect power failures, to provide a reset output, and to automatically switch to the backup supply when necessary.
Additionally, the RST pin is monitored as a pushbutton input for generating a μP reset.

Source: [maxim integrated data sheet](https://datasheets.maximintegrated.com/en/ds/DS3231.pdf)


### Link to data sheet
[DataSheet used for my Projects](https://github.com/michelheil/Arduino/lib/myDS3231/datasheet)

### Pictures of sensor
<img src="https://github.com/michelheil/Arduino/blob/master/lib/myDS3231/pictures/DS3231_Front.jpg" width="250"> <img src="https://github.com/michelheil/Arduino/blob/master/lib/myDS3231/pictures/DS3231_Back.jpg" width="250">

### Other Links and References
[Adafruit Shop DS3231](https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/overview)


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

