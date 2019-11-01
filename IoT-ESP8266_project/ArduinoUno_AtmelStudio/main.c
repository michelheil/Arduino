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
#include "myLCD.h"
#include "myUSART.h"
#include "myAMG8833.h"

#define USART_MAX_IN_STRLEN 10

// define string that activate particular actions
uint8_t compareStr[USART_MAX_IN_STRLEN] = "4";
uint8_t compareClearStr[USART_MAX_IN_STRLEN] = "clear";

// define global variables to collect input string through Interrupt Service Routine (ISR)
volatile uint8_t usartStrCompleteFlag = 0;
volatile uint8_t usartStrCount = 0;
volatile unsigned char usartStr[USART_MAX_IN_STRLEN + 1] = "";

/*
 * define function to compare two Strings
 * 
 * string1: pointer to unsigned char of first string
 * string2: pointer to unsigned char of second string
 *
 * returns: Byte value that is written in the register
 *
 * Example: cmpString(&usartStr[0], &compareStr[0])
 */
unsigned char cmpString(volatile uint8_t * string1, uint8_t * string2);


int main(void)
{
    // deactivate global Interrupts during initialization phase
    cli();
    
    float amgTherm;
    //float amgGrid[AMG8833_GRID_PIXELS_X][AMG8833_GRID_PIXELS_Y];
        
    // Initialize LCD display, TWI ports and AMG8833 device
    LCD_init(); // includes clear display
    USART_init(); // init USART with baud rate of 9600; includes writing of a newLine
    AMG8833_init(AMG8833_PCTL_NORMAL_MODE, AMG8833_RST_INITIAL_RESET, AMG8833_FPSC_10FPS, AMG8833_INTC_INTEN_REACTIVE);
        
    // activate moving average
    AMG8833_setRegisterByte(AMG8833_AVE, AMG8833_AVE_SWITCH_ON);

    cbi(DDRD, PD2); // push button at Pin PD2 as input in Data Direction Register (actually not required as INT0 is activated)
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
                    LCD_setCursorHome();
                    LCD_sendDataString("Temp:");
                    // read out Thermistor value and print it on display
                    amgTherm = AMG8833_readThermistor();
                    LCD_sendDataFloat(amgTherm);
                    LCD_sendDataString(" C");
            _delay_ms(1000);
            cbi(PORTD, PD3);
            usartStr[0] = 0; // "reset" received string
        }
            
if(cmpString(&usartStr[0], &compareClearStr[0])) {
     LCD_clearDisplay();
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