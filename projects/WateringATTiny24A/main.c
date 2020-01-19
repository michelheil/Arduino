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
 * c. 675 (digged into water) to 0 (held up in the air).
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

// https://github.com/MetreIdeas/ATtinyx4A_ADC_Example/blob/master/ATtinyx4A_ADC_Example.c

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
  
  // set input pins for moisture sensors
  DDRA &= ~(1 << MOISTURE_ANALOG_PIN1);

  // set water pump pin as output in Data Direction Register
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
  
  // Important remark on "Moisture Sensor v1.4" (from Groove):
  // This can send as maximum analog voltage output: 3.3V
  // Therefore, the maximum measurable ADC (with V_ref = 5V) is "3.3V * 1024 / 5.0V = 675
  // During testing, this value is reached when the moisture sensor is held into water
/* ADC INIT - End */  

  // DIDR0 - Digital Input Disable Register 0
  // When a bit is written logic one, the digital input buffer on the corresponding ADC pin is disabled.
  DIDR0 |= (1 << ADC0D);
  
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

  // enable global interrupts  
  sei();
  
  while(1) 
  {
    // Select input channel
    // if bits MUX[5:0] in register ADMUX are set to zero then PA0 is used
      
    // Start Conversion
    ADCSRA |= (1 << ADSC); 
    
    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC)) {} 
        
    // store ADC output
    moistValue = ADC;

    // if moisture is too dry, activate water pump for 2 seconds
    if(moistValue < MOISTURE_VALUE_THRESHOLD)
    {
       PORTB |= (1 << WATER_PUMP_PIN1);
       _delay_ms(5000);
       PORTB &= ~(1 << WATER_PUMP_PIN1);
    }

    // "sleep" for c. 64 seconds and repeatedly disable BOD
    for(int ii = 0; ii < 8; ii++)
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
      __asm__ __volatile__("sleep");
    }
  }
}


// Watchdog interrupt
ISR(WDT_vect)
{
    // do nothing
}
