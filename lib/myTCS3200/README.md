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

Source: [DataSheet](https://github.com/michelheil/Arduino/lib/myTCS3200/datasheet)


### Link to data sheet
[DataSheet used for my Projects](https://github.com/michelheil/Arduino/lib/myTCS3200/datasheet)

### Pictures of sensor
<img src="https://github.com/michelheil/Arduino/blob/master/lib/myTCS3200/pictures/TCS3200_Front.jpg" width="250"> <img src="https://github.com/michelheil/Arduino/blob/master/lib/myTCS3200/pictures/TCS3200_Back.jpg" width="250">

### Other Links and References
[Adafruit Shop DS3231](https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/overview)


## Library Documentation

### Dependencies
* "myGlobalDefines.h"
* <avr/io.h>
* <util/delay.h>

### Init function
```/**
 * @brief Construct a new TCS3200::TCS3200 object
 * @details Data Direction Registers and Output Frequency Scaling are set
 * Output Frequency Sclaing (f_0)
 * S0  S1  Output Frequency Scaling (f_0)
 * L   L   Power down
 * L   H   2%
 * H   L   20%
 * H   H   100% // Arduino not able to process
 * 
 * @param counter used to measure the color frequencies
 * @param s0p pin S0
 * @param s1p pin S1
 * @param s2p pin S2
 * @param s3p pin S3
 * @param outp pin OUT
 */
TCS3200::TCS3200(volatile int * counter, uint8_t s0p, uint8_t s1p, uint8_t s2p, uint8_t s3p, uint8_t outp):
   interruptCounter(counter), s0Pin(s0p), s1Pin(s1p), s2Pin(s2p), s3Pin(s3p), outPin(outp)
{
  // set data direction registers
  sbi(DDRD, s0Pin);
  sbi(DDRD, s1Pin);
  sbi(DDRD, s2Pin);
  sbi(DDRD, s3Pin);
  cbi(DDRD, outPin);

  // output frequency scaling
  sbi(PORTD, s0Pin);
  cbi(PORTD, s1Pin);
}```

### APIs
Measure and store reference colors

```void TCS3200::calibrate(void);```

Measure calibrated color value

```int TCS3200::measureColor(int color);```

Dependent on pins S2 and S3, select color photodiodes for each color
 @details 
  S2  S3  Photodiode Type
  L   L   Red
  L   H   Blue
  H   L   Clear (no filter)
  H   H   Green
 
 @param s2Level status for pin S2
 @param s3Level status for pin S3

```void TCS3200::colorSelection(int s2Level, int s3Level);``` 


#### Helper Functions
Measures the OUT activities of a color

```int TCS3200::measureTicks(int color);```


## Example
* deactivate global Interrupts during initialization phase
* initialize USART
* construct TCS320 object
* Enable INT0 (PD2) interrupts
* The rising edge of INT0 (PD2) generates an interrupt request.
* activate global interrupt flag
* Start calibration on white background..."

### Picture of wiring
Fritzing

### Dependencies
* "myGlobalDefines.h"
* <avr/interrupt.h>
* "myUSART.h"
* "myTCS3200.h"

