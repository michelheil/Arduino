/**
 * @file main.c
 * @author Michael Heil
 * @brief Automated Watering
 * @version 0.1
 * @date 2020-01-16
 * 
 * @copyright Copyright (c) 2020
 * 
 * @section Features
 * Watering one plant using one Moisture Sensor and a mini water pump.
 * Sensor has been tested beforehand and is providing values from 
 * 675 (digged into water) to 0 (held up in the air).
 * 
 * 
 * @section Open Points
 * @li Power saving modes
 * @li Battery power supply
 * @li New water tank
 * @li safely attach pump and electornics near the water and plant
 * 
 * @section Obeservations
 * @li Figuring out best moisture threshold
 * @li lifetime of water pump
 * @li amount of watering
 */
#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define MOISTURE_ANALOG_PIN1 PA0
#define WATER_PUMP_PIN1 PB0
#define MOISTURE_VALUE_THRESHOLD 250

int main(void)
{
  // value to store the moisture of the plant
  uint16_t moistValue = 0;

/* ADC INIT - Start */
  // enable ADC
  ADCSRA |= (1 << ADEN); // Voltage reference and input channel selections will not go into effect until ADEN is set.
  
  // select ADC voltage reference
  // if bits REFS1 and REFS0  in register ADMUX are set to 0 then VCC is used as analog reference
  
  // select input channel
  // if bits MUX[5:0] in register ADMUX are set to zero then PA0 is used
  
  // set pre-scale factor to 8. Used as division factor between the system clock frequency and the input clock to the ADC.
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
  
  // By default, the result is presented right adjusted.
  // ADCL must be read first, then ADCH, to ensure that the content of the data registers belongs to the same conversion.
  // Once ADCL is read, ADC access to data registers is blocked.
  ADCSRA |= (1 << ADSC); // Start Initial Dummy Conversion
  while (ADCSRA & (1 << ADSC)) {} // Wait for conversion to complete
  moistValue = ADC; // do not use result of dummy conversion
  
  // for single ended conversion, the result is
  // ADC = ((V_in * 1024) / V_ref)
  // where V_in is the voltage on the selected input pin and V_ref the selected voltage reference
  
  // Important remark on "Moisture Sensor v1.4" (from Groove):
  // This can send as maximum analog voltage output: 3.3V
  // Therefore, the maximum measurable ADC (with V_ref = 5V) is "3.3V * 1024 / 5.0V = 675
  // During testing, this value is reached when the moisture sensor is held into water
/* ADC INIT - End */  

  // DIDR0 - Digital Input Disable Register 0
  // When a bit is written logic one, the digital input buffer on the corresponding ADC pin is disabled.
  DIDR0 |= (1 << ADC0D);

  // set input pins for moisture sensors
  DDRA &= ~(1 << MOISTURE_ANALOG_PIN1);

  // set water pump pin as output in Data Direction Register
  // DDRB |= (1 << WATER_PUMP_PIN1);
  DDRB |= (1 << WATER_PUMP_PIN1);
  
  while(1) 
  {
    // Start Conversion
    ADCSRA |= (1 << ADSC); 
    while (ADCSRA & (1 << ADSC)) {} // Wait for conversion to complete
    moistValue = ADC;

    // if moisture is too dry, activate water pump for 2 seconds
    if(moistValue < MOISTURE_VALUE_THRESHOLD)
    {
       PORTB |= (1 << WATER_PUMP_PIN1);
       _delay_ms(5000);
       PORTB &= ~(1 << WATER_PUMP_PIN1);
    }
    
    // ToDo: go to sleep mode and wake up every day once
    _delay_ms(10000);
  }
}



/* TEST MOISTURE SENSOR - Start

#define LED0 PB0
#define LED1 PB1
#define LED2 PB2


    // Start Conversion
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)) {} // Wait for conversion to complete
    moistValue = ADC;

    if (moistValue < 1) {
        PORTB |= (1 << LED0);
        } else if (moistValue < 50) {
        PORTB |= (1 << LED1);
        } else if (moistValue < 300) {
        PORTB |= (1 << LED1) | (1 << LED0);
        } else if (moistValue < 500) {
        PORTB |= (1 << LED2);
        } else if (moistValue < 600) {
        PORTB |= (1 << LED2) | (1 << LED0);
        } else if (moistValue < 700) {
        PORTB |= (1 << LED2) | (1 << LED1);
        } else {
        PORTB |= (1 << LED2) | (1 << LED1) | (1 << LED0);
    }

    _delay_ms(3000);
    PORTB |= (1 << LED2) | (1 << LED1) | (1 << LED0);
    _delay_ms(100);
    PORTB = 0;
    _delay_ms(100);
TEST MOISTURE SENSOR - End */    
    
