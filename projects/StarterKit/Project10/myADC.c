/*
 * myADC.c
 *
 * Created: 03.09.2019 22:53:01
 *  Author: Michael
 */ 

#include "myADC.h"

void ADC_init(void);
void ADC_startConversionAndWait();
uint16_t ADC_readAnalogPin(uint8_t channel);
struct pairOfTwoUint16 ADC_calibrateAnalogPin(uint8_t channel, int calibrations);
void ADC_disableDigitalInput(uint8_t channel);

// initialize the Analog-to-Digital Converter (10-bit resolution)
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

// Start ADC conversion and wait until the conversion is completed
void ADC_startConversionAndWait()
{
	// Start Conversion
	ADCSRA |= (1 << ADSC);
	
	// ADSC will read as one as long as a conversion is in progress. When the conversion is complete, it returns to zero.
	while (ADCSRA & (1 << ADSC)) {}
	
}


// ADC measurement for a single channel
uint16_t ADC_readAnalogPin(uint8_t channel)
{
	// select pin without changing other bits within ADMUX.
	// pin selection for ATMega328P is only the bits 3..0
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
	
	ADC_startConversionAndWait();
	
	// read ADC and return value
	return ADC;
}


// 
struct pairOfTwoUint16 ADC_calibrateAnalogPin(uint8_t channel, int calibrations) {

	uint16_t readValue;
	struct pairOfTwoUint16 resultDetectedValues = {1023, 0};

	// calibrate the input signal for the first few seconds (dependent on the input parameter calibrations) after code upload
	for (int i = 0; i < calibrations; i++) {
		
		readValue = ADC_readAnalogPin(channel);
		
		if(readValue > resultDetectedValues.sensorUpperBound) {
			resultDetectedValues.sensorUpperBound = readValue;
		}
		if(readValue < resultDetectedValues.sensorLowerBound) {
			resultDetectedValues.sensorLowerBound = readValue;
		}

		// check new values every half second
		_delay_ms(500);
		
	}

	return resultDetectedValues;
}


// turn off the digital input register for analog pin
void ADC_disableDigitalInput(uint8_t channel)
{
	// Digital Input Disable Register
	// this prevents from accidentally reading and processing the digital input signal from the analog pin.
	sbi(DIDR0, channel); // for Pin 0: DIDR0 = (1 << ADC0D);
}


/* helper functions */

// this function scales a value within a range to an expected range of uint16_t values
uint16_t mapSensorValueToFullRange(uint16_t sValue, uint16_t detectedMinValue, uint16_t detectedMaxValue, uint16_t minFullRangeValue, uint16_t maxFullRangeValue)
{
	
	// scale detected range to expected range and round to an 16-bit integer
	if (detectedMaxValue == detectedMinValue) detectedMaxValue = detectedMinValue + 1; // ensure that no division by 0 is happening
	uint16_t mappedValue = (double) (maxFullRangeValue - minFullRangeValue) / (detectedMaxValue - detectedMinValue) * sValue + (minFullRangeValue - detectedMinValue);
	
	// if measured sensor value exceeds detected min-max range during runtime, set the values to the given full range boundaries
	// or if rounding errors lead to a mapped value below or above the fullRangeValues
	if ((sValue < detectedMinValue) | (mappedValue < minFullRangeValue)) {
		return minFullRangeValue;
	}
	else if ((sValue > detectedMaxValue) | (mappedValue > maxFullRangeValue)) {
		return maxFullRangeValue;
	}
	else {
		return mappedValue;
	}
}