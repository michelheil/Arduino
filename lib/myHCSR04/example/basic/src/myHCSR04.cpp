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

HCSR04::HCSR04() {}

HCSR04::~HCSR04() {}

/**
 * @brief initialize by setting the trigger and echo pins and define 
 *        ticksPerSecond conversion factor from the used Timer
 * @details the Data Direction Registers of wired pins are also set
*/
void HCSR04::init(uint8_t triggerPin, uint8_t echoPin, float ticksPerSecond)
{
  _triggerPin = triggerPin;
  _echoPin = echoPin;
  _ticksPerSecond = ticksPerSecond;

  sbi(DDRD, _triggerPin);
  cbi(DDRB, _echoPin);
}

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
  float durationInSec = (float)durationInTicks / _ticksPerSecond;

  // calculate distance based on speed of sound
  // division by factor 2 as sound goes back and forth
  return (float)(durationInSec * (float)_speedOfSound * (float)meterInCm / (float)doubleTravelDistance);
}

/**
 * @brief Trigger a distance measurement by setting trigger Pin at least 10us to high
 */
void HCSR04::triggerMeasurement()
{
  sbi(PORTD, _triggerPin);
  _delay_us(_triggerDurationUs);
  cbi(PORTD, _triggerPin);
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
    while(!(PINB & (1 << _echoPin))) {}
    startTC16 = TCNT1;

    // wait for echo to end
    while((PINB & (1 << _echoPin)) == 1) {}
    endTC16 = TCNT1;

    // calculate echo duration
    if(endTC16 < startTC16) { 
      // in case the timer hits its maximum value and starts from beginning (0)
      return ((_ticksPerSecond + endTC16) - startTC16);
    }
    else {
      return endTC16 - startTC16;
    }
}