/*
 * myTWI.c
 *
 * Created: 09.10.2019 18:43:33
 *  Author: Michael
 */ 
#include "myLOG.h"
#include "myTWI.h"

#include <avr/io.h>
#include <util/twi.h>


// enabled TWI, set frequency for SCL (and activate internal pull-up resistors on SDA and SCL)
int TWI_init(void)
{
    // set SCL frequency
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
    
    return 0;
}


// write byte to register
void TWI_setRegisterByte(uint8_t sla, uint8_t reg, uint8_t val)
{
    uint8_t slaw = (sla << 1) | TW_WRITE;

    TWI_startTransmission();
    TWI_writeSlaRW(slaw);
    TWI_writeRegisterAddress(reg);
    TWI_writeByte(val);
    TWI_stopTransmission();
}

// read byte from register
uint8_t TWI_getRegisterByte(uint8_t sla, uint8_t reg)
{
    uint8_t slaw = (sla << 1) | TW_WRITE;
    uint8_t slar = (sla << 1) | TW_READ;
    uint8_t regValue;
    
    TWI_startTransmission();
    TWI_writeSlaRW(slaw);
    TWI_writeRegisterAddress(reg);
    TWI_repeatStartTransmission();
    TWI_writeSlaRW(slar);

    // clear int to start transmission of either ACK or NACK after the last byte    
    TWCR = (1 << TWINT) | (1 << TWEN); // send NOT ACK as only one byte is expected

    // wait for transmission
    while ((TWCR & (1 << TWINT)) == 0);
    
    // read data register
    regValue = TWDR;
    
    TWI_stopTransmission();
    
    return regValue;
}


// Use I2C (Two-Wire Protocol) to get data from AMG8833
int TWI_getRegisterBytes(uint8_t sla, uint8_t reg, int len, uint8_t * dest)
{
    uint8_t slaw = ((sla << 1) | TW_WRITE);
    uint8_t slar = ((sla << 1) | TW_READ);
    
    TWI_startTransmission();
    TWI_writeSlaRW(slaw);
    TWI_writeRegisterAddress(reg);
    TWI_repeatStartTransmission();
    TWI_writeSlaRW(slar);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // receive Data
    uint8_t twcr, twst = 0;
    int bytesReceived = 0;

    LOG_debug("Read Data Bytes");
    // TWEA - TWI Enable Acknowledge Bit (setting this bit will send back ACK as confirmation of receiving a byte)
    for (twcr = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); len > 0; len--)
    {
        LOG_debug(".");
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
                if(twst == TW_MR_DATA_NACK) {TWI_stopTransmission();}
                break;
            default:
                LOG_error("ERR:6");
                TWI_stopTransmission();
        }
    }

    return bytesReceived;
}


void TWI_startTransmission(void)
{
    LOG_debug("Start");
    
    // Start Condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    LOG_debug("...");
    // Wait for TWINT Flag set. This indicates that the START condition has been transmitted
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bit1 and bit0, bit2 is unused anyway. If status different from START go to ERROR
    if ((TWSR & 0xF8) != TW_START) LOG_error("ERR:1"); // do not send stop after start condition
    LOG_debug("success!");
}


void TWI_writeSlaRW(uint8_t slarw)
{
    LOG_debug("SLA+R/W");

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Write SLA+R/W (means Slave Address + Read/Write-Flag) into TWI Data Register => entering Master Transmitter/Receiver Mode
    TWDR = slarw;

    // set TWINT bit to clear TWINT flag and transmit the content of TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait as long as TWINT flag is set.
    LOG_debug("...");
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_SLA_ACK go to ERROR
    if ( ((TWSR & 0xF8) != TW_MT_SLA_ACK) && ((TWSR & 0xF8) != TW_MR_SLA_ACK) ) LOG_error("ERR:2");
    
    LOG_debug("success!");
}


void TWI_writeByte(uint8_t val)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Load local Register Address (not the I2C slave address) of device into TWDR Register
    LOG_debug("Write Byte");
    TWDR = val;

    // Clear TWINT bit in TWCR to start transmission of data
    TWCR = (1 << TWINT) | (1 << TWEN);

    // Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
    LOG_debug("...");
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_DATA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK) LOG_error("ERR:3");
    LOG_debug("success!");
}


void TWI_writeRegisterAddress(uint8_t addr)
{
    TWI_writeByte(addr);
}


void TWI_repeatStartTransmission(void)
{
    LOG_debug("Repeat Start");
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Repeated Start Condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // Wait for TWINT Flag set. This indicates that the repeated START condition has been transmitted
    LOG_debug("...");
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from REP_START go to ERROR
    if ((TWSR & 0xF8) != TW_REP_START) LOG_error("ERR:4");
    LOG_debug("success!");
}


void TWI_stopTransmission(void)
{
    LOG_debug("STOP...");
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Stop Condition
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    LOG_debug("successfully transmitted data!");
}



