/**
 * @file main.c
 * @author Michael Heil
 * @brief Example for using Alarm function of DS3231 to wake up Arduino from deep sleep
 * @version 0.1
 * @date 2020-01-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <myGlobalDefines.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <myTWI.h>
#include <myDS3231.h>

#define LED_PIN PB0
#define WAKE_UP_INTERRUPT_PIN PD2

int main(void)
{
  // disable global interrupt
  cli();

  // enable TWI
  DS3231_init();

  // set alarm once per minute
  DS3231_setAlarmOncePerMinute();

  // set LED_PIN as output pin
  DDRB |= (1 << LED_PIN);

  // activate pull-up on PD2
  DDRD &= (1 << WAKE_UP_INTERRUPT_PIN);
  PORTD |= (1 << WAKE_UP_INTERRUPT_PIN);

  // External Interrupt Mask Register
  // When the INT0 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), the external pin interrupt is enabled.
  EIMSK |= (1 << INT0);

  // The falling edge of INT0 generates an interrupt request
  // DS3231 produces active-low interrupt
  EICRA |= (1 << ISC01);

  // prepare sleep mode
  SMCR |= (1 << SM1); // power-down mode
  SMCR |= (1 << SE); // sleep enable

  // enable global interrupt
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

    // reset Alarm flags
    DS3231_resetAlarmsFlags();
    __asm__ __volatile__("sleep");
  }
}

ISR(INT0_vect)
{
  // do nothing
}