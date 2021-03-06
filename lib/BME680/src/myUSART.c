/*
 * myUSART.c
 *
 * Created: 03.09.2019 22:53:01
 *  Author: Michael
 */ 
#include "myUSART.h"
#include "globalDefines.h" // required for sbi usage
#include "myHelperFunctions.h" // required for float2str usage

// set baud rate
#define USART_BAUDRATE	250000//125000 //9600
// calculate configuration parameter; check data sheet for calculation
#define USART_SETTING	((F_CPU/16UL/USART_BAUDRATE)-1) 

void USART_init(void)
{
    uint8_t trash; // define variable for initial reading of Data Register (UDR0)
    
	// set baud rate
	// the USART baud rate register consist of 12 bits
	// 4 bits in "H" for high frequencies and
	// 8 bits in "L" for low frequencies
	UBRR0H = (uint8_t) (USART_SETTING >> 8); // move value 8 bits to right as the other 8 bits are stored in "L"
	UBRR0L = (uint8_t) (USART_SETTING);

	// enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    
    // enabled RX Complete Interrupt
    sbi(UCSR0B, RXCIE0);

	// set frame format: asynchronous USART, parity mode disabled, 1stop bit, 8-bit data
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
    
    trash = UDR0;
    (void) trash;
    USART_newLine();
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


// write an entire string (collection of characters)
void USART_writeString(char * stringPtr)
{
	// taking advantage that in C every string is terminated with a null character
	// check if there is still more chars to send
	while(*stringPtr != 0x00) {
		USART_sendChar(*stringPtr);
		stringPtr++;
	}
}

// write an entire String with a line feed at the end
void USART_writeStringLn(char * stringPtr)
{
    USART_writeString(stringPtr);
    USART_newLine();
}

// write value of a float variable
void USART_writeFloat(float val)
{
    USART_writeString(float2str(val));
}


// receives one char (8-bit)
unsigned char USART_receiveChar(void)
{
    // wait for data to be received
    while ( !(UCSR0A & (1 << RXC0)));
    
    // get and return received data from buffer
    return UDR0;
}


unsigned char USART_getString(volatile char * stringPtr)
{
    // create output value, counting the characters
    unsigned char counter = 0;
    
    while(1)
    {
        // store received char
        *stringPtr = USART_receiveChar();
        
        if(*stringPtr >= 0x20 && counter < USART_MAX_INPUT_STRING_LENGTH) { // exclude escape characters
            counter++;
            stringPtr++;
        } // end if
        else {
            if (*stringPtr != '\b' || counter >= USART_MAX_INPUT_STRING_LENGTH) { // ending input (for example when 'enter' has been pressed)
                counter = *stringPtr; // store last character that caused the function to finish
                *stringPtr = 0; // closes the string
                return counter; // return last character
            } // end if
            else if (*stringPtr == '\b' && counter != 0) {
                stringPtr--;
                counter--;
            } // end if
        } // end else
    } // end file
} // end function





unsigned char USART_getStringWithEcho(char * stringPtr)
{
    // create output value, counting the characters
    unsigned char counter = 0;
    
    while(1)
    {
        // store received char
        *stringPtr = USART_receiveChar();
        
        if(*stringPtr >= 0x20 && counter < USART_MAX_INPUT_STRING_LENGTH) { // exclude escape characters
            USART_sendChar(*stringPtr);
            counter++;
            stringPtr++;
        } // end if
        else {
            if (*stringPtr != '\b' || counter >= USART_MAX_INPUT_STRING_LENGTH) { // ending input (for example when 'enter' has been pressed)
                counter = *stringPtr; // store last character that caused the function to finish
                *stringPtr = 0; // closes the string
                return counter; // return last character
            } // end if
            else if (*stringPtr == '\b' && counter != 0) {
                stringPtr--;
                USART_sendChar('\b');
                USART_sendChar(' ');
                USART_sendChar('\b');
                counter--;                
            } // end if                
        } // end else
    } // end file
} // end function



// start new line
void USART_newLine()
{
    USART_writeString("\r\n");
}

// create a headline that stands out compared to regular Strings
void USART_Headline(char * stringPtr)
{
    USART_newLine();
    USART_writeString("--------------------------------------------------");
    USART_newLine();
    USART_writeString(stringPtr);
    USART_newLine();
}
