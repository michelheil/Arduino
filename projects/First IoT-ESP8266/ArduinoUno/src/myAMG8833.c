/*
 * myAMG8833.c
 *
 * Created: 10.10.2019 19:50:07
 *  Author: Michael
 */ 
#include "myAMG8833.h"

#include "myHelperFunctions.h"

#include <util/delay.h>
#include <string.h>
#include "myLOG.h"
#include "myTWI.h"


// entering normal mode for AMG8833, restarting and setting general registers
int AMG8833_init(uint8_t pcr, uint8_t rr, uint8_t frr, uint8_t icr)
{
    // initialize I2C for communication with AMG8833
    TWI_init();

    /////////////////////////////////////////////////
    LOG_debug("Set Operating Mode:");
    AMG8833_setRegisterByte(AMG8833_PCTL, pcr);

    /////////////////////////////////////////////////
    LOG_debug("Perform Software Reset:");
    AMG8833_setRegisterByte(AMG8833_RST, rr);
    
    /////////////////////////////////////////////////
    LOG_debug("Set Frame Rate:");
    AMG8833_setRegisterByte(AMG8833_FPSC, frr);

    /////////////////////////////////////////////////
    LOG_debug("Disable Interrupts:");
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
    int numberBytes = AMG8833_GRID_PIXELS * 2; // the value for each pixel is stored in 2 bytes
    
    uint8_t rawGridData[numberBytes]; // buffer for raw input from device
    memset(rawGridData, 0, numberBytes*sizeof(uint8_t));

    // read Grid Bytes starting with lower bit from first Pixel
    LOG_debug("Read Grid");
    TWI_readBytes(AMG8833_SLAVE_ADDRESS, AMG8833_T01L, numberBytes, &rawGridData[0]);
    LOG_debug("...Grid reading done!");
    
    // combine two bytes for each Pixel
    for(uint16_t ii = 0; ii < AMG8833_GRID_PIXELS; ii++) {
        rawGridValues[ii] = ((uint16_t) rawGridData[2*ii + 1] << 8) | ((uint16_t) rawGridData[2*ii]);
    }

    // transform raw value into Celsius degree according to data sheet calculation
    for(int ii = 0; ii < AMG8833_GRID_PIXELS; ii++)
    {
        *resultGridValues++ = int12ToFloat(rawGridValues[ii]) * AMG8833_PIXEL_CONVERSION;
        
    }
}

float AMG8833_readThermistor(void)
{
    // initialize buffer for the two bytes of Thermistor
    uint8_t rawData[AMG8833_THERMISTOR_BYTES];
    memset(rawData, 0, AMG8833_THERMISTOR_BYTES*sizeof(uint8_t));

    LOG_debug("Read Thermistor");

    // read two bytes from Thermistor
    TWI_readBytes(AMG8833_SLAVE_ADDRESS, AMG8833_TTHL, AMG8833_THERMISTOR_BYTES, &rawData[0]);
    LOG_debug("...Thermistor reading done!");

    // combine two bytes into uint16_t
    uint16_t thermistorValueRaw =  ((uint16_t) rawData[1] << 8) | ((uint16_t) rawData[0]);

    // return float value containing temperature in Celsius
    return signedMag12ToFloat(thermistorValueRaw) * AMG8833_THERMISTOR_CONVERSION;
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