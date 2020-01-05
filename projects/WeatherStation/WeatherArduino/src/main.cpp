/**
 * @file main.c
 * @author Michael Heil
 * @brief Weather Station
 * @version 0.1 - Arduino
 * @date 2020-01-05
 * 
 * @copyright Copyright (c) 2020
 * 
 * @section Features
 * Sending information from BME680 to MQTT.
 * 
 * @section Open Points
 * @li Power saving modes of Arduino
 * @li Battery power supply for Arduino
 * @li using ATTiny instead of Arduino
 * @li Verwende "Verteilerbox" und nicht Eier-Pappe
 * 
 * @section Obeservations
 * @li .
 * @li .
 * @li .
 */

#include "myGlobalDefines.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "myUSART.h"
#include "myBME680.h"


// define string that activate particular actions
char compareTempStr[] = "temp";
BME680 myBME680; /**< Create instance of the BME680 class */

// define global variables to collect input string through Interrupt Service Routine (ISR)
volatile uint8_t usartStrCompleteFlag = 0;
volatile uint8_t usartStrCount = 0;
volatile char usartStr[USART_MAX_INPUT_STRING_LENGTH + 1] = "";

int main(void)
{
    // deactivate global Interrupts during initialization phase
    cli();
    
    // Initialize LCD display, TWI ports and AMG8833 device
    USART_init(); // init USART with baud rate of 9600; includes writing of a newLine

    myBME680.init();
    //USART_writeStringLn("Starting I2CDemo example program for BME680");
    //USART_writeStringLn("- Initializing BME680 sensor");
    //USART_writeStringLn("- Setting 16x oversampling for all sensors");    
    myBME680.setOversampling(TemperatureSensor,Oversample16); // Use enumerated type values
    myBME680.setOversampling(HumiditySensor,   Oversample16);
    myBME680.setOversampling(PressureSensor,   Oversample16);
    //USART_writeStringLn("- Setting IIR filter to a value of 4 samples");
    myBME680.setIIRFilter(IIR4);
    //USART_writeStringLn("- Setting gas measurement to 320\xC2\xB0 for 150ms");
    myBME680.setGas(320,150); // 320°c for 150 milliseconds

    // activate global interrupt flag
    sei();

    while(1)
    {
        // RX Commands 
        if(usartStrCompleteFlag == 1)
        { 
            // Read temperature
            if(cmpString(&usartStr[0], &compareTempStr[0]))
            {
                static int32_t temperature, humidity, pressure, gas; // variables to store readings

                myBME680.getSensorData(temperature, humidity, pressure, gas, true); // "true" ensures that we wait until new readings are available
                float tempDisplay = temperature/100.0f;

                // add key word to value to send to MQTT
                char keyTemperature[8] = "TP";
                strcat(&keyTemperature[0], float2str(tempDisplay)); // combine key word and the value
                USART_writeStringLn(&keyTemperature[0]);
            }

            // reset all RX strings and flags
            usartStr[0] = 0; // reset received string
            usartStrCompleteFlag = 0; // reset Rx flag of the usartString 
            usartStrCount = 0; // reset global counter of the usartString
        } 
    }
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
