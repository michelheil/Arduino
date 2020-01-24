// #include <Arduino.h>

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <myTWI.h>

#define LED_PIN PB0
// INT0 (PD2) is interrupt pin

int main(void)
{
  cli();

  // set LED_PIN as output pin
  DDRB |= (1 << LED_PIN);

  // External Interrupt Mask Register
  // When the INT0 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), the external pin interrupt is enabled.
  EIMSK |= (1 << INT0);

  // The rising edge of INT0 generates an interrupt request
  EICRA |= (1 << ISC00) | (1 << ISC00);

  // prepare sleep mode
  SMCR |= (1 << SM1); // power-down mode
  SMCR |= (1 << SE); // sleep enable

  sei();

  while(1)
  {
    PORTB |= (1 << LED_PIN);
    _delay_ms(500);
    PORTB &= ~(1 << LED_PIN);
    _delay_ms(500);
    PORTB |= (1 << LED_PIN);
    _delay_ms(500);
    PORTB &= ~(1 << LED_PIN);
    _delay_ms(500);

    __asm__ __volatile__("sleep");
  }
}

ISR(INT0_vect)
{
 // do nothing
}