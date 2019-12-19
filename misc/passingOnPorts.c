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

#define HCSR04_SPEED_OF_SOUND 343.0f
#define HCSR04_TRIGGER_DURATION_US 100

#define TC16_COUNTER_REGISTER TCNT1

#define METER_IN_CM 100

float     TC16_init();
void      HCSR04_triggerMeasurement(volatile uint8_t * triggerPort, uint8_t triggerPin);
uint16_t  HCSR04_measureEchoDuration(volatile uint8_t * echoInputPin, uint8_t echoPin, volatile uint16_t * counterRegister);


int main(void)
{
  uint16_t durationInTicks;
  float durationInSec, distanceInCm;

  // Initialize USART and the 16-bit Timer/Counter
  USART_init();
  float tc16_ticksPerSecond = TC16_init();

  // set trigger pin as output and echo pin as input
  // ToDo: HCSR04 Klasse, die man fuer ein konkretes Paar an Trigger und Echo Pin definieren kann.
  // Einzelne methoden, wie ::measureDistance und helfer funktionen (private) wie z.B. HCSR04_triggerMeasurement und HCSR04_measureEchoDuration
  sbi(HCSR04_TRIGGER_DDR, HCSR04_TRIGGER_PIN);
  cbi(HCSR04_ECHO_DDR, HCSR04_ECHO_PIN);

  while(1)
  {
    _delay_ms(2000);

    // It is important that the trigger is followed directly by measuring echo duration.
    // Any other tasks in between (such as USART print outs) could take longer then the 
    // actual echo and hence the echo will happen unnotified.
    // trigger HCSR04 to start measurement
    HCSR04_triggerMeasurement(&HCSR04_TRIGGER_PORT, HCSR04_TRIGGER_PIN);

    durationInTicks = HCSR04_measureEchoDuration(&HCSR04_ECHO_INPUT_PIN, HCSR04_ECHO_PIN, &TC16_COUNTER_REGISTER);
    USART_writeString("Duration in Ticks: ");
    USART_writeStringLn(uint162str(durationInTicks));

    // convert duration in ticks into duration in seconds
    durationInSec = (float)durationInTicks / tc16_ticksPerSecond;

    // calculate distance based on speed of sound
    // division by factor 2 as sound goes back and forth
    distanceInCm = (float)durationInSec * HCSR04_SPEED_OF_SOUND * METER_IN_CM / 2.0f;

    // print distance
    USART_writeString("Distance in Centimeters: ");
    USART_writeFloat(distanceInCm);
    USART_newLine();
    USART_newLine();
  }
}

/**
 * @brief initialize Timer 1A with interrupt and a Clear Timer on Compare Match and a pre-scaler of 1024
 * 
 * @return float based on the preScaleFactor return the conversion between a seconds and number of ticks
 */
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

/**
 * @brief Trigger a distance measurement by setting trigger Pin at least 10us to high
 * @details using "volatile uint8_t * triggerPort" because PORTD is defined as
 *          #define PORTD (*(volatile uint8_t *)(0x2B))
 * 
 * @param triggerPort Port of the trigger pin
 * @param triggerPin Pin that triggers the measurement
*/
void HCSR04_triggerMeasurement(volatile uint8_t * triggerPort, uint8_t triggerPin)
{
    sbi(*triggerPort, triggerPin);
    _delay_us(HCSR04_TRIGGER_DURATION_US);
    cbi(*triggerPort, triggerPin);
}

/**
 * @brief measure the echo duration of HCSR04 device after it has been triggered
 * 
 * @param echoInputPin pointer to input pin of echo
 * @param echoPin echo pin
 * @param counterRegister pointer to the counter that can be used to measure the duration in ticks 
 * @return uint16_t duration of echo in ticks
 */
uint16_t HCSR04_measureEchoDuration(volatile uint8_t * echoInputPin, uint8_t echoPin, volatile uint16_t * counterRegister)
{
    uint16_t startTC16, endTC16;

    // wait for echo to start
    while(!(*echoInputPin & (1 << echoPin))) {}
    startTC16 = *counterRegister;

    // wait for echo to end
    while((*echoInputPin & (1 << echoPin)) == 1) {}
    endTC16 = *counterRegister;

    // calculate echo duration
    return endTC16 - startTC16;
}
