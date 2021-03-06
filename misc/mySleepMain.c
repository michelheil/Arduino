/**
 * @file main.c
 * @author Michael Heil
 * @brief trying out the sleep mode power-down
 * @version 0.1
 * @date 2020-01-14
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED_PIN PB0

int main(void)
{
  // activate all pull-up resistors of non-used pins
  DDRB = (1 << PB0);
  DDRC = 0x00;
  DDRD = 0x00;
  PORTB = 0xFF & ~(1 << PB0);
  PORTC = 0xFF;
  PORTD = 0xFF;

  WDTCSR = (1 << WDCE) | (1 << WDE); // Set Watchdog Change Enable and Watchdog System Reset Enable
  WDTCSR = (1 << WDP3) | (1 << WDP0); // Time out at 8.0s (at VCC = 5.0V) and also clearing WDCE and WDE
  WDTCSR |= (1 << WDIE); // Watchdog Interrupt Enable; If WDE is cleared in combination with this setting, the watchdog timer is in interrupt mode,
                         // and the corresponding interrupt is executed if time-out in the watchdog timer occurs.

  ADCSRA &= ~(1 << ADEN); // disable ADC

  SMCR |= (1 << SM1); // power-down mode
  SMCR |= (1 << SE); // sleep enable

  sei();

  while(1)
  {
    PORTB |= (1 << PB0);
    _delay_ms(5000);
    PORTB &= ~(1 << PB0);

    // "sleep" for c. 64 seconds
    for(int ii = 0; ii < 8; ii++)
    {
      // If Brown-Out Detector (BOD) is disabled by Software, the BOD function is turned off immediately after entering the
      // sleep mode. Upon wake-up from sleep, BOD is automatically enabled again.
      // When the BOD has been disabled, the wake-up time from sleep mode will be approximately 60us to ensure that the BOD
      // is working correctly before the MCU continues executing code.
      //
      // In order to disable BOD during sleep the BODS bit must be written to logic one. First, the BODS and BODSE must be 
      // set to one. Second, within four clock cycles, BODS must be set to one and BODSE must be set to zero. The BODS bit 
      // is active three clock cycles after it is set. A sleep instruction must be executed while BODS is active in order
      // to turn off the BOD for the actual sleep mode. The BODS bit is automatically cleared after three clock cycles.
      MCUCR |= (1 << BODS) | (1 << BODSE); // set both BODS and BODSE at the same time
      MCUCR = (MCUCR & ~(1 << BODSE)) | (1 << BODS); // then set the BODS bit and clear the BODSE bit at the same time
      __asm__ __volatile__("sleep");
    }
  }
}

// Watchdog interrupt
ISR(WDT_vect)
{
  // do nothing
}
