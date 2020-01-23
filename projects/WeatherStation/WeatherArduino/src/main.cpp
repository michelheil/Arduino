/**
 * @file main.cpp
 * @author Michael Heil
 * @brief Weather Station
 * @version 0.2 - Arduino
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
char compareHumidStr[] = "humid";
char comparePressStr[] = "press";
char compareAltStr[] = "alt";

// Create instance of the BME680 class
BME680 myBME680; 

// define global variables to collect input string through Interrupt Service Routine (ISR)
volatile uint8_t usartStrCompleteFlag = 0;
volatile uint8_t usartStrCount = 0;
volatile char usartStr[USART_MAX_INPUT_STRING_LENGTH + 1] = "";

/**
 * @brief     This converts a pressure measurement into height in meters
 * @details   The sea-level pressure is set to 1013.25hPa
 * @param pressure Pressure value in hPa
 * @return    altitude in meters as float
 */
float altitude(int32_t pressure = 977.20);


int main(void)
{
    // deactivate global Interrupts during initialization phase
    cli();
    
    // init USART with baud rate of 9600; includes writing of a newLine
    USART_init(); 

    // init BME 680
    myBME680.init();
    myBME680.setOversampling(TemperatureSensor,Oversample16);
    myBME680.setOversampling(HumiditySensor,   Oversample16);
    myBME680.setOversampling(PressureSensor,   Oversample16);
    myBME680.setIIRFilter(IIROff);
    myBME680.setGas(320,150); // 320°c for 150 milliseconds

    int32_t temperature, humidity, pressure, gas; // variables to store readings

    // activate global interrupt flag
    sei();

    while(1)
    {
        // RX Commands 
        if(usartStrCompleteFlag == 1)
        { 
            // "true" ensures that we wait until new readings are available
            myBME680.getSensorData(temperature, humidity, pressure, gas, true); 

            // Temperature
            if(cmpString(&usartStr[0], &compareTempStr[0]))
            {
                float tempDisplay = temperature/100.0f;

                // add key word to value to send to MQTT
                char keyTemperature[8] = "TP";
                strcat(&keyTemperature[0], float2str(tempDisplay)); // combine key word and the value
                USART_writeStringLn(&keyTemperature[0]);
            }

            // Humidity
            else if(cmpString(&usartStr[0], &compareHumidStr[0]))
            {
                float humidDisplay = humidity/1000.0f;

                // add key word to value to send to MQTT
                char keyHumidity[8] = "HU";
                strcat(&keyHumidity[0], float2str(humidDisplay)); // combine key word and the value
                USART_writeStringLn(&keyHumidity[0]);
            }

            // Pressure
            else if(cmpString(&usartStr[0], &comparePressStr[0]))
            {
                float pressDisplay = pressure/100.0f;

                // add key word to value to send to MQTT
                char keyPressure[10] = "PR";
                strcat(&keyPressure[0], float2str(pressDisplay)); // combine key word and the value
                USART_writeStringLn(&keyPressure[0]);
            }

            // Altitude
            else if(cmpString(&usartStr[0], &compareAltStr[0]))
            {
                float altDisplay = altitude(pressure);

                // add key word to value to send to MQTT
                char keyAltitude[10] = "AL";
                strcat(&keyAltitude[0], float2str(altDisplay)); // combine key word and the value
                USART_writeStringLn(&keyAltitude[0]);
            }

            // unknown key word (default)
            else 
            {
                USART_writeStringLn("Unknown");
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


// Calculate altidude based on pressure
float altitude(int32_t pressure) 
{
  const float seaLevel = 1013.25;

  static float altitudeResult = 44330.0 * (1.0 - pow( ((float)pressure/100.0) / seaLevel , 0.1903));
  return(altitudeResult);
}