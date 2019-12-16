#include "myGlobalDefines.h"
#include <util/delay.h>

#include "myHCSR04.h"
#include "myTC16.h"

#include "myUSART.h"

#define HCSR04_TRIGGER_PIN PD4
#define HCSR04_TRIGGER_PORT PORTD
#define HCSR04_TRIGGER_DDR DDRD

#define HCSR04_ECHO_PIN PB0
#define HCSR04_ECHO_PORT PORTB
#define HCSR04_ECHO_DDR DDRB
#define HCSR04_ECHO_INPUT_PIN PINB

#define TC16_TICKS_PER_SECOND (15625.0f)

#define HCSR04_SPEED_OF_SOUND 343.0f
#define HCSR04_TRIGGER_DURATION_US 100

#define METER_IN_CM 100

void TC16_init();
void HCSR04_triggerMeasurement(uint8_t triggerPort, uint8_t triggerPin);


int main(void)
{
  uint16_t durationInTicks, startTC16, endTC16;
  float durationInSec, distanceInCm;

  // Initialize USART and the 16-bit Timer/Counter
  USART_init();
  TC16_init();

  // set trigger pin as output and echo pin as input
  // void HCSR04_setDataDirectionRegister()
  DDRD |= (1 << HCSR04_TRIGGER_PIN);
  DDRB &= ~(1 << HCSR04_ECHO_PIN);

  while(1)
  {
    // trigger HCSR04 to start measurement
    PORTD |= (1 << HCSR04_TRIGGER_PIN);
    _delay_us(HCSR04_TRIGGER_DURATION_US);
    PORTD &= ~(1 << HCSR04_TRIGGER_PIN);

    // wait for echo to start
    while(!(PINB & (1 << HCSR04_ECHO_PIN))) {}
    startTC16 = TCNT1;

    // wait for echo to end
    while((PINB & (1 << HCSR04_ECHO_PIN)) == 1) {}
    endTC16 = TCNT1;

    // print values for start and end counter
    USART_writeString("Start Counter: ");
    USART_writeStringLn(uint162str(startTC16));
    USART_writeString("End Counter: ");
    USART_writeStringLn(uint162str(endTC16));

    // calculate echo duration
    durationInTicks = endTC16 - startTC16;
    USART_writeString("Duration in Ticks: ");
    USART_writeStringLn(uint162str(durationInTicks));

    // convert duration in ticks into duration in seconds
    durationInSec = (float)durationInTicks / TC16_TICKS_PER_SECOND;

    // calculate distance based on speed of sound
    // division by factor 2 as sound goes back and forth
    distanceInCm = (float)durationInSec * HCSR04_SPEED_OF_SOUND * METER_IN_CM / 2.0f;

    // print distance
    USART_writeString("Distance in Centimeters: ");
    USART_writeFloat(distanceInCm);
    USART_newLine();
  }
}

// initialize Timer 1A with interrupt and a Clear Timer on Compare Match and a pre-scaler of 1024
void TC16_init()
{
	// Timer/Counter Control Register 1A/1B
	TCCR1A = 0; // normal port operation, OCA1/OCB1 disconnected

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
