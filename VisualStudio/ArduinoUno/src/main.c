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
 * SCL -> SCL (AMG8833 + DS3231)
 * SDA -> SDA (AMG8833 + DS3231)
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
#include "myHelperFunctions.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "myLCD.h"
#include "myUSART.h"
#include "myAMG8833.h"
#include "myDS3231.h"
#include "myADC.h"


// define string that activate particular actions
char compareReadStr[] = "read";
char compareClearStr[] = "clear";
char compareTimeStr[] = "time";

// define Interrupt level for AMG8833
// Interrupt flag  -This flag indicates whether Interrupt is generated  or not when INT control register is activated.
// High (VDD) : Interrupt is not generated
// Low (0V)   : Interrupt is generated
//
// Interrupt value
// 1 LSB has 12 bit resolution (sign + 11 bit) which is equivalent to 0.25 Celsius and it is indicated as two's complement form.
#define AMG8833_INT_UPPER_LEVEL_LOW  0b01111000 // 120 => 30 degree Celcius
#define AMG8833_INT_UPPER_LEVEL_HIGH 0b00000000 // positive sign

// define delimiter 
char gridDelimiter[] = ";";

// define global variables to collect input string through Interrupt Service Routine (ISR)
volatile uint8_t usartStrCompleteFlag = 0;
volatile uint8_t usartStrCount = 0;
volatile char usartStr[USART_MAX_INPUT_STRING_LENGTH + 1] = "";
volatile uint8_t thermInterruptFlag = 0;
volatile uint8_t pushButtonInterruptFlag = 0;
// volatile uint8_t measureCounter = 1;

#define USART_MAX_GRID_STRING_LENGTH (((5+1)*8) + 1) // 5 digits + 1 delimiter per pixel for all 64 pixels including a trailing '\0'

int main(void)
{
    // deactivate global Interrupts during initialization phase
    cli();
    
    float amgTherm;
    float amgGrid[AMG8833_GRID_PIXELS_X][AMG8833_GRID_PIXELS_Y];
    float currentGridValue;
    float minGridValue;
    float maxGridValue;
        
    // Initialize LCD display, TWI ports and AMG8833 device
    LCD_init(); // includes clear display
    USART_init(); // init USART with baud rate of 9600; includes writing of a newLine
    ADC_init();
    AMG8833_init(AMG8833_PCTL_NORMAL_MODE, AMG8833_RST_INITIAL_RESET, AMG8833_FPSC_10FPS, AMG8833_INTC_INTEN_REACTIVE);
        
    // activate moving average
    AMG8833_setRegisterByte(AMG8833_AVE, AMG8833_AVE_SWITCH_ON);
    
    // activate Interrupt with upper limit
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
        // when push button is pressed send word "Send" to USART TX
        if(pushButtonInterruptFlag == 1) { 
            // send "Send" to ESP8266 over TX
            USART_writeStringLn("Send");
            
            // experimental zone
            // Moisture Sensor
            uint16_t moistureValue = ADC_readAnalogPin(0); // read analog input pin A0 on Arduino
            LCD_clearDisplay();
            LCD_sendDataString("Moisture: ");
            LCD_sendDataUint16(moistureValue);

            pushButtonInterruptFlag = 0; // reset interrupt flag
        }            
        
        // when grid temperature exceeds upper interrupt level read grid Values and send them as a string to Tx
        if(thermInterruptFlag == 1) { 

            sbi(PORTD, PD4); // indicate interrupt action

            AMG8833_readGrid(&amgGrid[0][0]); // read AMG8833 grid values

            minGridValue = 100; // reset maxValue before each calculation
            maxGridValue = 0; // reset maxValue before each calculation
            
            // iterate through all pixels and (a) calculate maximum value and (b) concatenate values to string
            for(int row = 0; row < AMG8833_GRID_PIXELS_X; row++) {
                char buff[USART_MAX_GRID_STRING_LENGTH] = ""; //measureCounter:
                for(int col = 0; col < AMG8833_GRID_PIXELS_Y; col++) {
                    currentGridValue = amgGrid[row][col];
                    strcat(&buff[0], float2str(currentGridValue));
                    if( !(col == (AMG8833_GRID_PIXELS_Y - 1)) ) strcat(&buff[0], ";");
                    if(currentGridValue < minGridValue) minGridValue = currentGridValue;
                    if(currentGridValue > maxGridValue) maxGridValue = currentGridValue;
                }
                USART_writeStringLn(&buff[0]);
                _delay_ms(200); // delay RX as ESP cannot read data fast enough (buffer is ~64 bytes)
                buff[0] = 0; // reset buffer for grid string output
            }
            
            LCD_clearDisplay();
            LCD_setCursorHome();
            LCD_sendDataString("Min:");
            LCD_sendDataFloat(minGridValue);
            LCD_setCursorTo(0,2);
            LCD_sendDataString("Max:");
            LCD_sendDataFloat(maxGridValue);

            cbi(PORTD, PD4); // indicate end of interrupt action
            
            thermInterruptFlag = 0; // reset interrupt flag   
        }
             
        // when a string from Rx is received start comparison of the received string with pre-defined commands 
        if(usartStrCompleteFlag == 1) { 
            if(cmpString(&usartStr[0], &compareReadStr[0])) {
                LCD_clearDisplay();
                LCD_setCursorHome();
                LCD_sendDataString("Temp:");
                amgTherm = AMG8833_readThermistor(); // read out Thermistor value and print it on display
                LCD_sendDataFloat(amgTherm);
                LCD_sendDataString(" C");
            }
            
            if(cmpString(&usartStr[0], &compareClearStr[0])) {
                LCD_clearDisplay();
            }

            if(cmpString(&usartStr[0], &compareTimeStr[0])) {
                LCD_clearDisplay();
                
                // write Date
                LCD_setCursorHome();
                char dmy[11];
                DS3231_getDMYString(&dmy[0]);
                LCD_sendDataString("Date: ");
                LCD_sendDataString(&dmy[0]);
                
                // write Time
                LCD_setCursorTo(0,2);
                char time[9];
                DS3231_getTimeString(&time[0]);
                LCD_sendDataString("Time: ");
                LCD_sendDataString(&time[0]);
            }

            usartStr[0] = 0; // reset received string
            usartStrCompleteFlag = 0; // reset Rx flag of the usartString 
            usartStrCount = 0; // reset global counter of the usartString
        } 
    }
}


// linked to push button
ISR(INT0_vect)
{
    pushButtonInterruptFlag = 1;
}

// linked to AMG8833 int pin
ISR(INT1_vect)
{
    thermInterruptFlag = 1;
    // measureCounter++;
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

