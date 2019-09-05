/*
 * ArduinoStarterKitProject3.c
 *
 * Created: 01.09.2019 06:59:08
 * Author : Michael
 */ 

// set frequency [Hz]
#define F_CPU 16000000L

#include <avr/io.h>
#include <util/delay.h>

#include "myUSART.h"
#include "myADC.h"

/* program properties */
const uint16_t baselineTemp = 20;

int main(void)
{
	USART_init();
	ADC_init();
	
	// set output pins for the three LEDs (Data Direction Register)
	DDRD = (1 << DDD2) | (1 << DDD3) | (1 << DDD4);
	
	// switch all LEDs off (ensure that the default value of 0 is set to the entire Port D)
	PORTD = 0x00;
	
	// send initial welcoming message to TX
	USART_writeString("Baseline temperature: ");
	USART_writeString(uint162char(baselineTemp));
	USART_writeString("\r\n");
	
	// switch off digital input for analog pin 0
	ADC_disableDigitalInput(0);
	
    while(1)
    {
		// read analog signal from temperature sensor (tmp36 - https://www.analog.com/media/en/technical-documentation/data-sheets/TMP35_36_37.pdf
		uint16_t tempSensorValue = ADC_readAnalogPin(0); // result should have values between 0 and 1023 (i.e. 10 bits)
		USART_writeString("Sensor Value: ");
		USART_writeString(uint162char(tempSensorValue));
		
		// calculate ADC-value into volt (see data sheet of temp sensor)
		float voltage = 5.0 * (tempSensorValue/1024.0);
		USART_writeString(", Volts: ");
		USART_writeString(float2str(1000.0F*voltage));
 		
		// calculate voltage into temperature in degrees
		float temperature = 100.0F * (voltage - 0.5F); // see data sheet of temp sensor
		USART_writeString("mV, degrees C: ");
		USART_writeString(float2str(temperature));
		
		// switch on LEDs based on the temperature
		if(temperature >= baselineTemp && temperature < baselineTemp+3) {
			PORTD = 0x00 | (1 << PD2);
		} else if (temperature >= baselineTemp+3 && temperature < baselineTemp+5) {
			PORTD = 0x00 | (1 << PD2) | (1 << PD3);
		} else if (temperature >= baselineTemp+5) {
			PORTD = 0x00 | (1 << PD2) | (1 << PD3) | (1 << PD4);
		} else {
			PORTD = 0x00;
		}
		
		USART_writeString("\r\n");
		_delay_ms(5000);

    }
}
