/*
 * IoT_ESP8266.c
 *
 * circuitdigest.com/microcontroller-projects/esp8266-nodemcu-with-atmega16-avr-microcontroller-to-send-an-email
 *
 * PD0/RX -> TX (ESP)
 * PD1/TX -> RX (ESP)
 * PD2 -> push button
 * PD3 -> INT (AMG8833)
 * PD4 -> LED
 *
 * SCL -> SCL (AMG8833)
 * SDA -> SDA (AMG8833)
 *
 * PB0 -> RS (LCD)
 * PB1 -> E (LCD)
 * PB2 -> DB4 (LCD)
 * PB3 -> DB5 (LCD)
 * PB4 -> DB6 (LCD)
 * PB5 -> DB7 (LCD)
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

// define string that activate particular actions
char compareStr[] = "read";
char compareClearStr[] = "clear";

// define Interrupt level for AMG8833
// Interrupt flag  -This flag indicates whether Interrupt is generated  or not when INT control register is activated.
// High (VDD) : Interrupt is not generated
// Low (0V)   : Interrupt is generated
//
// Interrupt value
// 1 LSB has 12 bit resolution (11 bit + sign) which is equivalent to 0.25? and it is indicated as two's complement form.
#define AMG8833_INT_UPPER_LEVEL_LOW  0b01100100 // 100 => 25 Celcius
#define AMG8833_INT_UPPER_LEVEL_HIGH 0b00000000 // positive sign

// define global variables to collect input string through Interrupt Service Routine (ISR)
volatile uint8_t usartStrCompleteFlag = 0;
volatile uint8_t usartStrCount = 0;
volatile char usartStr[USART_MAX_INPUT_STRING_LENGTH + 1] = "";

/*
 * define function to compare two Strings
 * 
 * string1: pointer to unsigned char of first string
 * string2: pointer to unsigned char of second string
 *
 * returns: 1 for string matching, 0 else
 *
 * Example: cmpString(&usartStr[0], &compareStr[0])
 */
uint8_t cmpString(volatile char * string1, char * string2);


int main(void)
{
    // deactivate global Interrupts during initialization phase
    cli();
    
    float amgTherm;
    float amgGrid[AMG8833_GRID_PIXELS_X][AMG8833_GRID_PIXELS_Y];
        
    // Initialize LCD display, TWI ports and AMG8833 device
    LCD_init(); // includes clear display
    USART_init(); // init USART with baud rate of 9600; includes writing of a newLine
    AMG8833_init(AMG8833_PCTL_NORMAL_MODE, AMG8833_RST_INITIAL_RESET, AMG8833_FPSC_10FPS, AMG8833_INTC_INTEN_REACTIVE);
        
    // activate moving average
    AMG8833_setRegisterByte(AMG8833_AVE, AMG8833_AVE_SWITCH_ON);
    
    // activate Interrupt of 25 Celsius
    AMG8833_setRegisterByte(AMG8833_INTC, AMG8833_INTC_INTMOD_ABS + AMG8833_INTC_INTEN_ACTIVE);
    AMG8833_setRegisterByte(AMG8833_INTHL, AMG8833_INT_UPPER_LEVEL_LOW);
    AMG8833_setRegisterByte(AMG8833_INTHH, AMG8833_INT_UPPER_LEVEL_HIGH);

    cbi(DDRD, PD2); // push button at Pin PD2 as input in Data Direction Register (actually not required as INT0 is activated)
    cbi(DDRD, PD3); // set as input pin (connected to INT from AMG8833)
    sbi(DDRD, PD4); // set as output LED pin
    

    // Enabled INT0 (PD2) and INT1 (PD3) interrupts
    EIMSK = (1 << INT1) | (1 << INT0);
    
    // The rising edge of INT0 (PD2) generates an interrupt request.
    // The falling edge of INT1 generates an interrupt 
    EICRA = (1 << ISC11) | (0 << ISC10) | (1 << ISC01) | (1 << ISC00);
        
    // activate global interrupt flag
    sei();

    while(1)
    {
        if(usartStrCompleteFlag == 1) { // only start comparison when the received string is completely transmitted
            if(cmpString(&usartStr[0], &compareStr[0])) {
                sbi(PORTD, PD4);
                LCD_setCursorHome();
                LCD_sendDataString("Temp:");
                // read out Thermistor value and print it on display
                amgTherm = AMG8833_readThermistor();
                LCD_sendDataFloat(amgTherm);
                LCD_sendDataString(" C");
  
                AMG8833_readGrid(&amgGrid[0][0]);
                LCD_setCursorTo(0,2);
                LCD_sendDataString("Pixel_88");
                LCD_sendDataFloat(amgGrid[7][7]);
      
                _delay_ms(500);
                cbi(PORTD, PD4);
                usartStr[0] = 0; // "reset" received string
            }
            
            if(cmpString(&usartStr[0], &compareClearStr[0])) {
                LCD_clearDisplay();
            }     
            
            // reset flag and counter of the usartString 
            _delay_ms(20);
            usartStrCompleteFlag = 0;
            usartStrCount = 0; 
        }  
    }
}


// if data is received through USART RX write all incoming bytes into the usartStr
ISR(USART_RX_vect)
{
    unsigned char nextChar;

    // read incoming byte out of data register 0
    nextChar = UDR0;
    if(usartStrCompleteFlag == 0) {	// if the usartStr contains a complete string that the nextChar will be ignored
        if( nextChar >= 0x20 && usartStrCount < USART_MAX_INPUT_STRING_LENGTH ) { // condition ">= 0x20" ensures that only non-escape characters are considered
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

ISR(INT1_vect)
{
    USART_writeString("AMG interrupt");
}


// helper function to compare two strings.
unsigned char cmpString(volatile char * string1, char * string2)
{
    while (1)
    {
        if ( (*string1 == 0) && (*string2 == 0) ) return 1;
        if (*string1 != *string2) return 0;
        string1++; string2++;
    }
}