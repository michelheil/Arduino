/*
 * myUSART.c
 *
 * Created: 03.09.2019 22:53:01
 *  Author: Michael
 */ 
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

// set frequency [Hz]
#ifndef F_CPU // should be defined in main function
#define F_CPU 16000000L
#endif

// set baud rate
#define UART_BAUDRATE	9600
// calculate configuration parameter
#define UART_SETTING	((F_CPU/16UL/UART_BAUDRATE)-1) // (check data sheet for calculation)

/* declaration of functions */
void USART_init(void);
void USART_sendChar(unsigned char data);
unsigned char USART_receive(void);
void USART_writeString(char* StringPtr);

// helper functions
char* float2str(float floatValue);
char* uint162char(uint16_t uint16Value);
char* uint322char(uint32_t uint32Value);

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


// www.mikrocontroller.net/articles/FAQ#Aktivieren_der_Floating_Point_Version_von_sprintf_beim_WinAVR_mit_AVR-Studio
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

char* uint322char(uint32_t uint32Value)
{
	static char buffer[4];
	itoa(uint32Value, buffer, 10);
	return buffer;
}
