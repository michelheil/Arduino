/*
 * ArduinoStarterKitProject6.c
 *
 * Created: 11.09.2019 21:13:15
 * Author : Michael
 */ 

#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/interrupt.h>
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


/* START CALIBRATION */
/* Input: channel = 0, how many time = 10 */
	DDRB |= (1 << DDB5);

	// set PB5 to 1
	PORTB |= (1 << PORTB5);
	
	// calibrate the input signal
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
	
	// switch on-board LED off to signal the end of calibration
	PORTB &= ~(1 << PORTB5);
/* END CALIBRATION */	


/*
cli(); // disable global interrupt setting
TCCR1A = 0;
TCCR1B = 0;

OCR1A = 2000;

TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

TIMSK1 = (1 << OCIE1A);

sei(); // enable global interrupt setting SREG = (1 << I);
*/


	
    /* Replace with your application code */
    while (1) 
    {
		sensorValue = ADC_readAnalogPin(0);
		USART_writeString("SensorValue: ");
		USART_writeString(uint162char(sensorValue));
		
		uint16_t mappedSensorValue = mapSensorValueToFullRange(sensorValue, sensorLowerBound, sensorUpperBound, 0, 1023);
		USART_writeString(", MappedSensorValue: ");
		USART_writeString(uint162char(mappedSensorValue));
		USART_writeString("\r\n");
		
		// define tone(mappedValue)
		
		if (sensorValue <= 300) {
			// set PB4
			PORTB |= (1 << PORTB4);
			_delay_ms(10);
			// clear PB4
			PORTB &= ~(1 << PORTB4);
			_delay_ms(10);
		} else if (sensorValue > 300 && sensorValue <= 600) {
			// set PB4
			PORTB |= (1 << PORTB4);
			_delay_ms(100);
			// clear PB4
			PORTB &= ~(1 << PORTB4);
			_delay_ms(100);
		} else {
			// set PB4
			PORTB |= (1 << PORTB4);
			_delay_ms(500);
			// clear PB4
			PORTB &= ~(1 << PORTB4);
			_delay_ms(500);			
		}
	

		_delay_ms(10);
		
    }
}

// this function scales a value within a range to an expected range of uint16_t values
uint16_t mapSensorValueToFullRange(uint16_t sValue, uint16_t detectedMinValue, uint16_t detectedMaxValue, uint16_t minFullRangeValue, uint16_t maxFullRangeValue) {
	
	// scale detected range to expected range
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

/*
ISR(TIMER0_COMPA_vect) {
	PORTB ^= (1 << PORTB4);
}
*/
