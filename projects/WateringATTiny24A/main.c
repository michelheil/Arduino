/**
 * @file main.c
 * @author Michael Heil
 * @brief Automated Watering
 * @version 1.1
 * @date 2020-01-21
 * 
 * @copyright Copyright (c) 2020
 * 
 * @section Features
 * Watering one plant using two Capacitive Soil Moisture Sensor v1.2
 * and two mini water pumps.
 * Sensor has been tested beforehand and is providing following values:
 * Dry: 650 (4.5V) / 600 (5V)
 * Water: 400 (4.5V) / 360 (5V)
 * Threshold to activate pump: moistureValue > 550 (4.5V) / 500 (5V)
 * 
 * @section Open Points
 * @li New water tank
 * @li Verwende "Verteilerbox" und nicht Eier-Pappe
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

#define MOISTURE_ANALOG_PIN0 PA0
#define MOISTURE_ANALOG_PIN1 PA1
#define WATER_PUMP_PIN0 PB0
#define WATER_PUMP_PIN1 PB1
#define MOISTURE_VALUE_THRESHOLD 550

int main(void)
{
  // value to store the moisture of the plants
  uint16_t moistValue0, moistValue1;
  
  // set input pins for moisture sensors
  DDRA &= ~(1 << MOISTURE_ANALOG_PIN0);
  DDRA &= ~(1 << MOISTURE_ANALOG_PIN1);

  // set water pump pin as output in Data Direction Register
  DDRB |= (1 << WATER_PUMP_PIN0);
  DDRB |= (1 << WATER_PUMP_PIN1);

/* ADC INIT - Start */
  // enable ADC
  ADCSRA |= (1 << ADEN); // Voltage reference and input channel selections will not go into effect until ADEN is set.
  
  // select ADC voltage reference
  // if bits REFS1 and REFS0  in register ADMUX are set to 0 then VCC is used as analog reference
  
  // set pre-scale factor to 8. Used as division factor between the system clock frequency and the input clock to the ADC.
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
  
  // By default, the result is presented right adjusted.
  // ADCL must be read first, then ADCH, to ensure that the content of the data registers belongs to the same conversion.
  // Once ADCL is read, ADC access to data registers is blocked.
  ADCSRA |= (1 << ADSC); // Start Initial Dummy Conversion
  while (ADCSRA & (1 << ADSC)) {} // Wait for conversion to complete
  ADC; // do not use result of dummy conversion
  
  // for single ended conversion, the result is
  // ADC = ((V_in * 1024) / V_ref)
  // where V_in is the voltage on the selected input pin and V_ref the selected voltage reference
/* ADC INIT - End */  

  // DIDR0 - Digital Input Disable Register 0
  // When a bit is written logic one, the digital input buffer on the corresponding ADC pin is disabled.
  DIDR0 = 0xFF; // disable all digital inputs on analog pins
  
  // Setup Watchdog as a wakeup source from Power-down Sleep Mode.
  // Use Timed Sequence for disabling Watchdog System Reset Mode if it has been enabled unintentionally.
  MCUSR  &= ~(1 << WDRF);               // Clear WDRF (Watchdog Reset Flag) if it has been unintentionally set.
  WDTCSR =  (1 << WDCE) | (1 << WDE);   // Enable configuration change.
  WDTCSR =  (1 << WDIF) | (1 << WDIE) | // Enable Watchdog Interrupt Mode. If WDE is cleared in combination with 
                                        // this setting, the watchdog timer is in interrupt mode, and the corresponding
                                        // interrupt is executed if time-out in the watchdog timer occurs.
                                        // WDIF is cleared by writing a logic one to the flag
            (1 << WDCE) | (0 << WDE)  | // Disable Watchdog System Reset Mode if unintentionally enabled.
            (1 << WDP3) | (0 << WDP2) | (0 << WDP1) | (1 << WDP0); // Set Watchdog Timeout period to 8.0 sec.
  
  // power-down mode and enable sleep
  MCUCR |= (1 << SE) | (1 << SM1);
  
  // reduce power
  PRR |= (1 << PRTIM1) | // Power Reduction Timer/Counter1
         (1 << PRTIM0) | // Power Reduction Timer/Counter0
         (1 << PRUSI);   // Power Reduction USI

  // enable global interrupts  
  sei();

  while(1) 
  {
/* Measure Input PA0 - Start */
    // Select input channel
    // if bits MUX[5:0] in register ADMUX are set to zero then PA0 is used
    ADMUX &= ~(1 << MUX0); // clear MUX0 bit
      
    // Start Conversion
    ADCSRA |= (1 << ADSC); 
    
    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC)) {} 
        
    // store ADC output
    moistValue0 = ADC;
/* Measure Input PA0 - End */    

/* Water Plant 0 - Start */
    // if moisture is too dry, activate water pump for 2 seconds
    if(moistValue0 > MOISTURE_VALUE_THRESHOLD)
    {
       PORTB |= (1 << WATER_PUMP_PIN0);       
       _delay_ms(2500);
       PORTB &= ~(1 << WATER_PUMP_PIN0);
    }
    
    // watchdog timer reset (wdr)
    __asm__ __volatile__("wdr");
/* Water Plant 0 - End */

/* Measure Input PA1 - Start */
    // Select input channel
    // if bits MUX[5:0] in register ADMUX are set to zero then PA0 is used
    ADMUX |= (1 << MUX0); // set MUX0 bit

    // Start Conversion
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC)) {}

    // store ADC output
    moistValue1 = ADC;
/* Measure Input PA1 - End */

/* Water Plant 1 - Start */
    // if moisture is too dry, activate water pump for 2 seconds
    if(moistValue1 > MOISTURE_VALUE_THRESHOLD)
    {
        PORTB |= (1 << WATER_PUMP_PIN1);
        _delay_ms(2500);
        PORTB &= ~(1 << WATER_PUMP_PIN1);
    }

    // watchdog timer reset (wdr)
    __asm__ __volatile__("wdr");
/* Water Plant 1 - End */
    
    // "sleep" for c. 1 hour and repeatedly disable BOD
    for(int ii = 0; ii < 8; ii++) // 480 = 1 hour
    {
      // If Brown-Out Detector (BOD) is disabled by Software, the BOD function is turned off immediately after entering the
      // sleep mode. Upon wake-up from sleep, BOD is automatically enabled again.
      // When the BOD has been disabled, the wake-up time from sleep mode will be approximately 60us to ensure that the BOD
      // is working correctly before the MCU continues executing code.
      //
      // In order to disable BOD during sleep the BODS bit must be written to logic one. First, the BODS and BODSE must be
      // set to one. Second, within four clock cycles, BODS must be set to one and BODSE must be set to zero. The BODS bit
      // is active three clock cycles after it is set. A sleep instruction must be executed while BODS is active in order
      // to turn off the BOD for the actual sleep mode. The BODS bit is automatically cleared after three clock cycles.
      MCUCR |= (1 << BODS) | (1 << BODSE); // set both BODS and BODSE at the same time
      MCUCR = (MCUCR & ~(1 << BODSE)) | (1 << BODS); // then set the BODS bit and clear the BODSE bit at the same time
      
      // going to sleep
      __asm__ __volatile__("sleep");
    }
  }
}


// Watchdog interrupt
ISR(WDT_vect)
{
    // do nothing
}
