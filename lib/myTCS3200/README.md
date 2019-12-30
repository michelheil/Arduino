## Sensor Description
The TCS3200 programmable color light-to-frequency converter
combines configurable silicon photodiodes and a
current-to-frequency converter on a single monolithic CMOS
integrated circuit. The output is a square wave (50% duty cycle)
with frequency directly proportional to light intensity
(irradiance).

The full-scale output frequency can be scaled by one of three
preset values via two control input pins. Digital inputs and
digital output allow direct interface to a microcontroller or
other logic circuitry. Output enable (OE) places the output in
the high-impedance state for multiple-unit sharing of a
microcontroller input line.

In the TCS3200, the light-to-frequency converter reads an 8 × 8
array of photodiodes. Sixteen photodiodes have blue filters, 16
photodiodes have green filters, 16 photodiodes have red filters,
and 16 photodiodes are clear with no filters.

The four types (colors) of photodiodes are interdigitated to
minimize the effect of non-uniformity of incident irradiance. All
photodiodes of the same color are connected in parallel. Pins
S2 and S3 are used to select which group of photodiodes (red,
green, blue, clear) are active. Photodiodes are 110μm × 110μm
in size and are on 134μm centers.

Source: [maxim integrated data sheet](https://datasheets.maximintegrated.com/en/ds/DS3231.pdf)


### Link to data sheet
[DataSheet used for my Projects](https://github.com/michelheil/Arduino/lib/myTCS3200/datasheet)

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

