/*
 * myHCSR04.c
 *
 * Created: 10.12.2019 22:53:01
 *  Author: Michael
 */ 

#include "myHCSR04.h"

#include "myGlobalDefines.h"
#include <avr/io.h>
#include <util/delay.h>

/**
 * @brief Construct a new HCSR04::HCSR04 object
 * @details set wired pins and the conversion factor of the used Timer/Counter
 * 
 * @param triggerP name of wired trigger pin
 * @param echoP name of wired echo pin
 * @param ticksPerSec factor to convert ticks into seconds based on the used Timer/Counter
 */
HCSR04::HCSR04(uint8_t triggerP, uint8_t echoP, float ticksPerSec):
    triggerPin(triggerP), echoPin(echoP), ticksPerSecond(ticksPerSec) {

  sbi(DDRD, triggerPin);
  cbi(DDRB, echoPin);
}

/**
 * @brief Destroy the HCSR04::HCSR04 object
 */
HCSR04::~HCSR04(void) {}

/**
 * @brief main function of HCSR04 class that returns the distance in centimeters
 * 
 * @return float measured distance in centimeters
 */
float HCSR04::measureDistanceInCm()
{
  int meterInCm = 100; // conversion factor
  int doubleTravelDistance = 2; // sound travelling back and forth

  // data sheet: we suggest to use over 60ms measurement cycle, in order to prevent 
  // trigger signal to the echo signal.
  _delay_ms(65);

  // It is important that the trigger is followed directly by measuring echo duration.
  // Any other tasks in between (such as USART print outs) could take longer then the 
  // actual echo and hence the echo will happen unnotified.
  // trigger HCSR04 to start measurement
  triggerMeasurement();
  uint16_t durationInTicks = measureEchoDuration();

  // convert duration in ticks into duration in seconds
  float durationInSec = (float)durationInTicks / ticksPerSecond;

  // calculate distance based on speed of sound
  // division by factor 2 as sound goes back and forth
  float result = durationInSec * (float)speedOfSound * (float)meterInCm / (float)doubleTravelDistance;

  if (result < 0) {
    return lastMeasuredDistanceInCm;
  } else {
    lastMeasuredDistanceInCm = result;
    return result;
  }
}

/**
 * @brief Trigger a distance measurement by setting trigger Pin at least 10us to high
 */
void HCSR04::triggerMeasurement()
{
  sbi(PORTD, triggerPin);
  _delay_us(triggerDurationUs);
  cbi(PORTD, triggerPin);
}

/**
 * @brief measure the echo duration of HCSR04 device after it has been triggered
 * 
 * @return uint16_t duration of echo in ticks
 */
uint16_t HCSR04::measureEchoDuration()
{
    uint16_t startTC16, endTC16;

    // wait for echo to start
    while(!(PINB & (1 << echoPin))) {}
    startTC16 = TCNT1;

    // wait for echo to end
    while((PINB & (1 << echoPin)) == 1) {}
    endTC16 = TCNT1;

    // calculate echo duration
    if(endTC16 < startTC16) { 
      // in case the timer hits its maximum value and starts from beginning (0)
      return ((ticksPerSecond + endTC16) - startTC16);
    }
    else {
      return endTC16 - startTC16;
    }
}