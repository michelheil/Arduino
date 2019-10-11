/*
 * myAMG8833.c
 *
 * Created: 10.10.2019 19:50:07
 *  Author: Michael
 */ 
#include "myAMG8833.h"

#include <util/delay.h>
#include "myUSART.h"
#include "myTWI.h"


// entering normal mode for AMG8833, restarting and setting general registers
int AMG8833_init(uint8_t pcr, uint8_t rr, uint8_t frr, uint8_t icr)
{
    // initialize I2C for communication with AMG8833
    TWI_init();

    /////////////////////////////////////////////////
    USART_Headline("Set Operating Mode:");
    AMG8833_setRegisterByte(AMG8833_PCTL, pcr);

    /////////////////////////////////////////////////
    USART_Headline("Perform Software Reset:");
    AMG8833_setRegisterByte(AMG8833_RST, rr);
    
    /////////////////////////////////////////////////
    USART_Headline("Set Frame Rate:");
    AMG8833_setRegisterByte(AMG8833_FPSC, frr);

    /////////////////////////////////////////////////
    USART_Headline("Disable Interrupts:");
    AMG8833_setRegisterByte(AMG8833_INTC, icr);

    _delay_ms(100);

    return 0;
}


void AMG8833_setRegisterByte(uint8_t reg, uint8_t val)
{
    TWI_startTransmission();
    TWI_writeSlaRW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(reg);
    TWI_writeByte(val);
    TWI_stopTransmission();
}

uint8_t AMG8833_getRegisterByte(uint8_t reg)
{
    uint8_t regValue;
    
    TWI_startTransmission();
    TWI_writeSlaRW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(reg);
    TWI_repeatStartTransmission();
    TWI_writeSlaRW(AMG8833_SLA_R);

    // clear int to start transmission of either ACK or NACK after the last byte    
    TWCR = (1 << TWINT) | (1 << TWEN); // send NOT ACK as only one byte is expected

    // wait for transmission
    while ((TWCR & (1 << TWINT)) == 0);
    
    // read data register
    regValue = TWDR;
    
    TWI_stopTransmission();
    
    return regValue;
}

void AMG8833_readGrid(float * resultGridValues)
{
    // define an array to store the raw grid Values from AMG8833 device
    uint16_t rawGridValues[AMG8833_GRID_PIXELS];
    
    // read out the 64 pixels from the grid. Each Pixel consist of two bytes
    TWI_readPairBytesFromAddressRaw(AMG8833_SLAVE_ADDRESS, AMG8833_T01L, AMG8833_GRID_PIXELS, &rawGridValues[0]);

    // transform raw value into Celsius degree according to data sheet calculation
    for(int ii = 0; ii < AMG8833_GRID_PIXELS; ii++)
    {
        *resultGridValues++ = TWI_int12ToFloat(rawGridValues[ii]) * AMG8833_PIXEL_CONVERSION;
        
    }
}

float AMG8833_readThermistor(void)
{
    // read the thermistor value as 16-bit value
    uint16_t thermistorValueRaw = TWI_readBytesFromAddressRaw(AMG8833_SLAVE_ADDRESS, AMG8833_TTHL, AMG8833_THERMISTOR_BYTES);
    
    // return float value containing temperature in Celsius
    return TWI_signedMag12ToFloat(thermistorValueRaw) * AMG8833_THERMISTOR_CONVERSION;
}

const char * AMG8833_readStatusRegister(void)
{
    // read Status Register from AMG8833
    uint8_t statusReg = AMG8833_getRegisterByte(AMG8833_SR);
        
    // based on register content return String describing the status
    switch(statusReg)
    {
        case 0x00:
            return "All good!";

        case AMG8833_SR_THERM_OVERFLOW:
            return "Therm Overflow";

        case AMG8833_SR_TEMP_OVERFLOW:
            return "Temp Overflow";
            
        case AMG8833_SR_INT_OUTBREAK:
            return "INT Outbreak";
            
        default:
            return "SR read error";
    }
}