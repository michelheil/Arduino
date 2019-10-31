/*
 * IoT_ESP8266.c
 *
 * circuitdigest.com/microcontroller-projects/esp8266-nodemcu-with-atmega16-avr-microcontroller-to-send-an-email
 *
 * Created: 28.09.2019 15:23:18
 * Author : Michael
 */ 

#include "globalDefines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "myUSART.h"


#define USART_MAX_IN_STRLEN 10

volatile uint8_t usartStrCompleteFlag = 0;     // 1 .. String komplett empfangen
volatile uint8_t usartStrCount = 0;
volatile unsigned char usartStr[USART_MAX_IN_STRLEN + 1] = "";

uint8_t compareStr[USART_MAX_IN_STRLEN + 1] = "4";

unsigned char cmpString(volatile uint8_t * string1, uint8_t * string2);

int main(void)
{
    // deactivate global Interrupts during initialization phase
    cli();
    
    // initialize USART with baud rate 9600
    USART_init();

    cbi(DDRD, PD2); // push button at Pin PD2 as input in Data Direction Register
    sbi(DDRD, PD3); // set PD3 as output LED pin

    // Enabled INT0 (PD2) interrupt
    EIMSK = (1 << INT0);
    
    // The rising edge of INT0 (PD2) generates an interrupt request.
    EICRA = (1 << ISC01) | (1 << ISC00);
        
    // activate global interrupt flag
    sei();
    
    while(1)
    {
        if(cmpString(&usartStr[0], &compareStr[0])) {
            sbi(PORTD, PD3);
            _delay_ms(1000);
            cbi(PORTD, PD3);
            usartStr[0] = 0; // "reset" received string
            }
        // reset flag and counter of the usart string 
        _delay_ms(20);
        usartStrCompleteFlag = 0;
        usartStrCount = 0;   
    }
}


// if data is received through USART RX write all incoming bytes into the usartStr
ISR(USART_RX_vect)
{
    unsigned char nextChar;

    // Daten aus dem Puffer lesen
    nextChar = UDR0;
    if(usartStrCompleteFlag == 0) {	// wenn uart_string gerade in Verwendung, neues Zeichen verwerfen

        // Daten werden erst in uart_string geschrieben, wenn nicht String-Ende/max Zeichenlänge erreicht ist/string gerade verarbeitet wird
        if( nextChar != '\n' && nextChar != '\r' && usartStrCount < USART_MAX_IN_STRLEN ) {
            usartStr[usartStrCount] = nextChar;
            usartStrCount++;
        }
        else {
            usartStr[usartStrCount] = '\0';
            usartStrCount = 0;
            usartStrCompleteFlag = 1;
        }
    }
}

// when push button is pressed send word "Send" to USART TX
ISR(INT0_vect)
{
    USART_writeString("Send");
}

// helper function to compare two strings.
unsigned char cmpString(volatile uint8_t * string1, uint8_t * string2)
{
    while (1)
    {
        if ( (*string1 == 0) && (*string2 == 0) ) return 1;
        if (*string1 != *string2) return 0;
        string1++; string2++;
    }
}