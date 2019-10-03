/*
 * AMG8833.c
 *
 * Example code: www.nongnu.org/avr-libc/user-manual/group__twi__demo.html
 * Documentation of util/twi.h: www.nongnu.org/avr-libc/user-manual/group__util__twi.html
 * Schaltplan mit Arduino: learn.adafruit.com/assets/43127
 *
 * Created: 28.09.2019 22:41:28
 * Author : Michael
 */ 

/*
ToDo:
- AMG8833 soldering
- write general function to read out bytes with parameters: device/slave address, register address, expected bytes, buffer to store data
- handle all Status
- write individual function for sending start, stop signal through Control Register
- Auslagerung in myTWI.h
- define constants for 8x8 128 bytes gridRead
- convert string of 64 uint16
*/

#define F_CPU                   16000000L

#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include "myLCD.h"


/*
 * According to the AMG88** data sheet the SCL clock frequency (f_SCL) has
 * min = 0 kHz and max = 400 kHZ
 *
 * According to the ATmega328p data sheet the SCL clock frequency is
 * f_SCL = CPU_ClockFrequency / (16 + 2 * TWBR * PrescalerValue)
 * where TWBR is the value of the TWI Bit Rate Register
 * and the PrescaleValue (in {1, 4, 16, 64}) can be set through the TWSR (TWI Status Register)
 * 
 * Assuming we want to work with a f_SCL of 100kHz
 * => TWBR * PrescalerValue = 72
 * => TWBR = 18 = 0b00010010
 * => PrescalerValue: TWPS1 = 0, TWPS0 = 1 (both bits are part of byte TWSR)
 
*/

#define TWI_PRESCALER_VALUE     0x04 // Dec: 4
#define TWI_BIT_RATE            0x12 // Dec: 18


/*
 * TWSR - TWI Status Register (initial value: 0xF8)
 * TWBR - TWI Bit Rate Register (initial value: 0x00)
 * TWCR - TWI Control Register
 * TWDR - TWI Data Register (initial value: 0xFF)
 * TWAR - TWI (Slave) Address Register; Includes TWGCE (TWI General Call Recognition Enable Bit) as bit 0.
 * TWAMR - TWI (Slave) Address Mask Register
*/


// AMG8833 I2C Slave Address is 0x69
#define AMG8833_SLAVE_ADDRESS       0x69
#define AMG8833_SLA_W               ((AMG8833_SLAVE_ADDRESS << 1) | TW_WRITE) // shifted by one as the address is stored in the higher 7-bits
#define AMG8833_SLA_R               ((AMG8833_SLAVE_ADDRESS << 1) | TW_READ)  // TW_WRITE (=0) and TW_READ (=1) are defined in util/twi.h

// Thermistor Register
#define AMG8833_THERMISTOR_BYTES    2       // Thermistor value consist of 2 bytes. bits T7..0 in lower byte and T10..8 in higher byte
                                            // bit3 in higher byte carries sign (1=minus; 0=plus)
#define AMG8833_TTHL                0x0E    // Thermistor Temperature Register (lower level)
#define AMG8833_TTHH                0x0F    // Thermistor Temperature Register (higher level)

// 8x8 Temperature Grid Register
#define AMG8833_GRID_BYTES          128     // Thermistor value consist of 8x8 witch 2 bytes each. bits T7..0 in lower byte and T10..8 in higher byte
                                            // bit3 in higher byte carries sign (1=minus; 0=plus)
#define AMG8833_T01L                0x80    // Pixel 1 Temperature Register (lower level)
#define AMG8833_T01H                0x81    // Pixel 1 Temperature Register (higher level)
#define AMG8833_T64L                0xFE    // Pixel 64
#define AMG8833_T64H                0xFF    // Pixel 64



// setting TWAR is only required when the ATmega328p is in slave mode
// TWAR = (AMG8833_SLAVE_ADDRESS << 1); // move one bit to left as bit0 of TWAR is used for General Call
float temp = 0;

int16_t TWI_readThermistor(void);
float * TWI_readGrid(void);
int     TWI_readAMG8833Bytes(uint8_t reg, uint8_t len, uint8_t * dest);


int main(void)
{
        // Initialize LCD display
        LCD_init();

        // start Cursor at Home
        LCD_setCursorHome();
        LCD_sendDataString("Temperature:");
        
    
    while (1) 
    {
        LCD_setCursorTo(0, 2);
        temp = 0.0265f * TWI_readThermistor();
        LCD_sendDataByte((uint8_t) temp);
        LCD_sendDataString(" Celcius");
        
        _delay_ms(5000);
    }
    
    return 0;

}


// wrapper function for handing over buffer array to readAMG833ThermistorBytes
int16_t TWI_readThermistor(void) {
    // initialize buffer for the two bytes of Thermistor
    uint8_t rawData[2] = {0, 0};

    // read two bytes from Thermistor
    TWI_readAMG8833Bytes(AMG8833_TTHL, AMG8833_THERMISTOR_BYTES, &rawData[0]);
    
    // return Thermistor bytes as one 16-bit variable
    return (int16_t) (((int16_t) rawData[1] << 8) | rawData[0]);
}


// github.com/jodalyst/AMG8833/blob/master/src/AMG8833.cpp
// call method: float * tempGridValues = readGrid();
float * TWI_readGrid(void) {
    
    static float tempValues[64];
    uint8_t rawGridData[128] = {0};

    TWI_readAMG8833Bytes(AMG8833_T01L, AMG8833_GRID_BYTES, &rawGridData[0]);
    for(uint16_t ii = 0; ii < 64; ii++) {
        tempValues[ii] = (float) ((int16_t) ( (int16_t) rawGridData[2*ii + 1] << 8) | rawGridData[2*ii]);
        tempValues[ii] *=0.25f; // scale to get temperatures in degrees C. Refer to data sheet "Temperature Output Resolution"
    }
    
    return tempValues;
}


int TWI_readAMG8833Bytes(uint8_t reg, uint8_t len, uint8_t * dest) {
        
    uint8_t twcr, twst = 0;
    int bytesReceived = 0;
        
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Configure transmission speed        
    TWSR |= TWI_PRESCALER_VALUE;
    TWBR = TWI_BIT_RATE;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Start Condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // Wait for TWINT Flag set. This indicates that the START condition has been transmitted
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bit1 and bit0, bit2 is unused anyway. If status different from START go to ERROR
    if ((TWSR & 0xF8) != TW_START) LCD_sendDataString("ERROR: A"); // do not send stop after start condition

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Write SLA+W (means Slave Address + Write-Flag) into TWI Data Register => entering Master Transmitter Mode
    TWDR = AMG8833_SLA_W;

    // set TWINT bit to clear TWINT flag and transmit the content of TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait as long as TWINT flag is set.
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_SLA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK) LCD_sendDataString("ERROR: B");
    else goto stopSignal;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Load Register Address in device into TWDR Register
    TWDR = reg;

    // Clear TWINT bit in TWCR to start transmission of data
    TWCR = (1<<TWINT) | (1<<TWEN);

    // Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_DATA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK) LCD_sendDataString("ERROR: C");
    else goto stopSignal;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Repeated Start Condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // Wait for TWINT Flag set. This indicates that the repeated START condition has been transmitted
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from REP_START go to ERROR
    if ((TWSR & 0xF8) != TW_REP_START) LCD_sendDataString("ERROR: D");
    else goto stopSignal;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Write SLA+R (means Slave Address + Read-Flag) into TWI Data Register => entering Master Receiver Mode
    TWDR = AMG8833_SLA_R;

    // set TWINT bit to clear TWINT flag and transmit the content of TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait as long as TWINT flag is set.
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MR_SLA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MR_SLA_ACK) LCD_sendDataString("ERROR: E");
    else goto stopSignal;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // receive Data
    // TWEA - TWI Enable Acknowledge Bit (setting this bit will send back ACK as confirmation of receiving a byte)
    for (twcr = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); len > 0; len--)
    {
            
        // After the last byte has been received, the MR should inform the ST by sending a NACK after the last received data byte.
        if (len == 1) { // if len == 1 then this is the last byte
            twcr = (1 << TWINT) | (1 << TWEN); /* send NOT ACK this time */
        }
            
        // clear int to start transmission of either ACK or after the last byte
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
                if(twst == TW_MR_DATA_NACK) goto stopSignal;
                break;
            default:
                LCD_sendDataString("ERROR: F");
                goto stopSignal;
        }
            
    }
        
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Stop Condition
    stopSignal:
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    LCD_setCursorTo(0, 2);
    LCD_sendDataString("Sent STOP");
    return bytesReceived;
}
