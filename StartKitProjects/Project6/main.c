/*
 * ArduinoStarterKitProject6.c
 *
 * Created: 11.09.2019 21:13:15
 * Author : Michael
 */ 

#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/interrupt.h> // includes cli(), sei(), ISR(*_vector)
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "myADC.h"
#include "myUSART.h"


uint16_t mapSensorValueToFullRange(uint16_t sValue, uint16_t minDetectedValue, uint16_t maxDetectedValue, uint16_t minFullRangeValue, uint16_t maxFullRangeValue);


int main(void)
{
	
	// start with initial boundaries that will be adjusted during calibration
	uint16_t sensorValue;
	uint16_t sensorLowerBound = 1023;
	uint16_t sensorUpperBound = 0;

	ADC_init();
	USART_init();
	USART_writeString("\r\n");
	
	// set PB4 as output pin
	DDRB = (1 << DDB4);

	/* START INTERRUPT DISABLED CODE */
	// disable global interrupt setting
	cli();
	
	/* START INTERRUPT INIT */
	// ToDo: write Interrupt_init()
	// Timer/Counter Control Register 1A/1B
	TCCR1A = 0; // normal port operation, OCA1/OCB1 disconnected

	// The Output Compare Registers (OCR1A) contain a 16-bit value that is continuously compared
	// with the counter value (TCNT1). A match can be used to generate an Output Compare interrupt.
	OCR1A = 15624; // max value = 65535 (1 second = (15624 + 1) = 16MHz / 1024

	// Setting only WGM12 on TCCR1B activates the CTC (Clear Timer on Compare Match) mode
	// Bits on CS12 and CS10 set the pre scale factor to 1024
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

	// Timer/Counter Interrupt Mask Register
	// OCIE1A Timer/Counter1, Output Compare A Match Interrupt Enable
	TIMSK1 = (1 << OCIE1A);
	/* END INTERRUPT INIT */


	/* START CALIBRATION */
	// ToDo: write calibration function
	/* Input: channel = 0, how many time = 10 */
	DDRB |= (1 << DDB5);

	// set PB5 to 1 to indicate the start of calibration
	PORTB |= (1 << PORTB5);
	
	// calibrate the input signal for the first 5 seconds after code upload
	for(int i = 0; i < 10; i++) {
		
		sensorValue = ADC_readAnalogPin(0);
		USART_writeString("SensorValue: ");
		USART_writeString(uint162char(sensorValue));
		
		if(sensorValue > sensorUpperBound) {
			sensorUpperBound = sensorValue;
		}
		if(sensorValue < sensorLowerBound) {
			sensorLowerBound = sensorValue;
		}
		
		USART_writeString(", Upper Bound: ");
		USART_writeString(uint162char(sensorUpperBound));
		USART_writeString(", Lower Bound: ");
		USART_writeString(uint162char(sensorLowerBound));
		USART_writeString("\r\n");
		
		_delay_ms(500);
		
	}

	// switch on-board LED off to indicate the end of calibration
	PORTB &= ~(1 << PORTB5);
	/* END CALIBRATION */	

	sei();
	/* END INTERRUPT DISABLED CODE */


    while (1) 
    {
		// read sensorValue and map it to full range of possible compare match values (as we use 16-bit, max value is 65535)
		// this mapped value is then set as the Compare Value for Timer 1A (OCR1A)
		sensorValue = ADC_readAnalogPin(0);
		uint16_t mappedSensorValue = mapSensorValueToFullRange(sensorValue, sensorLowerBound, sensorUpperBound, 0, 65535);
		OCR1A = mappedSensorValue;
		
		// give the sound time to establish
		_delay_ms(10);

    }
}

// this function scales a value within a range to an expected range of uint16_t values
uint16_t mapSensorValueToFullRange(uint16_t sValue, uint16_t detectedMinValue, uint16_t detectedMaxValue, uint16_t minFullRangeValue, uint16_t maxFullRangeValue) {
	
	// scale detected range to expected range and round to an 16-bit integer
	uint16_t mappedValue = (double) (maxFullRangeValue - minFullRangeValue) / (detectedMaxValue - detectedMinValue) * (sValue - detectedMinValue);
	
	// if measured sensor value exceeds detected min-max range during runtime, set the values to the given full range boundaries
	if(sValue < detectedMinValue) {
		 return minFullRangeValue;
	}
	else if (sValue > detectedMaxValue) {
		return maxFullRangeValue;
	}
	else {
		return mappedValue;
	}
}

// Interrupt Service Routine
ISR(TIMER1_COMPA_vect) {
	
	// switch pin PB4 on and off in the frequency of the set OCR1A
	PORTB ^= (1 << PORTB4);
}