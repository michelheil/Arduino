/*
 * myADC.c
 *
 * Created: 03.09.2019 22:53:01
 *  Author: Michael
 */ 
#include <avr/io.h>
#define sbi(PORT, bit) (PORT |= (1 << bit))  // set bit in PORT
#define cbi(PORT, bit) (PORT &= ~(1 << bit)) // clear bit in PORT
#define tgl(PORT, bit) (PORT ^= (1 << bit))  // set bit in PORT

void ADC_init(void);
void ADC_startConversionAndWait();
uint16_t ADC_readAnalogPin(uint8_t channel);
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
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
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

// turn off the digital input register for analog pin
void ADC_disableDigitalInput(uint8_t channel)
{
	// Digital Input Disable Register
	// this prevents from accidentally reading and processing the digital input signal from the analog pin.
	sbi(DIDR0, channel); // for Pin 0: DIDR0 = (1 << ADC0D);
}