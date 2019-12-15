#include "myGlobalDefines.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "myHCSR04.h"
#include "myTC16.h"

#include "myUSART.h"

#define HCSR04_ECHO_PIN PB0
#define HCSR04_TRIGGER_PIN PD4
#define HCSR04_TRIGGER_DURATION_US 100

#define SAMPLING 100

#define M_IN_CM 100

#define MY_PIND _SFR_IO8(0x09)


void TC16_init();
//void HCSR04_triggerMeasurement(uint8_t triggerPort, uint8_t triggerPin);
volatile uint16_t startTC16, endTC16, waitTC16;
//volatile uint8_t measurementIndex = 0; // 0 = not started, so start trigger, 1 = waiting, 2 = finished

int main(void)
{
  uint16_t durationInTicks;
  float durationInSec, durationInUs, distanceInCm;

	// disable global interrupt setting
  //cli();

  // Initialize USART and the 16-bit Timer/Counter
  USART_init();
  TC16_init();

	// set echo pin as input and trigger pin as output
	//cbi(DDRD, HCSR04_ECHO_PIN);
  //sbi(DDRD, HCSR04_TRIGGER_PIN);
  //PORTD = 0;
  DDRD |= (1 << PD4);
  DDRB &= ~(1 << HCSR04_ECHO_PIN);

  // Enable INT1 (PD3) interrupt
  //EIMSK = (1 << INT1);
  
  // The rising edge of INT1 generates an interrupt 
  //EICRA = (0 << ISC11) | (1 << ISC10);

//  _delay_ms(1000);

  //sei(); // enable global interrupt setting

  while(1)
  {

_delay_ms(1000);
uint8_t echoValue;

PORTD |= (1 << PD4);
        _delay_us(25);
        PORTD &= ~(1 << PD4);

while(!(PINB & (1 << PB0))) {}
startTC16 = TCNT1;

while((PINB & (1 << PB0)) == 1) {}
        endTC16 = TCNT1;


        USART_writeString("Start Counter: ");
        USART_writeStringLn(uint162str(startTC16));

        USART_writeString("End Counter: ");
        USART_writeStringLn(uint162str(endTC16));

        durationInTicks = endTC16 - startTC16;
        USART_writeString("Duration in Ticks: ");
        USART_writeStringLn(uint162str(durationInTicks));


        // convert duration in ticks into duration in seconds
        durationInSec = (float)durationInTicks / 15625.0f;

        distanceInCm = (float)durationInSec * 340.0f * M_IN_CM / 2.0f;

        USART_writeString("Distance in Centimeters: ");
        USART_writeFloat(distanceInCm);
        USART_newLine();


/*
do
{
  uint8_t echoValue = (PIND & (1 << PD3));
}

        while((PIND & (1 << PD3)) == 0) {USART_writeStringLn("Wait 0");}
        startTC16 = TCNT1;

      while((PIND & (1 << PD3)) == 1) {USART_writeStringLn("Wait 1");}
        endTC16 = TCNT1;


        USART_writeString("Start Counter: ");
        USART_writeStringLn(uint162str(startTC16));

        USART_writeString("End Counter: ");
        USART_writeStringLn(uint162str(endTC16));

        durationInTicks = endTC16 - startTC16;
        USART_writeString("Duration in Ticks: ");
        USART_writeStringLn(uint162str(durationInTicks));

        // convert duration in ticks into duration in seconds
        durationInSec = (float)durationInTicks / 15625.0f;

        distanceInCm = (float)durationInSec * 340000.0f / 2.0f;

        USART_writeString("Distance in Centimeters: ");
        USART_writeFloat(distanceInCm);
        USART_newLine();

        // Use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
        _delay_ms(1000); // wait before next measurement




*/
    
/*
    switch(measurementIndex)
    {
      case 0: // measurement not started -> start trigger
        // reset counter value
        TCNT1 = 0;

        // Trigger (at least 10us high level signal)
        //HCSR04_triggerMeasurement(PORTD, HCSR04_TRIGGER_PIN);
        PORTD |= (1 << PD4);
        _delay_us(100);
        PORTD &= ~(1 << PD4);
        
        // start counting
        startTC16 = TCNT1;
        USART_writeString("Start Counter: ");
        USART_writeStringLn(uint162str(startTC16));

        // indicate waiting
        measurementIndex = 1;
        USART_writeStringLn("Measurement started. Waiting for echo...");
        
        break;

      case 1:
        //USART_writeStringLn("Waiting for echo...");
        waitTC16 = TCNT1;
        //USART_writeString("Wait Counter: ");
        //USART_writeStringLn(uint162str(waitTC16));
        break;

      case 2:
        // print end of timer stopped in Interrupt Service Routine
        USART_writeString("End Counter: ");
        USART_writeStringLn(uint162str(endTC16));

        // calculate duration
        durationInTicks = endTC16 - startTC16;
        USART_writeString("Duration in Ticks: ");
        USART_writeStringLn(uint162str(durationInTicks));

        // convert duration in ticks into duration in seconds
        durationInSec = (float)durationInTicks / 15625.0f;

        // convert to us
        durationInUs = (float)durationInSec * 1000000.0f;

        // us/58 = centimeters
        distanceInCm = (float)durationInUs / 58.0f;

        // print result to USART
        USART_writeString("Distance in Centimeters: ");
        USART_writeFloat(distanceInCm);
        USART_newLine();

        // Use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
        _delay_ms(3000); // wait before next measurement

        // reset measurement flag to enable another measurement
        measurementIndex = 0; 
    }
*/
  }
}

// initialize Timer 1A with interrupt and a Clear Timer on Compare Match and a pre-scaler of 1024
void TC16_init()
{
	// Timer/Counter Control Register 1A/1B
	TCCR1A = 0; // normal port operation, OCA1/OCB1 disconnected

  // Reset Counter
  TCNT1 = 0;

	// The Output Compare Registers (OCR1A) contain a 16-bit value that is continuously compared
	// with the counter value (TCNT1). A match can be used to generate an Output Compare interrupt.
	OCR1A = 23767; // max value = 65535 (1 second = (15624 + 1) = 16MHz / 1024)

	// Setting only WGM12 on TCCR1B activates the CTC (Clear Timer on Compare Match) mode
	// Bits on CS12 and CS10 set the pre scale factor to 1024
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
}

/**
 * @brief Trigger a distance measurement by setting trigger Pin at least 10us to high
 * 
 * @param triggerPort Port of the trigger pin
 * @param triggerPin Pin that triggers the measurement
*/
void HCSR04_triggerMeasurement(uint8_t triggerPort, uint8_t triggerPin)
{
  PORTD |= (1 << triggerPin);
  _delay_us(20);
  PORTD &= ~(1 << triggerPin);
}

/*
// Interrupt if echo starts
ISR(INT1_vect)
{
  endTC16 = TCNT1; // end counting
  measurementIndex = 2; // indicate measurement has ended
}
*/