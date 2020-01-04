/**
 * @file main.c
 * @author Michael Heil
 * @brief Automated Watering
 * @version 0.1
 * @date 2020-01-04
 * 
 * @copyright Copyright (c) 2020
 * 
 * @section Features
 * Watering one plant using one Moisture Sensor and a mini water pump.
 * Sensor has been tested beforehand and is providing values from 
 * 650 (digged into water) to 0 (held up in the air).
 * 
 * 
 * @section Open Points
 * @li Power saving modes of Arduino
 * @li Battery power supply for Arduino
 * @li New water tank
 * @li ESP8266/ESP32
 * @li using ATTiny instead of Arduino
 * @li Verwende "Verteilerbox" und nicht Eier-Pappe
 * @li safely attach pump and electornics near the water and plant
 * 
 * @section Obeservations
 * @li Figuring out best moisture threshold
 * @li lifetime of water pump
 * @li amount of watering
 */

#include <Arduino.h>
#include <util/delay.h>
#include "myGlobalDefines.h"
#include "myADC.h"

#define MOISTURE_ANALOG_PIN 0
#define MOISTURE_VALUE_THRESHOLD 200
#define WATER_PUMP_PIN PD2

int main(void)
{
  // value to store the moisture of the plant
  uint16_t moistValue;

  // activate ADC of ATMega328P
  ADC_init();

  // set water pump pin as output in Data Direction Register
  DDRD |= (1 << WATER_PUMP_PIN);

  while(1)
  {
    // read current moisture value
    moistValue = ADC_readAnalogPin(MOISTURE_ANALOG_PIN);
   
    // is too dry, activate water pump for 2 seconds
    if(moistValue < MOISTURE_VALUE_THRESHOLD)
    {
      sbi(PORTD, WATER_PUMP_PIN);
      _delay_ms(2000);
      cbi(PORTD, WATER_PUMP_PIN);
    }

    // idle 1 minute before checking again
    for(int i = 0; i < 6; i++)
    {
      _delay_ms(10000);
    }
  }
}
