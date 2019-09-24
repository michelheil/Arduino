/*
 * ArduinoStarterKitProject10.c
 *
 * PCINT1 (PB1 / 9)  Pin Change Interrupt Request 1 used to switch on/off motor
 * PCINT2 (PB2 / 10) Pin Change Interrupt Request 2 used to switch motor direction
 *
 *
 * Created: 23.09.2019 18:54:08
 * Author : Michael
 */ 

#include "globalDefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

#include "myUSART.h"
#include "myADC.h"
#include "myVariableTypes.h"

// global Variable (volatile) that can be changed by interrupt routines and used throughout the program.
volatile uint8_t motorSwitch = 0b00000000; // switched by PB1
volatile uint8_t motorDirection = 0b00000000; // switched by PB2

volatile uint8_t historyOfPortB = 0x00;



int main(void)
{
	// initialize USART functionality
	USART_init();
	USART_writeString("Motor Switch: ");
	USART_writeString(uint82char(motorSwitch));
	USART_writeString(", Motor Direction: ");
	USART_writeString(uint82char(motorDirection));
	USART_writeString("\r\n");
	
	// clear global interrupt flag to allow for interrupted calibration of the input analog Pin without any interrupts
	cli();
	
	// initialize ADC and calibrate poti for 5 seconds
	ADC_init();
	struct pairOfTwoUint16 detectedMinMaxPotiValue = ADC_calibrateAnalogPin(0, 10);
	
	// set PD2 and PD3 as output Pins and start with 0
	// Those two pins will steer the motor direction
	DDRD |= (1 << DDD3) | (1 << DDD2);
	PORTD &= ~((1 << PORTD3) | (1 << PORTD2));
	
	// set PD5 as output pin and start with 0
	// this pin will steer the speed of the motor through the high-current half-H drivers (H-Bruecke)
	DDRD |= (1 << DDD5);
	PORTD &= ~(1 << PORTD5);

	// set PB1 as input PIN (pull-up resistor in PORTB not required as we use pull-down register on bread board)
	DDRB &= ~((1 << DDB5) | (1 << DDB4));
	
	// Pin Change Interrupt Control Register
	// When the PCIE0 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), pin change interrupt 0 is enabled. 
	// Any change on any enabled PCINT[7:0] pin will cause an interrupt. The corresponding interrupt of Pin Change Interrupt Request
	// is executed from the PCI0 Interrupt Vector. PCINT[7:0] pins are enabled individually by the PCMSK0 Register.
	PCICR |= (1 << PCIE0);
	
	// Pin Change Mask Register 0
	// Each PCINT[7:0] bit selects whether pin change interrupt is enabled on the corresponding I/O pin. 
	// If PCINT[7:0] is set and the PCIE0 bit in PCICR is set, pin change interrupt is enabled on the corresponding I/O pin.
	// If PCINT[7:0] is cleared, pin change interrupt on the corresponding I/O pin is disabled.
	PCMSK0 |= (1 << PCINT5) | (1 << PCINT4);
	
	// enable global interrupts
	sei();

    while (1) 
    {
		uint16_t potiValue = ADC_readAnalogPin(0); // read analog input pin A0 on Arduino
		uint16_t calibratedPotiValue = mapSensorValueToFullRange(potiValue, detectedMinMaxPotiValue.sensorLowerBound, detectedMinMaxPotiValue.sensorUpperBound, 0, 1023);
		USART_writeString("Poti Value: ");
		USART_writeString(uint162char(potiValue));
		USART_writeString(", Calibrated Poti Value: ");
		USART_writeString(uint162char(calibratedPotiValue));
		USART_writeString("\r\n");
		
		if (motorSwitch == 1) {
			// adjust the direction of motor rotation
			if (motorDirection == 0) {
				sbi(PORTD,3);
				cbi(PORTD,2);
			} else if (motorDirection == 1) {
				cbi(PORTD,3);
				sbi(PORTD,2);
			} else {
				USART_writeString("Unknown Motor Direction!\r\n");
			}
			
			// add PWM
			
			
			
			
		} else {
			// when both driver input pin of the half-H are set to HIGH or LOW at the same time, the motor will stop
			cbi(PORTD,3);
			cbi(PORTD,2);
			USART_writeString("Motor switched off\r\n");
		}

		_delay_ms(1000);
    }
}

ISR(PCINT0_vect) {
	
	uint8_t changedBits;
	
	changedBits = PINB ^ historyOfPortB;
	historyOfPortB = PINB;

	if(changedBits & (1 << DDB4))
	{
		// invert bit 0 of the uint8_t motorSwitch
		motorSwitch ^= (1 << 0);
		USART_writeString("Motor Switch: ");
		USART_writeString(uint82char(motorSwitch));
		USART_writeString("\r\n");
	}
	
	if(changedBits & (1 << DDB5))
	{
		// invert bit 0 of the uint8_t motorSwitch
		motorDirection ^= (1 << 0);
		USART_writeString("Motor Direction: ");
		USART_writeString(uint82char(motorDirection));
		USART_writeString("\r\n");
	}

}

