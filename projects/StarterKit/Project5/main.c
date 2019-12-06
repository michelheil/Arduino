/*
 * Arduino StarterKitProject5.c
 *
 * Created: 06.09.2019 19:48:42
 * Author : Michael
 */ 


// set frequency [Hz]
#define F_CPU 16000000L

#include <avr/io.h>

#include "myADC.h"
#include "myServo.h"

int main(void)
{
	ADC_init();
	PWM16_init();

	/* Replace with your application code */
	while (1)
	{
		// read analog signal from poti sensor
		uint16_t potiValue = ADC_readAnalogPin(0); // result should have values between 0 and 1023 (i.e. 10 bits)

		// use input signal to set Servo position
		Servo_set10Bit(potiValue);
	}
	
	return 0;
}


