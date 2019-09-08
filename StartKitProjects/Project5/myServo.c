/*
 * myServo.c
 *
 * Created: 08.09.2019 07:58:03
 *  Author: Michael
 */ 

#include <avr/io.h>

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
	
	// set timer/counter to 0
	TCNT1 = 0;
	
	// set pins OC1A (= PB1 = ~9) and OC1B (= PB2 = ~10) to Output. DDRB = Data Direction Register port B
	DDRB = (1 << DDB1) | (1 << DDB2);
}


// transform angle into a pulse width and set 16-bit output compare register
uint16_t Servo_setAngle(uint32_t angle)
{
	uint16_t pulse = PULSE_0DEG + (angle * ONE_DEGREE);
	OCR1A = pulse;
	
	return pulse;
}


// map 10-bit input to angle
uint32_t Servo_map10BitToAngle(uint16_t analogRead)
{
	 return ((uint32_t) analogRead * 180) / 1024;
}

// set Servo position based on a 10-bit value, e.g. an analog input read
void Servo_set10Bit(uint16_t analogRead)
{
	Servo_setAngle(Servo_map10BitToAngle(analogRead));
}

// write functions to set servo position given a pulse width in percentage, an angle or ...?
// for setAngle: ensure that values below 0 get assigned to 0 and values greater then 180 to 180.
// 
/* debug Mode: dann werden alle ZwischenWerte nach USART geschrieben.
#include "myUSART.h"
USART_init();

					USART_writeString("Poti: ");
					USART_writeString(uint162char(potiValue));
			
			
				USART_writeString(", Angle: ");
				USART_writeString(uint162char(angle));
		
		
		USART_writeString(", Pulse: ");
		USART_writeString(uint162char(pulse));
		
		USART_writeString("\r\n");

*/


// Wenn man ein Knopf drueckt dann wird das Poti bzw. der Input Kanal kalibriert, d.h. die Spanne von Maximum und Minimum wird ausgelesen und entsprechend koennen dann die uebrigen Werte berechnet werden.