## Sensor Description
The AMG8833 is a Grid-Eye infrared array sensor in 14 pin SMD module. This high precision infrared 
array sensor based on advanced MEMS technology. It detects the heat (infrared rays) of the human body
and other objects. Typical applications include high function home appliances such as microwaves 
and air-conditioners, energy saving at office such as air-conditioning/lighting control, digital 
signage and automatic doors/elevators.

- Temperature detection of two-dimensional area is 8 x 8 (64 pixels)
- Operating voltage is 3.3VDC
- High performance type high gain amplification
- Temperature accuracy is ±2.5°C or ±4.5°F (typical)
- Human detection distance is 7m or less (reference value) 22.966ft
- Noise equivalent temperature difference is 0.05°C or 32.900°F at 1Hz
- Viewing angle is 60° and optical axis gap is within ±5.6°(typical)
- Current consumption is 4.5mA (normal mode), 0.2mA (sleep mode), 0.8mA (stand-by mode)
- I2C interface and frame rate is typically 10frames/sec or 1frame/sec
- Time to enable communication after setup is 50ms & time to stabilize output after setup is 15ms

Source: [Newark Shop](https://www.newark.com/panasonic/amg8833/temperature-sensor-ir-2-5deg-c/dp/05AC7504?mckv=My7ydwSy_dc|pcrid|76965866888829|plid||kword|amg8833|match|bp|slid||&msclkid=b65d706b36cc132e1dce2d811d0851c9&CMP=KNC-BUSA-SKU-MDC)


### Link to data sheet
[DataSheet used for my Projects](https://github.com/michelheil/Arduino/lib/myAMG8833/datasheet)

### Pictures of sensor
<img src="https://github.com/michelheil/Arduino/blob/master/lib/myAMG8833/pictures/AMG8833_Front.jpg" width="250"> <img src="https://github.com/michelheil/Arduino/blob/master/lib/myAMG8833/pictures/AMG8833_Back.jpg" width="250">

### Other Links and References
[Adafruit Shop AMG8833](https://www.adafruit.com/product/3538)

[Panasonic Infrared Array SensorGrid-EYE](http://industrial.panasonic.com/cdbs/www-data/pdf/ADI8000/ADI8000C66.pdf)


## Library Documentation

### Dependencies
* <util/twi.h>
* <util/delay.h>
* <string.h>
* "myGlobalDefines.h"
* "myTWI.h"

### Init function
Entering normal mode for AMG8833, restarting and setting general registers

```c
int AMG8833_init(uint8_t pcr, uint8_t rr, uint8_t frr, uint8_t icr)
{
    // initialize I2C for communication with AMG8833
    TWI_init();

#ifdef MYLOG_H_
    LOG_debug("Set Operating Mode:");
#endif
    AMG8833_setRegisterByte(AMG8833_PCTL, pcr);

#ifdef MYLOG_H_
    LOG_debug("Perform Software Reset:");
#endif    
    AMG8833_setRegisterByte(AMG8833_RST, rr);
    
#ifdef MYLOG_H_    
    LOG_debug("Set Frame Rate:");
#endif    
    AMG8833_setRegisterByte(AMG8833_FPSC, frr);

#ifdef MYLOG_H_
    LOG_debug("Disable Interrupts:");
#endif    
    AMG8833_setRegisterByte(AMG8833_INTC, icr);

    _delay_ms(100);

    return 0;
}
```


### APIs
Switch moving average on or off

```void AMG8833_setMovingAverage(uint8_t flag);```

Activate Grid Interrupt and set Upper Limit

```void AMG8833_setInterruptUpperLimit(uint8_t msbLimit, uint8_t lsbLimit);```

Read the Temperature Register of all 8x8 Pixels from AMG8833 through TWI (I2C)

```void AMG8833_readGrid(float * resultGridValues);```

Read the Thermistor Register from AMG8833 through TWI (I2C)

```float AMG8833_readThermistor(void);```

Read the Status Register from AMG8833 through TWI (I2C)

```const char * AMG8833_readStatusRegister(void);```

#### Helper Functions
Set a register of the AMG8833 device through TWI (I2C)

```void AMG8833_setRegisterByte(uint8_t reg, uint8_t val)```

Read register of the AMG8833 device through TWI (I2C)

```uint8_t AMG8833_getRegisterByte(uint8_t reg)```


## Examples
To get started using the myAMG8833 library I have created examples.

### Basic
The [basic](https://github.com/michelheil/Arduino/tree/master/lib/myAMG8833/example/basic) example has
minimal dependencies and shows how to initialise and read both Thermistor and Grid Values.

#### Picture of wiring (Basic)
Fritzing

#### Dependencies (Basic)
- <avr/io.h>
- <util/delay.h>
- "myGlobalDefines.h"
- "myTWI.h"
- "myUSART.h"

### Interrupt
The [interrupt](https://github.com/michelheil/Arduino/tree/master/lib/myAMG8833/example/interrupt) example makes
use of the interrupt capabilities of the AMG8833 device. An Interrupt is actived when one of the grid values
exceeds a given upper limit.

#### Picture of wiring (Interrupt)
Fritzing

#### Dependencies (Interrupt)
- <avr/io.h>
- <avr/interrupt.h>
- <stdlib.h>
- <stdio.h>
- <string.h>
- <util/delay.h>
- "myGlobalDefines.h"
- "myTWI.h"
- "myUSART.h"


