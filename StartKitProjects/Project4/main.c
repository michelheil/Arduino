/*
 * ArduinoStarterKitProject4.c
 *
 * Created: 05.09.2019 19:47:12
 * Author : Michael
 */ 

// set frequency [Hz]
#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>

#include "myUSART.h"
#include "myADC.h"


int main(void)
{
	USART_init();
	ADC_init();
	
	// set pin ~11 (PB3) to output and to 0. DDRB = Data Direction Register port B
	DDRB = (1 << DDB3);
	
	// switch off digital input for analog pin 0
	ADC_disableDigitalInput(0);

	/* Non-inverting PWM Mode initialization */	
	// set non-inverting fast PWM mode
	TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20);
	// set scaler to 1024
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
	
    /* Replace with your application code */
    while (1) 
    {
		// read analog signal from temperature sensor (tmp36 - https://www.analog.com/media/en/technical-documentation/data-sheets/TMP35_36_37.pdf
		uint16_t tempSensorValue = ADC_readAnalogPin(0); // result should have values between 0 and 1023 (i.e. 10 bits)
		
		USART_writeString("Sensor Value: ");
		USART_writeString(uint162char(tempSensorValue));
		
		// Output Compare Register 2A is a 8-bit register (taking values between 0 and 255) storing the compare value
		OCR2A = tempSensorValue >> 2;
		
		USART_writeString("\r\n");
		_delay_ms(1500);
    }
}

