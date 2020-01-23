/*
 * TestMoistureSensorOnSTK600.c
 *
 * Created: 21.01.2020 20:25:20
 * Author : Michael
 */ 

#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define MOISTURE_ANALOG_PIN1 PA0
#define LED0 PB0
#define LED1 PB1
#define LED2 PB2
#define MOISTURE_VALUE_THRESHOLD 250

int main(void)
{
  // value to store the moisture of the plant
  uint16_t moistValue = 0;
    
  // set input pins for moisture sensors
  DDRA &= ~(1 << MOISTURE_ANALOG_PIN1);

  // set led pins as output in Data Direction Register
  DDRB |= (1 << LED2) | (1 << LED1) | (1 << LED0);

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
  
  // Important remark on "Capacitive Soil Moisture Sensor v1.2":
  // Dry: 600
  // Water: 360
  // Threshold to activate pump: moistureValue > 500
/* ADC INIT - End */

  // DIDR0 - Digital Input Disable Register 0
  // When a bit is written logic one, the digital input buffer on the corresponding ADC pin is disabled.
  DIDR0 |= (1 << ADC0D);

  while(1)
  {
  /* Measure Input PA0 - Start */
    // Select input channel
    // if bits MUX[5:0] in register ADMUX are set to zero then PA0 is used
    
    // Start Conversion
    ADCSRA |= (1 << ADSC);
    
    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC)) {}
    
    // store ADC output
    moistValue = ADC;
  /* Measure Input PA0 - End */

    if (moistValue < 350) {
      PORTB |= (1 << LED0);
    } else if (moistValue < 365) {
      PORTB |= (1 << LED1);
    } else if (moistValue < 370) {
      PORTB |= (1 << LED1) | (1 << LED0);
    } else if (moistValue < 375) {
      PORTB |= (1 << LED2);
    } else if (moistValue < 377) {
      PORTB |= (1 << LED2) | (1 << LED0);
    } else if (moistValue < 380) {
      PORTB |= (1 << LED2) | (1 << LED1);
    } else {
      PORTB |= (1 << LED2) | (1 << LED1) | (1 << LED0);
    }

    _delay_ms(3000);
    PORTB |= (1 << LED2) | (1 << LED1) | (1 << LED0);
    _delay_ms(100);
    PORTB = 0;
    _delay_ms(100);
  }
}  
  
