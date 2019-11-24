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
char compareTempStr[] = "temp";
char compareTimeStr[] = "time";
char comparePlantsStr[] = "plants";
char compareClearStr[] = "clear";

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

    // define Input and Output pins
    cbi(DDRD, PD2); // push button at Pin PD2 as input in Data Direction Register (actually not required as INT0 is activated)
    cbi(DDRD, PD3); // set as input pin connected to INT from AMG8833 (actually not required as INT1 is activated)
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
        //////////////////////////////////////////////////////////////////////////
        // Bush Button
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
        
        //////////////////////////////////////////////////////////////////////////
        // AMG Interrupt (Temperature)
        if(thermInterruptFlag == 1) { 

            sbi(PORTD, PD4); // indicate interrupt action

            AMG8833_readGrid(&amgGrid[0][0]); // read AMG8833 grid values

            minGridValue = 100; // reset maxValue before each calculation
            maxGridValue = 0; // reset maxValue before each calculation
            
            // iterate through all pixels and (a) calculate maximum value and (b) concatenate values to string
            for(int row = 0; row < AMG8833_GRID_PIXELS_X; row++) {
                char buff[USART_MAX_GRID_STRING_LENGTH] = "";
                for(int col = 0; col < AMG8833_GRID_PIXELS_Y; col++) {
                    currentGridValue = amgGrid[row][col];
                    strcat(&buff[0], float2str(currentGridValue));
                    if( !(col == (AMG8833_GRID_PIXELS_Y - 1)) ) strcat(&buff[0], ";");
                    if(currentGridValue < minGridValue) minGridValue = currentGridValue;
                    if(currentGridValue > maxGridValue) maxGridValue = currentGridValue;
                }
                USART_writeStringLn(&buff[0]);
                _delay_us(200); // delay RX such that 8 temperature values can be sent to and processed by ESP
                buff[0] = 0; // reset buffer for grid string output
            }

            // Display Min and Max values
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
             
        //////////////////////////////////////////////////////////////////////////
        // RX Commands 
        if(usartStrCompleteFlag == 1) { 

            // Read Thermistor
            if(cmpString(&usartStr[0], &compareTempStr[0])) {
                LCD_clearDisplay();
                LCD_setCursorHome();
                LCD_sendDataString("Temp:");
                amgTherm = AMG8833_readThermistor(); // read out Thermistor value and print it on display
                LCD_sendDataFloat(amgTherm);
                LCD_sendDataString(" C");

                // add key word to value to send to MQTT
                char keyTemperature[8] = "TP";
                strcat(&keyTemperature[0], float2str(amgTherm)); // combine key word for moisture values and the value
                USART_writeStringLn(&keyTemperature[0]);
            }
            
            // Display date and time
            if(cmpString(&usartStr[0], &compareTimeStr[0])) {
                LCD_clearDisplay();
                
                // display Date
                LCD_setCursorHome();
                char dmy[11];
                DS3231_getDMYString(&dmy[0]);
                LCD_sendDataString("Date: ");
                LCD_sendDataString(&dmy[0]);
                
                // display Time
                LCD_setCursorTo(0,2);
                char time[9];
                DS3231_getTimeString(&time[0]);
                LCD_sendDataString("Time: ");
                LCD_sendDataString(&time[0]);

                // add key word to value to send to MQTT
                char keyDateTime[22] = "DT";
                strcat(&keyDateTime[0], &dmy[0]);
                strcat(&keyDateTime[0], ";");
                strcat(&keyDateTime[0], &time[0]);
                USART_writeStringLn(&keyDateTime[0]);
            }

            // Moisture of plants
            if(cmpString(&usartStr[0], &comparePlantsStr[0])){

                // 1
                char keyPlant1[6] = "P1";
                uint16_t moistureValue1 = ADC_readAnalogPin(0); // read analog input pin A0 on Arduino
                strcat(&keyPlant1[0], uint162str(moistureValue1)); // combine key word for moisture values and the value
                USART_writeStringLn(&keyPlant1[0]);

                LCD_clearDisplay();
                LCD_sendDataString("Moisture1: ");
                LCD_sendDataUint16(moistureValue1);

                // 2
                char keyPlant2[6] = "P2";
                uint16_t moistureValue2 = ADC_readAnalogPin(1); // read analog input pin A0 on Arduino
                strcat(&keyPlant2[0], uint162str(moistureValue2)); // combine key word for moisture values and the value
                USART_writeStringLn(&keyPlant2[0]);

                LCD_setCursorTo(0, 2);
                LCD_sendDataString("Moisture2: ");
                LCD_sendDataUint16(moistureValue2);

                // 3
                char keyPlant3[6] = "P3";
                uint16_t moistureValue3 = ADC_readAnalogPin(0); // read analog input pin A0 on Arduino
                strcat(&keyPlant3[0], uint162str(moistureValue3)); // combine key word for moisture values and the value
                USART_writeStringLn(&keyPlant3[0]);


                // 4
                char keyPlant4[6] = "P4";
                uint16_t moistureValue4 = ADC_readAnalogPin(0); // read analog input pin A0 on Arduino
                strcat(&keyPlant4[0], uint162str(moistureValue4)); // combine key word for moisture values and the value
                USART_writeStringLn(&keyPlant4[0]);
            }

            // Clear LCD
            if(cmpString(&usartStr[0], &compareClearStr[0])) {
                LCD_clearDisplay();
            }

            // reset all RX strings and flags
            usartStr[0] = 0; // reset received string
            usartStrCompleteFlag = 0; // reset Rx flag of the usartString 
            usartStrCount = 0; // reset global counter of the usartString
        } 
    }
}


// Push button
ISR(INT0_vect)
{
    pushButtonInterruptFlag = 1;
}

// AMG Interrupt (Temperature)
ISR(INT1_vect)
{
    thermInterruptFlag = 1;
}

// RX
ISR(USART_RX_vect)
{
    unsigned char nextChar;

    // read incoming byte out of UART data register 0
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

