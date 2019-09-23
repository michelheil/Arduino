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

	// set PB1 as input PIN (pull-up resistor in PORTB not required as we use pull-down register on bread board)
	DDRB &= ~((1 << DDB2) | (1 << DDB1));
	
	// Pin Change Interrupt Control Register
	// When the PCIE0 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), pin change interrupt 0 is enabled. 
	// Any change on any enabled PCINT[7:0] pin will cause an interrupt. The corresponding interrupt of Pin Change Interrupt Request
	// is executed from the PCI0 Interrupt Vector. PCINT[7:0] pins are enabled individually by the PCMSK0 Register.
	PCICR |= (1 << PCIE0);
	
	// Pin Change Mask Register 0
	// Each PCINT[7:0] bit selects whether pin change interrupt is enabled on the corresponding I/O pin. 
	// If PCINT[7:0] is set and the PCIE0 bit in PCICR is set, pin change interrupt is enabled on the corresponding I/O pin.
	// If PCINT[7:0] is cleared, pin change interrupt on the corresponding I/O pin is disabled.
	PCMSK0 |= (1 << PCINT2) | (1 << PCINT1);
	
	// enable global interrupts
	sei();

    while (1) 
    {
		
    }
}

ISR(PCINT0_vect) {
	
	uint8_t changedBits;
	
	changedBits = PINB ^ historyOfPortB;
	historyOfPortB = PINB;

	if(changedBits & (1 << DDB1))
	{
		// invert bit 0 of the uint8_t motorSwitch
		motorSwitch ^= (1 << 0);
		USART_writeString("Motor Switch: ");
		USART_writeString(uint82char(motorSwitch));
		USART_writeString("\r\n");
	}
	
	if(changedBits & (1 << DDB2))
	{
		// invert bit 0 of the uint8_t motorSwitch
		motorDirection ^= (1 << 0);
		USART_writeString("Motor Direction: ");
		USART_writeString(uint82char(motorDirection));
		USART_writeString("\r\n");
	}

}

