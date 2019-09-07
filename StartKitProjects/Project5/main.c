/*
 * Arduino StarterKitProject5.c
 *
 * Created: 06.09.2019 19:48:42
 * Author : Michael
 */ 


// set frequency [Hz]
#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>

#include "myUSART.h"
#include "myADC.h"

/* RC (Radio Control) Servo Basics (in this case SM-S2309S):
The base frequency for an RC servo is (usually) 50Hz, i.e. 50 signals per seconds, i.e. 20 ms for one signal.
The default servo pulse width is between 1ms (0 degree) and 2ms (180 degree). Usually rather between 0.75ms and 2.25ms (=> try out!).

According to data sheet the PWM frequency f_OCnxPWM = f_clk_I/O / (N * (1 + TOP))
In my set up:
f_clk_I/O = F_CPU = 16MHz
PWM frequency should match servo requirements of 50Hz => f_OCnxPWM = 50 * 1 / sec
We will choose the PWM pre-scaler N as N = 8
=> TOP has to be set to 39999 (TOP_COUNTER) which means the timer counts from 0 to 39999

Now, trying out the 0 and 180 degree values. Results:

0 degree will be reached when PULSE_0DEG = 559 => pulse width = 559/40000 * 20ms = 0.2795ms
180 degree will be reached when PULSE_180DEG = 2300 => pulse width = 2300/40000 * 20ms = 1.15ms
90 degree will be reached when PULSE_90DEG = (PULSE_180DEG + PULSE_0DEG) / 2
A change of one degree is calculated as (PULSE_180DEG - PULSE_0DEG) / 180

These calculations require a Phase Correct Waveform Generation (see Port TCCR1A and TCCR1B).
*/
#define TOP_COUNTER 39999
#define PULSE_0DEG 559
#define PULSE_180DEG 2300
#define PULSE_90DEG (PULSE_180DEG + PULSE_0DEG) / 2
#define ONE_DEGREE (PULSE_180DEG - PULSE_0DEG) / 180

void PWM16_init();

int main(void)
{
    uint32_t angle; // as maximum value can get to 1024*280 = 184320 = 0b0010 1101 0000 0000 0000
	uint16_t pulse; // steers the servo by setting the 16-bit Output Compare Register

	USART_init();
	ADC_init();
	PWM16_init();
		
	// switch off digital input for analog pin 4
	ADC_disableDigitalInput(0);

	/* Replace with your application code */
	while (1)
	{
		// read analog signal from poti sensor
		uint16_t potiValue = ADC_readAnalogPin(0); // result should have values between 0 and 1023 (i.e. 10 bits)
		USART_writeString("Poti: ");
		USART_writeString(uint162char(potiValue));
		
		/* Output Compare Register 1A is a 16-bit register (taking values between 0 and 65535) storing the compare value */
        // map to 0-180 degree range
        angle = ((uint32_t) potiValue * 180) / 1024;
		USART_writeString(", Angle: ");
		USART_writeString(uint162char(angle));
		
		// transform angle into pulse
        pulse = PULSE_0DEG + (angle * ONE_DEGREE);
		USART_writeString(", Pulse: ");
		USART_writeString(uint162char(pulse));
		
		// set OCR1A
		OCR1A = pulse;

		USART_writeString("\r\n");
		//_delay_ms(20);
	}
	
	return 0;
}

//TODO: initialize with a given value to set the servo position
// write servo functionality into another file
// write functions to set servo position given a pulse width in percentage, an angle or ...?

/* Non-inverting 16-bit PWM Mode initialization (= Timer/Counter1) */
void PWM16_init(){

	// set non-inverting fast PWM mode for both pins
	// COM = Compare Output Mode. Only setting COMnx1 to 1 equals the non-inverting mode
	TCCR1A = (1 << COM1A1) | (1 << COM1B1);
	
	// WGM = Waveform Generation Mode
    // Phase Correct and ICR1 as TOP
	TCCR1A |= (1 << WGM11);
	TCCR1B = (1 << WGM13);
	
	// set scaler (PWM frequency) to 8
	TCCR1B |= (1 << CS11);
	
	// set cycle length (ICR1 = TOP in chosen Waveform Generation Mode)
	ICR1 = TOP_COUNTER;
	
	// set servo to middle position (90 degrees)
	OCR1A = PULSE_0DEG;
	
	// set timer/counter to 0
	TCNT1 = 0;
	
    // set pins OC1A (= PB1 = ~9) and OC1B (= PB2 = ~10) to Output. DDRB = Data Direction Register port B
	DDRB = (1 << DDB1) | (1 << DDB2);
}
