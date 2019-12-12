#include "myGlobalDefines.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#include "myHCSR04.h"
#include "myTC16.h"

#include "myUSART.h"

#define HCSR04_ECHO_PIN PD3 // INT1
#define HCSR04_TRIGGER_PIN PD4


void TC16_init();
volatile uint16_t startTC16, endTC16;
volatile uint8_t measurementIndex = 0; // 0 = not started, 1 = finished, else/default = waiting

int main(void)
{
  uint16_t durationInTicks;
  float durationInSec, durationInUs, distanceInCm;

  /* START INTERRUPT DISABLED CODE */
	cli(); // disable global interrupt setting

  USART_init();
  TC16_init();

	// set echo pin as input and trigger pin as output
	cbi(DDRD, HCSR04_ECHO_PIN);
  sbi(DDRD, HCSR04_TRIGGER_PIN);

  // Enabled INT1 (PD3) interrupt
  EIMSK = (1 << INT1);
  
  // The rising edge of INT1 generates an interrupt 
  EICRA = (1 << ISC11) | (1 << ISC10);

	sei(); // enable global interrupt setting
	/* END INTERRUPT DISABLED CODE */

  _delay_ms(2000);

  while(1)
  {
    switch(measurementIndex)
    {
      case 0: // measurement and trigger not started -> start trigger
        // Trigger (at least 10us high level signal)
        sbi(PORTD, HCSR04_TRIGGER_PIN);
        _delay_us(10);
        cbi(PORTD, HCSR04_TRIGGER_PIN);
        
        // reset counter value
        TCNT1 = 0;  
        startTC16 = TCNT1; // start counting
        USART_writeString("Start Counter: ");
        USART_writeStringLn(uint162str(startTC16));

        measurementIndex = 1; // indicate start of measurement
        USART_writeStringLn("Measurement started. Waiting for echo...");
        break;

      case 1:
        // calculate duration
        durationInTicks = endTC16 - startTC16;
        USART_writeString("Duration in Ticks: ");
        USART_writeStringLn(uint162str(durationInTicks));

        // convert duration in ticks into duration in seconds
        durationInSec = (float)durationInTicks / 15625.0f;

        // convert to us
        durationInUs = (float)durationInSec * 1000000.0f;

        // us/58 = centimeters
        distanceInCm = (float)durationInUs/58.0f;

        // print result to USART
        USART_writeString("Distance in Centimeters: ");
        USART_writeFloat(distanceInCm);
        USART_newLine();

        // Use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
        _delay_ms(3000); // wait before next measurement

        // reset measurement flag to enable another measurement
        measurementIndex = 0; 

      default: 
        USART_writeStringLn("Waiting for echo...");
    }
  }
}

// initialize Timer 1A with interrupt and a Clear Timer on Compare Match and a pre-scaler of 1024
void TC16_init()
{
	// Timer/Counter Control Register 1A/1B
	TCCR1A = 0; // normal port operation, OCA1/OCB1 disconnected

	// The Output Compare Registers (OCR1A) contain a 16-bit value that is continuously compared
	// with the counter value (TCNT1). A match can be used to generate an Output Compare interrupt.
	OCR1A = 65535; // max value = 65535 (1 second = (15624 + 1) = 16MHz / 1024)

	// Setting only WGM12 on TCCR1B activates the CTC (Clear Timer on Compare Match) mode
	// Bits on CS12 and CS10 set the pre scale factor to 1024
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
}

// Interrupt if echo starts
ISR(INT1_vect)
{
  endTC16 = TCNT1;
  USART_writeString("End Counter: ");
  USART_writeStringLn(uint162str(endTC16));
  measurementIndex = 1;
}