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

These calculations require a Phase Correct Waveform Generation (see ports TCCR1A and TCCR1B).
*/

/* PWM constants */
#define PWM_PERIOD_IN_MS 20 // => PWM frequency = 50Hz (depends on the selection of pre-scaler, in this case the pre-scaler is set to 8)
#define TOP_COUNTER 39999

/* Servo constants */
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
	
	// set pre-scaler to 8
	TCCR1B |= (1 << CS11);
	
	// set cycle length (ICR1 = TOP in chosen Waveform Generation Mode)
	ICR1 = TOP_COUNTER;
	
	// set timer/counter to 0
	TCNT1 = 0;
	
	// set pins OC1A (= PB1 = ~9) and OC1B (= PB2 = ~10) to Output. DDRB = Data Direction Register port B
	DDRB = (1 << DDB1) | (1 << DDB2);
}


// transform angle into a pulse width and set 16-bit output compare register
void Servo_setAngle(uint32_t angle)
{
	uint16_t dutyCycleInTicks;
		
	if (angle < 0) 
		dutyCycleInTicks = PULSE_0DEG;
	else if (angle > 180)
		dutyCycleInTicks = PULSE_180DEG;
	else
		dutyCycleInTicks = PULSE_0DEG + (angle * ONE_DEGREE);

	OCR1A = dutyCycleInTicks;		
}


// set servo position based on a 10-bit value, e.g. an analog input read
void Servo_set10Bit(uint16_t analogRead)
{
	Servo_setAngle(Servo_map10BitToAngle(analogRead));
}


// set servo position based on the duty cycle in milliseconds
void Servo_setMs(float dutyCycleInMs)
{
	uint16_t dutyCycleInTicks;
	
	if (dutyCycleInMs < 0)
		dutyCycleInTicks = 0;
	else if (dutyCycleInMs > PWM_PERIOD_IN_MS)
		dutyCycleInTicks = TOP_COUNTER;
	else
		dutyCycleInTicks = dutyCycleInMs * (1 + TOP_COUNTER) / PWM_PERIOD_IN_MS; // for Phase Correct PWM 
	
	// set 16-bit value Output Compare Register
	OCR1A = dutyCycleInTicks;

}


// map 10-bit input to angle
uint32_t Servo_map10BitToAngle(uint16_t analogRead)
{
	return ((uint32_t) analogRead * 180) / 1024;
}
