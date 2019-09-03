/*
 * myADC.c
 *
 * Created: 03.09.2019 22:53:01
 *  Author: Michael
 */ 
#include <avr/io.h>

void ADC_init(void);
uint16_t ADC_readAnalogPin(uint8_t channel);
void ADC_startConversionAndWait();

// initialize the ADC
void ADC_init(void)
{
	// REFS0 defines the reference voltage of 5V
	// MUX3..0 within port ADMUX defines the input pin.
	ADMUX = (1 << REFS0);
	
	// ADEN set to 1 enables the ADC converter
	// ADIE set to 1 enables the ADC interrupt
	// ADPS2..0: These bits determine the division factor between the system clock frequency and the input clock to the ADC. All set to 1 leads to division factor of 128
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	// turn off the digital input register for analog pin 0.
	// this prevents from accidentally reading and processing the digital input signal from the analog pin.
	DIDR0 = (1 << ADC0D);
	
	// after activation of the ADC a "Dummy-Readout" is recommended
	// ADC is initially being read, otherwise the result of the upcoming conversion will not be taken
	ADC_startConversionAndWait();
	(void) ADC;
}

/* ADC measurement for a single channel */
uint16_t ADC_readAnalogPin(uint8_t channel)
{
	// select pin without changing other bits within ADMUX.
	// pin selection for ATMega328P is only the bits 3..0
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
	
	ADC_startConversionAndWait();
	
	// read ADC and return value
	return ADC;
}

// Start ADC conversion and wait until the conversion is completed
void ADC_startConversionAndWait()
{
	// Start Conversion
	ADCSRA |= (1 << ADSC);
	
	// wait until conversion is completed
	while (ADCSRA & (1<<ADSC) ) {}
	
}
