/*
 * myTWI.c
 *
 * Created: 09.10.2019 18:43:33
 *  Author: Michael
 */ 
#include "myTWI.h"
#include "myUSART.h"
#include "myLCD.h"

#include <avr/io.h>
//#include <stdio.h>
#include <util/twi.h>
//#include <util/delay.h>
#include <string.h>


// enabled TWI, set frequency for SCL and activate internal pull-up resistors on SDA and SCL
void TWI_init(void)
{
    // set SCL to 100kHz
    TWSR |= TWI_PRESCALER_VALUE; // using "|=" instead of only "=" because status bits are initially set to 1
    TWBR = TWI_BIT_RATE;
    
    // The TWEN bit enables TWI operation and activates the TWI interface. When TWEN is written to one, the TWI
    // takes control over the I/O pins connected to the SCL and SDA pins, enabling the slew-rate limiters and
    // spike filters. If this bit is written to zero, the TWI is switched off and all TWI transmissions are
    // terminated, regardless of any ongoing operation.
    TWCR = (1 << TWEN);
    
    // activate internal pull-up resistors on SDA (PC4) and SCL (PC5)
    //DDRC = (1 << DDC5) | (1 << DDC4);
    //PORTC = (1 << PORTC5) | (1 << PORTC4);
}


// wrapper function for handing over buffer array to readAMG833Bytes
uint16_t TWI_readBytesFromAddressRaw(uint8_t devAddress, uint8_t regAddress, int numberBytes)
{
    // initialize buffer for the two bytes of Thermistor
    uint8_t rawData[numberBytes];
    memset(rawData, 0, numberBytes*sizeof(uint8_t));

    USART_Headline("Read Thermistor");

    // read two bytes from Thermistor
    TWI_readAMG8833Bytes(devAddress, regAddress, numberBytes, &rawData[0]);
    USART_writeStringLn("...Thermistor reading done!");

    // combine two bytes into uint16_t
    uint16_t data =  ((uint16_t) rawData[1] << 8) | ((uint16_t) rawData[0]);
    
    // return Thermistor bytes as one 16-bit variable
    return TWI_signedMag12ToFloat(data);
}


// wrapper function for handing over buffer array to readAMG833Bytes
// github.com/jodalyst/AMG8833/blob/master/src/AMG8833.cpp// usage: uint16_t results[n];//        float gridValue = TWI_int12ToFloat(Raw) * CONVERSION;uint16_t * TWI_readPairBytesFromAddressRaw(uint8_t devAddress, uint8_t regAddress, int numberPix)
{
    int numberBytes = numberPix * 2;
    
    static uint16_t tempValues[64]; // buffer for return value
    memset(tempValues, 0, numberPix*sizeof(uint16_t));
    uint8_t rawGridData[numberBytes]; // buffer for raw input from device
    memset(rawGridData, 0, numberBytes*sizeof(uint8_t));

    // read Grid Bytes starting with lower bit from first Pixel
    TWI_readAMG8833Bytes(devAddress, regAddress, numberBytes, &rawGridData[0]);
    
    for(uint16_t ii = 0; ii < numberPix; ii++) {
        // combine two bytes for each Pixel
        tempValues[ii] = ((uint16_t) rawGridData[2*ii + 1] << 8) | ((uint16_t) rawGridData[2*ii]);
    }
    
    return tempValues;
}


// Use I2C (Two-Wire Protocol) to get data from AMG8833
int TWI_readAMG8833Bytes(uint8_t sla, uint8_t reg, int len, uint8_t * dest)
{
    uint8_t slaw = ((sla << 1) | TW_WRITE);
    uint8_t slar = ((sla << 1) | TW_READ);
    
    TWI_startTransmission();
    TWI_writeSlaW(slaw);
    TWI_writeRegisterAddress(reg);
    TWI_repeatStartTransmission();
    TWI_writeSlaR(slar);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // receive Data
    uint8_t twcr, twst = 0;
    int bytesReceived = 0;

    USART_writeString("Read Data Bytes");
    // TWEA - TWI Enable Acknowledge Bit (setting this bit will send back ACK as confirmation of receiving a byte)
    for (twcr = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); len > 0; len--)
    {
        USART_writeString(".");
        // After the last byte has been received, the MR should inform the ST by sending a NACK after the last received data byte.
        if (len == 1) { // if len == 1 then this is the last byte
            twcr = (1 << TWINT) | (1 << TWEN); /* send NOT ACK this time */
        }
        
        // clear int to start transmission of either ACK or NACK after the last byte
        TWCR = twcr;
        
        // wait for transmission
        while ((TWCR & (1 << TWINT)) == 0);
        
        switch (twst = (TWSR & 0xF8)) // masked Status Register
        {
            case TW_MR_DATA_NACK:
            len = 0; // force end of the loop
            // FALLTHROUGH: no "break;" here means that the next case will be executed
            case TW_MR_DATA_ACK:
            *dest++ = TWDR;
            bytesReceived++;
            if(twst == TW_MR_DATA_NACK) {USART_newLine();TWI_stopTransmission();}
            break;
            default:
            LCD_setCursorTo(0, 2);
            LCD_sendDataString("ERR:6");
            TWI_stopTransmission();
        }
    }

    return bytesReceived;
}


void TWI_startTransmission(void)
{
    USART_writeString("Start");
    
    // Start Condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    USART_writeString("...");
    // Wait for TWINT Flag set. This indicates that the START condition has been transmitted
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bit1 and bit0, bit2 is unused anyway. If status different from START go to ERROR
    if ((TWSR & 0xF8) != TW_START) LCD_sendDataString("ERR:1"); // do not send stop after start condition
    USART_writeStringLn("success!");
}


void TWI_writeSlaW(uint8_t slaw)
{
    USART_writeString("SLA+W");

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Write SLA+W (means Slave Address + Write-Flag) into TWI Data Register => entering Master Transmitter Mode
    TWDR = slaw;

    // set TWINT bit to clear TWINT flag and transmit the content of TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait as long as TWINT flag is set.
    USART_writeString("...");
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_SLA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK) LCD_sendDataString("ERR:2");
    
    USART_writeStringLn("success!");
}


void TWI_writeByte(uint8_t val)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Load local Register Address (not the I2C slave address) of device into TWDR Register
    USART_writeString("Write Byte");
    TWDR = val;

    // Clear TWINT bit in TWCR to start transmission of data
    TWCR = (1 << TWINT) | (1 << TWEN);

    // Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
    USART_writeString("...");
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_DATA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK) LCD_sendDataString("ERR:3");
    USART_writeStringLn("success!");
}


void TWI_writeRegisterAddress(uint8_t addr)
{
    TWI_writeByte(addr);
}


void TWI_repeatStartTransmission(void)
{
    USART_writeString("Repeat Start");
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Repeated Start Condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // Wait for TWINT Flag set. This indicates that the repeated START condition has been transmitted
    USART_writeString("...");
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from REP_START go to ERROR
    if ((TWSR & 0xF8) != TW_REP_START) LCD_sendDataString("ERR:4");
    USART_writeStringLn("success!");
}


void TWI_writeSlaR(uint8_t slar)
{
    USART_writeString("SLA+R");
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Write SLA+R (means Slave Address + Read-Flag) into TWI Data Register => entering Master Receiver Mode
    TWDR = slar;

    // set TWINT bit to clear TWINT flag and transmit the content of TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait as long as TWINT flag is set.
    USART_writeString("...");
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MR_SLA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MR_SLA_ACK) LCD_sendDataString("ERR:5");
    USART_writeStringLn("success!");
}


void TWI_stopTransmission(void)
{
    USART_writeString("STOP...");
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Stop Condition
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    USART_writeStringLn("successfully transmitted data!");
}



// helper functions
// convert a 12-bit signed magnitude value to a floating point number
// According to data sheet AMG8833: 12-bit Thermistor resolution is indicated as code (sign) + absolute value
float TWI_signedMag12ToFloat(uint16_t val)
{
    //take first 11 bits as absolute val by applying the 11-bit mask 0x7FF
    uint16_t absVal = (val & 0x7FF);

    // if 12th bit (0x800) is 0 (= minus) then return negative absolute value, otherwise just return positive (absolute) value
    return (val & 0x800) ? 0 - (float) absVal : (float) absVal;
}


// convert a 12-bit integer two's complement value to a floating point number
// According to data sheet AMG8833: 12-bit Pixel resolution is indicated as two's complement form (Zweierkomplement)
float TWI_int12ToFloat(uint16_t val)
{
    // shift to left so that sign bit of 12 bit integer number is placed on sign bit of 16 bit signed integer number
    int16_t sVal = (val << 4);

    // shift back the signed number. Call of return converts to float
    return sVal >> 4;
}
