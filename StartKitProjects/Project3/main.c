/*
 * ArduinoStarterKitProject3.c
 *
 * Created: 01.09.2019 06:59:08
 * Author : Michael
 */ 

/*
Offen:
- Funktionen in .h / .c Dateien auslagern (Webreader Zeitschrift)
- einlesen vom analogen Signal
- int und float nach USART senden
*/

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
// #include <c:\users\michael\Documents\Atmel Studio\7.0\SerialMonitor\SerialMonitor\serial.h>

// set baud rate
#define UART_BAUDRATE	9600
// calculate configuration parameter
#define UART_SETTING	((F_CPU/16UL/UART_BAUDRATE)-1) // (check data sheet for calculation)

// declaration of functions
void USART_init(void);
void USART_sendChar(unsigned char data);
unsigned char USART_receive(void);
void USART_writeString(char* StringPtr);
void ADC_init();
void ADC_startConversion();

const float baselineTemp = 20.0;
char baselineTempToChar[2];
char tempSensorValueToChar[4];
char voltageToChar[4];
char temperatureToChar[2];

int main(void)
{
    USART_init();
	ADC_init();
	
	// set output pins for the three LEDs
	DDRD = (1 << DDD2) | (1 << DDD3) | (1 << DDD4);
	
	// switch all LEDs off (ensure that the default value of 0 is set to the entire Port D)
	PORTD = 0x00;
	
	// send initial welcoming message to TX
	USART_writeString("Baseline temperature: ");
	itoa(baselineTemp, baselineTempToChar, 10);
	USART_writeString(baselineTempToChar);
	USART_writeString("\r\n");
	
    while(1)
    {
		// read analog signal from temperature sensor (tmp36 - https://www.analog.com/media/en/technical-documentation/data-sheets/TMP35_36_37.pdf
		uint16_t tempSensorValue = ADC; // should have values between 0 and 1023 (i.e. 10 bits)
		itoa(tempSensorValue, tempSensorValueToChar, 10);
		USART_writeString("Sensor Value: ");
		USART_writeString(tempSensorValueToChar);
		
		// calculate ADC-value into volt
		float voltage = 5.0 * (tempSensorValue/1024.0);
		itoa(voltage, voltageToChar, 10);
		USART_writeString(", Volts: ");
		USART_writeString(voltageToChar);
		
		// calculate voltage into temperature in degrees
		float temperature = 100 * (voltage - 0.5);
		itoa(temperature, temperatureToChar, 10);
		USART_writeString(", degrees C: ");
		USART_writeString(temperatureToChar);
		
		
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

void ADC_init()
{
	// REFS0 defines the reference voltage of 5V
	// MUX3..0 within port ADMUX defines the input pin. As I use ADC0, all of them will stay with their default value of 0.
	ADMUX = (1 << REFS0);
	
	// ADEN set to 1 enables the ADC converter
	// ADIE set to 1 enables the ADC interrupt
	// ADPS2..0: These bits determine the division factor between the system clock frequency and the input clock to the ADC. All set to 1 leads to division factor of 128
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	// turn off the digital input register for analog pin 0. 
	DIDR0 = (1 << ADC0D);
	
	ADC_startConversion();
}

void ADC_startConversion()
{
	ADCSRA |= (1 << ADSC);
}
