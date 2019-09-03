/*
 * ArduinoStarterKitProject3.c
 *
 * Created: 01.09.2019 06:59:08
 * Author : Michael
 */ 

/*
Offen:
- Funktionen in .h / .c Dateien auslagern (Webreader Zeitschrift)
- int und float nach USART senden (https://www.mikrocontroller.net/articles/FAQ#Aktivieren_der_Floating_Point_Version_von_sprintf_beim_WinAVR_mit_AVR-Studio)
*/

#define F_CPU 16000000L
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>

// set baud rate
#define UART_BAUDRATE	9600
// calculate configuration parameter
#define UART_SETTING	((F_CPU/16UL/UART_BAUDRATE)-1) // (check data sheet for calculation)

/* declaration of functions */
// USART
void USART_init(void);
void USART_sendChar(unsigned char data);
unsigned char USART_receive(void);
void USART_writeString(char* StringPtr);
// USART helper functions
char* float2str(float floatValue);
char* uint162char(uint16_t uint16Value);
//ADC
void ADC_init(void);
uint16_t ADC_readAnalogPin(uint8_t channel);
void ADC_startConversionAndWait();

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


void USART_init(void)
{
	// set baud rate
	// the USART baud rate register consist of 12 bits
	// 4 bits in "H" for high frequencies and
	// 8 bits in "L" for low frequencies
	UBRR0H = (uint8_t) (UART_SETTING >> 8); // move value 8 bits to right as these other 8 bits are stored in "L"
	UBRR0L = (uint8_t) (UART_SETTING);

	// enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// set frame format: asynchronous USART, parity mode disabled, 1stop bit, 8-bit data
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

// sends one char
void USART_sendChar(unsigned char data)
{
	// wait for empty transmit buffer
	// UCSR0A is the name of the entire port (8 bits)
	// UDRE0 - USART Data Register Empty
	while(!(UCSR0A & (1 << UDRE0))); // means: while buffer not empty do nothing and wait
	
	// put data into buffer, sends the data
	UDR0 = data;
}


// receives one char (8-bit)
unsigned char USART_receive(void)
{
	// wait for data to be received
	while ( !(UCSR0A & (1 << RXC0)));
	
	// get and return received data from buffer
	return UDR0;
}

// write an entire string (collection of characters)
void USART_writeString(char* StringPtr)
{
	// taking advantage that in C every string is terminated with a null character
	// check if there is still more chars to send
	while(*StringPtr != 0x00) {
		USART_sendChar(*StringPtr);
		StringPtr++;
	}
}

char* float2str(float floatValue)
{
	static char retnum[20];       // Enough for 20 digits
	sprintf(retnum,"%d.%02d", (int)floatValue , (int) (100*((floatValue)-(int)floatValue)));
	return retnum;
}

char* uint162char(uint16_t uint16Value)
{
    static char buffer[2];
	itoa(uint16Value, buffer, 10);
	return buffer;
}


// initialize the ADC
void ADC_init(void)
{
	// REFS0 defines the reference voltage of 5V
	// MUX3..0 within port ADMUX defines the input pin.
	ADMUX = (1 << REFS0);
	
	// ADEN set to 1 enables the ADC converter
	// ADIE set to 1 enables the ADC interrupt
	// ADPS2..0: These bits determine the division factor between the system clock frequency and the input clock to the ADC. All set to 1 leads to division factor of 128
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	// turn off the digital input register for analog pin 0. 
	// this prevents from accidentally reading and processing the digital input signal from the analog pin.
	DIDR0 = (1 << ADC0D);
	
	// after activation of the ADC a "Dummy-Readout" is recommended
  	// ADC is initially being read, otherwise the result of the upcoming conversion will not be taken
	ADC_startConversionAndWait();
	(void) ADC;
}

/* ADC measurement for a single channel */
uint16_t ADC_readAnalogPin(uint8_t channel)
{
	// select pin without changing other bits within ADMUX.
	// pin selection for ATMega328P is only the bits 3..0
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
	
	ADC_startConversionAndWait();
	
	// read ADC and return value
	return ADC;
}



// Start ADC conversion and wait until the conversion is completed
void ADC_startConversionAndWait()
{
	// Start Conversion
	ADCSRA |= (1 << ADSC);
	
	// wait until conversion is completed
	while (ADCSRA & (1<<ADSC) ) {}
	
}
