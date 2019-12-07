## Short Description
The ATmega328P features a 10-bit successive approximation ADC. The ADC is connected to an 8-channel Analog 
Multiplexer which allows eight single-ended voltage inputs constructed from the pins of Port A. The single-ended
voltage inputs refer to 0V (GND).

### Reference to Data Sheet ATMega328P
[Download ATMega328P data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)

Chapter 24. Analog-to-Digital Converter

Features
- 10-bit Resolution
- 0.5 LSB Integral Non-linearity
- ± 2 LSB Absolute Accuracy
- 13 - 260μs Conversion Time
- Up to 76.9kSPS (Up to 15kSPS at Maximum Resolution)
- 6 Multiplexed Single Ended Input Channels
- 2 Additional Multiplexed Single Ended Input Channels (TQFP and QFN/MLF Package only)
- Temperature Sensor Input Channel
- Optional Left Adjustment for ADC Result Readout
- 0 - V CC ADC Input Voltage Range
- Selectable 1.1V ADC Reference Voltage
- Free Running or Single Conversion Mode
- Interrupt on ADC Conversion Complete
- Sleep Mode Noise Canceler

## Library Documentation

### Dependencies
* <avr/io.h>
* <util/delay.h>
* "myGlobalDefines.h"

### Init function
Initialize the Analog-to-Digital Converter (10-bit resolution)

```c
void ADC_init(void)
{
 // REFS0 defines the reference voltage of 5V
 // MUX3..0 within port ADMUX defines the input pin. The pin will be set using function ADC_readAnalogPin.
 ADMUX = (1 << REFS0);
	
 // ADEN set to 1 enables the ADC converter
 // ADIE set to 1 enables the ADC interrupt
 // ADPS2..0: These bits determine the division factor between the system clock frequency and the input clock to the ADC. All set to 1 leads to division factor of 128
 ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
 // after activation of the ADC a "Dummy-Readout" is recommended
 // ADC is initially being read, otherwise the result of the upcoming conversion will not be taken
 ADC_startConversionAndWait();
 (void) ADC;
}
```

### APIs
ADC measurement for a single channel

```uint16_t ADC_readAnalogPin(uint8_t channel);```

Prevents from accidentally reading and processing the digital input signal from the analog pin

```void ADC_disableDigitalInput(uint8_t channel);```

Measure the values of a flaky actor for calibration

```struct pairOfTwoUint16 ADC_calibrateAnalogPin(uint8_t channel, int calibrations);```

Scales a value within a range to an expected range of uint16_t values

```uint16_t mapSensorValueToFullRange(uint16_t sValue, uint16_t detectedMinValue, uint16_t detectedMaxValue, uint16_t minFullRangeValue, uint16_t maxFullRangeValue);```


### Helper Functions
Start ADC conversion and wait until the conversion is completed

```void ADC_startConversionAndWait()```
