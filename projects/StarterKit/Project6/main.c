/*
 * ArduinoStarterKitProject6.c
 *
 * Created: 11.09.2019 21:13:15
 * Author : Michael
 */ 

#include "globalDefines.h"

#include <avr/io.h>
#include <avr/interrupt.h> // includes cli(), sei(), ISR(*_vector)
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "myADC.h"
#include "myUSART.h"

// constants/parameters/function for this particular project
#define ANALOG_INPUT_CHANNEL 0
uint16_t mapSensorValueToFullRange(uint16_t sValue, uint16_t minDetectedValue, uint16_t maxDetectedValue, uint16_t minFullRangeValue, uint16_t maxFullRangeValue);
void Timer1A_interrupt_init();


// calibrate input and steer Piezo
int main(void)
{
	uint16_t sensorValue;
	
	ADC_init();
	USART_init();
	
	/* START INTERRUPT DISABLED CODE */
	cli(); // disable global interrupt setting
	
	// set PB5 to 1 to indicate the start of calibration
	DDRB |= (1 << DDB5);
	PORTB |= (1 << PORTB5);
	
	// calibrate input values to cover a given range
	struct pairOfTwoUint16 detectedSensorValues = ADC_calibrateAnalogPin(ANALOG_INPUT_CHANNEL, 10);
	
	// switch on-board LED off to indicate the end of calibration
	PORTB &= ~(1 << PORTB5);
		
	// set bits in Timer 1A registers
	Timer1A_interrupt_init();

	sei(); // enable global interrupt setting
	/* END INTERRUPT DISABLED CODE */
	
	// set PB4 as output pin
	DDRB |= (1 << DDB4);

    while (1) 
    {
		// read sensorValue and map it to range of possible compare match values (as we use 16-bit, max value is 65535)
		// Minimum value is 1
		// Maximum Value is 500 ms = 15624 / 2 = 7812
		// this mapped value is then set as the Compare Value for Timer 1A (OCR1A)
		sensorValue = ADC_readAnalogPin(ANALOG_INPUT_CHANNEL);
		uint16_t mappedSensorValue = mapSensorValueToFullRange(sensorValue, detectedSensorValues.sensorLowerBound, detectedSensorValues.sensorUpperBound, 100, 500);
		OCR1A = mappedSensorValue;
		
		// when the sensorValue decreases it could be the case that the counter already passed the old compare match value
		// in that case the counter will continue to count until TOP (= max value = 65535) which means nothing happens for a few seconds
		// therefore, the counter is set to 0 when it already exceeded the compare value
		if(TCNT1 > OCR1A) TCNT1 = 0;
		
		USART_writeString("Sensor Value: ");
		USART_writeString(uint162char(sensorValue));
		USART_writeString(", Mapped Sensor Value: ");
		USART_writeString(uint162char(mappedSensorValue));
		USART_writeString("\r\n");
		
		// give the sound time to establish
		_delay_ms(100);

    }
}

// this function scales a value within a range to an expected range of uint16_t values
uint16_t mapSensorValueToFullRange(uint16_t sValue, uint16_t detectedMinValue, uint16_t detectedMaxValue, uint16_t minFullRangeValue, uint16_t maxFullRangeValue) {
	
	// scale detected range to expected range and round to an 16-bit integer
	if (detectedMaxValue == detectedMinValue) detectedMaxValue = detectedMinValue + 1; // ensure that no division by 0 is happening
	uint16_t mappedValue = (double) (maxFullRangeValue - minFullRangeValue) / (detectedMaxValue - detectedMinValue) * sValue + (minFullRangeValue - detectedMinValue);
	
	// if measured sensor value exceeds detected min-max range during runtime, set the values to the given full range boundaries
	// or if rounding errors lead to a mapped value below or above the fullRangeValues
	if((sValue < detectedMinValue) | (mappedValue < detectedMinValue)) {
		 return minFullRangeValue;
	}
	else if ((sValue > detectedMaxValue) | (mappedValue > detectedMaxValue)) {
		return maxFullRangeValue;
	}
	else {
		return mappedValue;
	}
}

// initialize Timer 1A with interrupt and a Clear Timer on Compare Match and a pre-scaler of 1024
void Timer1A_interrupt_init() {
	// ToDo:
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
}

// Interrupt Service Routine
ISR(TIMER1_COMPA_vect) {
	
	// switch pin PB4 on and off in the frequency of the set OCR1A
	PORTB ^= (1 << PORTB4);
}