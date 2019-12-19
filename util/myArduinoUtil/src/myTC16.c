/*
 * myTC16.c
 *
 * Created: 12.09.2019 22:53:01
 *  Author: Michael
 */ 

#include "myTC16.h"

#include <avr/io.h>


float TC16_init(int factor)
{
	// Timer/Counter Control Register 1A/1B
	TCCR1A = 0; // normal port operation, OCA1/OCB1 disconnected

	// The Output Compare Registers (OCR1A) contain a 16-bit value that is continuously compared
	// with the counter value (TCNT1). A match can be used to generate an Output Compare interrupt.
	OCR1A = 0xFFFF;//23767; // max value = 65535 (1 second = (15624 + 1) = 16MHz / 1024)

	// Setting only WGM12 on TCCR1B activates the CTC (Clear Timer on Compare Match) mode
	TCCR1B = (1 << WGM12);
  
  // Setting pre-scaling factor using bit CS12, CS11, and CS10
  switch(factor)
  {
    case 1024:
      TCCR1B |= (1 << CS12) | (1 << CS10);
      break;
    case 256:
      TCCR1B |= (1 << CS12);
      break;
    case 64:
      TCCR1B |= (1 << CS11) | (1 << CS10);
      break;
    case 8:
      TCCR1B |= (1 << CS11);
      break;
    case 1:
      TCCR1B |= (1 << CS10);
      break;
    default:
      factor = 1024;
      TCCR1B |= (1 << CS12) | (1 << CS10);
  }
  
  return F_CPU / (float)factor;
}
