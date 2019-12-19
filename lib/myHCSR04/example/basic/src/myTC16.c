/*
 * myTC16.c
 *
 * Created: 12.09.2019 22:53:01
 *  Author: Michael
 */ 

#include "myTC16.h"

#include <avr/io.h>


float TC16_init()
{
	// Timer/Counter Control Register 1A/1B
	TCCR1A = 0; // normal port operation, OCA1/OCB1 disconnected

	// The Output Compare Registers (OCR1A) contain a 16-bit value that is continuously compared
	// with the counter value (TCNT1). A match can be used to generate an Output Compare interrupt.
	OCR1A = 23767; // max value = 65535 (1 second = (15624 + 1) = 16MHz / 1024)

	// Setting only WGM12 on TCCR1B activates the CTC (Clear Timer on Compare Match) mode
	// Bits on CS12 and CS10 set the pre scale factor to 1024
  int preScaleFactor = 1024;
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

  return F_CPU / (float)preScaleFactor;
}

