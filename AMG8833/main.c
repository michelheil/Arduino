/*
 * AMG8833.c
 *
 * Example code: www.nongnu.org/avr-libc/user-manual/group__twi__demo.html
 * Documentation of util/twi.h: www.nongnu.org/avr-libc/user-manual/group__util__twi.html
 * Schaltplan mit Arduino: learn.adafruit.com/assets/43127
 * Sehr gute Hilfen: embedds.com/programming-avr-i2c-interface/
 *
 * Created: 28.09.2019 22:41:28
 * Author : Michael
 */ 

/*
ToDo:
- AMG8833 soldering (done!)
- write general function to read out bytes with parameters: device/slave address, register address, expected bytes, buffer to store data
- handle all Status aus dem TWSR (nach erfolgreichem test)
- write individual function for sending start, stop signal through Control Register (done)
- Auslagerung in myTWI.h and myAMG8833.h (nach erfolgreichem test)
- define constants for 8x8 128 bytes gridRead (done)
- convert string of 64 uint16 into 8x8 matrix (nach erfolgreichem test)
- initialize I2C for ATMega328p (done)
- initialize AMG8833 (setting mode, interrupts disabled, frames per second...) (done)
- internal Pin-up resistors aktivieren (PC4 und PC5) (done)
- rename readThermistor so dass man Device und register addresse verwenden kann (nach erfolgreichem test)
- schreibe alle USART Befehler nur mit #ifdef "DEBUG"
*/

#define F_CPU                   16000000L

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h> // www.nongnu.org/avr-libc//user-manual/group__avr__stdlib.html
#include <util/twi.h>
#include <util/delay.h>
#include "myLCD.h"
#include "myUSART.h"


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

// AMG8833 Power Control Register (PCTL)
#define AMG8833_PCTL                    0x00
#define AMG8833_PCTL_NORMAL_MODE        0x00
#define AMG8833_PCTL_SLEEP_MODE         0x10
#define AMG8833_PCTL_STAND_BY_MODE_60   0x20 // 60 sec intermittence
#define AMG8833_PCTL_STAND_BY_MODE_10   0x21 // 10 sec intermittence

// AMG8833 Reset Register (RST)
#define AMG8833_RST                     0x01
#define AMG8833_RST_FLAG_RESET          0x30
#define AMG8833_RST_INITIAL_RESET       0x3F

// AMG8833 Frame Rate Register (FPSC)
#define AMG8833_FPSC                    0x02
#define AMG8833_FPSC_1FPS               0x01
#define AMG8833_FPSC_10FPS              0x00

// AMG8833 Interrupt Control Register (INTC)
#define AMG8833_INTC                    0x03
#define AMG8833_INTC_INTMOD_ABS         0x02
#define AMG8833_INTC_INTMOD_DIFF        0x00
#define AMG8833_INTC_INTEN_ACTIVE       0x01
#define AMG8833_INTC_INTEN_REACTIVE     0x00


// AMG8833 I2C Slave Address is 0x69
#define AMG8833_SLAVE_ADDRESS           0x69
#define AMG8833_SLA_W                   ((AMG8833_SLAVE_ADDRESS << 1) | TW_WRITE) // shifted by one as the address is stored in the higher 7-bits
#define AMG8833_SLA_R                   ((AMG8833_SLAVE_ADDRESS << 1) | TW_READ)  // TW_WRITE (=0) and TW_READ (=1) are defined in util/twi.h

// AMG8833 Thermistor Register
#define AMG8833_THERMISTOR_BYTES        2       // Thermistor value consist of 2 bytes. bits T7..0 in lower byte and T10..8 in higher byte
                                                // bit3 in higher byte carries sign (1=minus; 0=plus)
#define AMG8833_TTHL                    0x0E    // Thermistor Temperature Register (lower level)
#define AMG8833_TTHH                    0x0F    // Thermistor Temperature Register (higher level)
#define AMG8833_THERMISTOR_CONVERSION   0.0625  // According to data sheet: "1 LSB has 12 bit resolution which is equivalent to 0.0625"


// AMG8833 8x8 Temperature Grid Register
#define AMG8833_GRID_BYTES              128     // Thermistor value consist of 8x8 witch 2 bytes each. bits T7..0 in lower byte and T10..8 in higher byte
                                                // bit3 in higher byte carries sign (1=minus; 0=plus)
#define AMG8833_T01L                    0x80    // Pixel 1 Temperature Register (lower level)
#define AMG8833_T01H                    0x81    // Pixel 1 Temperature Register (higher level)
#define AMG8833_T64L                    0xFE    // Pixel 64
#define AMG8833_T64H                    0xFF    // Pixel 64
#define AMG8833_PIXEL_CONVERSION        0.25    // According to data sheet: "1 LSB has 12 bit resolution (11 bit + sign) which is equivalent to 0.25"

#define DTOSTR_ALWAYS_SIGN   0x01
#define DTOSTR_PLUS_SIGN   0x02 /* put '+' rather than ' ' */

// setting TWAR is only required when the ATmega328p is in slave mode
// TWAR = (AMG8833_SLAVE_ADDRESS << 1); // move one bit to left as bit0 of TWAR is used for General Call

// TWI general function
void    TWI_init(void);
float   TWI_readThermistor(void);
float * TWI_readGrid(void);
int     TWI_readAMG8833Bytes(uint8_t reg, int len, uint8_t * dest);

// TWI function for transmissions
void    TWI_startTransmission(void);
void    TWI_repeatStartTransmission(void);
void    TWI_writeSlaW(void);
void    TWI_writeRegisterAddress(uint8_t addr);
void    TWI_writeByte(uint8_t addr);
void    TWI_writeSlaR(void);
void    TWI_stopTransmission(void);


// AMG8833 specific functions
int     AMG8833_init();
float   AMG8833_signedMag12ToFloat(uint16_t val);
float   AMG8833_int12ToFloat(uint16_t val);


int main(void)
{
    float thermFloatValue;
    char stringBuffer[10];
    
    // Initialize LCD display, TWI ports and AMG8833 device
    LCD_init(); // includes clear display
    TWI_init();
    USART_init();
    USART_writeString("\r\n");
    AMG8833_init();


    // start LCD Cursor at Home
    LCD_setCursorHome();
    LCD_sendDataString("Temperature:");

    while (1) 
    {
        // Write to beginning of second line in LCD
        LCD_setCursorTo(0, 2);
        
        // read out Thermistor value and print it on display
USART_writeString("\r\n\r\n");
USART_writeString("Reading Thermistor:");
USART_writeString("\r\n");        
        
        thermFloatValue = TWI_readThermistor();
dtostrf(thermFloatValue, 9, 4, stringBuffer);
//dtostre(thermFloatValue, stringBuffer, 4, (DTOSTR_ALWAYS_SIGN | DTOSTR_PLUS_SIGN));
        LCD_sendDataString(&stringBuffer[0]);
        LCD_sendDataString(" C");
        
        // repeat measure every 5 seconds
        _delay_ms(5000);
    }
    
    return 0;

}

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

// entering normal mode for AMG8833, restarting and setting general registers
int AMG8833_init()
{
    /////////////////////////////////////////////////
    // set Operating Mode
    USART_writeString("\r\n");
    USART_writeString("Set Operating Mode:");
    USART_writeString("\r\n");    
    TWI_startTransmission();
    TWI_writeSlaW();
    TWI_writeRegisterAddress(AMG8833_PCTL);
    TWI_writeByte(AMG8833_PCTL_NORMAL_MODE);
    TWI_stopTransmission();


    /////////////////////////////////////////////////
    // software reset
    USART_writeString("\r\n\r\n");
    USART_writeString("Perform Software Reset:");
    USART_writeString("\r\n");
    TWI_startTransmission();
    TWI_writeSlaW();
    TWI_writeRegisterAddress(AMG8833_RST);
    TWI_writeByte(AMG8833_RST_INITIAL_RESET);
    TWI_stopTransmission();
    _delay_ms(500);

    /////////////////////////////////////////////////
    // set frame rate
    USART_writeString("\r\n\r\n");
    USART_writeString("Set Frame Rate:");
    USART_writeString("\r\n");
    TWI_startTransmission();
    TWI_writeSlaW();
    TWI_writeRegisterAddress(AMG8833_FPSC);
    TWI_writeByte(AMG8833_FPSC_10FPS);
    TWI_stopTransmission();
    _delay_ms(500);

    /////////////////////////////////////////////////
    // disable interrupts
    USART_writeString("\r\n\r\n");
    USART_writeString("Disable Interrupts:");
    USART_writeString("\r\n");    
    TWI_startTransmission();
    TWI_writeSlaW();
    TWI_writeRegisterAddress(AMG8833_INTC);
    TWI_writeByte(AMG8833_INTC_INTEN_REACTIVE);
    TWI_stopTransmission();

    _delay_ms(100);

    return 0;
}



// wrapper function for handing over buffer array to readAMG833Bytes
float TWI_readThermistor(void) { /// ToDo: Rename to read2BytesFromAddress
    // initialize buffer for the two bytes of Thermistor
    uint8_t rawData[2] = {0, 0};

USART_writeString("\r\n");
USART_writeString("Prepared rawData[2]");

USART_writeString("\r\n");
USART_writeString("Start reading Thermistor...");
    // read two bytes from Thermistor
    TWI_readAMG8833Bytes(AMG8833_TTHL, AMG8833_THERMISTOR_BYTES, &rawData[0]);
USART_writeString("\r\n");
USART_writeString("...Thermistor reading done!");

    // combine two bytes into uint16_t
    uint16_t data =  ((uint16_t) rawData[1] << 8) | ((uint16_t) rawData[0]);
USART_writeString("\r\n");
USART_sendChar((rawData[0]>>7)+101);
USART_writeString("\r\n");
USART_sendChar(rawData[1]+100);
USART_writeString("\r\n");
USART_writeString("Combined two bytes to uint16_t");    
    
    // return Thermistor bytes as one 16-bit variable
    return AMG8833_signedMag12ToFloat(data) * AMG8833_THERMISTOR_CONVERSION;
}


// convert a 12-bit signed magnitude value to a floating point number
// According to data sheet AMG8833: 12-bit Thermistor resolution is indicated as code (sign) + absolute value
float AMG8833_signedMag12ToFloat(uint16_t val)
{
    //take first 11 bits as absolute val by applying the 11-bit mask 0x7FF
    uint16_t absVal = (val & 0x7FF);
    
uint8_t temp = (uint8_t) absVal;
USART_writeString("\r\n");
USART_sendChar(temp);
    
    // if 12th bit (0x800) is 0 (= minus) then return negative absolute value, otherwise just return positive (absolute) value
    return (val & 0x800) ? 0 - (float) absVal : (float) absVal;
}


// wrapper function for handing over buffer array to readAMG833Bytes
// github.com/jodalyst/AMG8833/blob/master/src/AMG8833.cpp// call method: float * tempGridValues = readGrid();float * TWI_readGrid(void) {
    
    static float tempValues[64]; // return value buffer
    uint8_t rawGridData[128] = {0}; // raw input from AMG8833

    // read Grid Bytes starting with lower bit from first Pixel
    TWI_readAMG8833Bytes(AMG8833_T01L, AMG8833_GRID_BYTES, &rawGridData[0]);
    
    for(uint16_t ii = 0; ii < 64; ii++) {
        // combine two bytes for each Pixel
        uint16_t combinedBytes = ((uint16_t) rawGridData[2*ii + 1] << 8) | ((uint16_t) rawGridData[2*ii]);
        
        // convert the two's complement form (Zweierkomplement) and multiply by Celsius conversion factor
        tempValues[ii] = AMG8833_int12ToFloat(combinedBytes) * AMG8833_PIXEL_CONVERSION;
    }
    
    return tempValues;
}

// convert a 12-bit integer two's complement value to a floating point number
// According to data sheet AMG8833: 12-bit Pixel resolution is indicated as two's complement form (Zweierkomplement)
float AMG8833_int12ToFloat(uint16_t val)
{
    // shift to left so that sign bit of 12 bit integer number is placed on sign bit of 16 bit signed integer number
    int16_t sVal = (val << 4); 

    // shift back the signed number. Call of return converts to float
    return sVal >> 4;
}



void TWI_startTransmission(void) 
{
USART_writeString("\r\n");
USART_writeString("Start");
    
        // Start Condition
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

USART_writeString("...");
        // Wait for TWINT Flag set. This indicates that the START condition has been transmitted
        while ((TWCR & (1 << TWINT)) == 0);

        // Check value of TWI Status Register. Mask prescaler bit1 and bit0, bit2 is unused anyway. If status different from START go to ERROR
        if ((TWSR & 0xF8) != TW_START) LCD_sendDataString("ERR:1"); // do not send stop after start condition
USART_writeString("success!");
}


void TWI_writeSlaW(void)
{
USART_writeString("\r\n");
USART_writeString("SLA+W");

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Write SLA+W (means Slave Address + Write-Flag) into TWI Data Register => entering Master Transmitter Mode
    TWDR = AMG8833_SLA_W;

    // set TWINT bit to clear TWINT flag and transmit the content of TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait as long as TWINT flag is set.
USART_writeString("...");
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_SLA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK) LCD_sendDataString("ERR:2");
    
USART_writeString("success!");    
}


void TWI_writeByte(uint8_t val)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Load local Register Address (not the I2C slave address) of device into TWDR Register
USART_writeString("\r\n");
USART_writeString("Write Byte");
    TWDR = val;

    // Clear TWINT bit in TWCR to start transmission of data
    TWCR = (1 << TWINT) | (1 << TWEN);

    // Wait for TWINT Flag set. This indicates that the DATA has been transmitted, and ACK/NACK has been received.
USART_writeString("...");
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MT_DATA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK) LCD_sendDataString("ERR:3");
USART_writeString("success!");
}


void TWI_writeRegisterAddress(uint8_t addr)
{
    TWI_writeByte(addr);
}


void TWI_repeatStartTransmission(void)
{
USART_writeString("\r\n");
USART_writeString("Repeat Start");
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Repeated Start Condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // Wait for TWINT Flag set. This indicates that the repeated START condition has been transmitted
USART_writeString("...");    
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from REP_START go to ERROR
    if ((TWSR & 0xF8) != TW_REP_START) LCD_sendDataString("ERR:4");
USART_writeString("success!");    
}


void TWI_writeSlaR(void)
{
USART_writeString("\r\n");
USART_writeString("SLA+R");    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Write SLA+R (means Slave Address + Read-Flag) into TWI Data Register => entering Master Receiver Mode
    TWDR = AMG8833_SLA_R;

    // set TWINT bit to clear TWINT flag and transmit the content of TWDR
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wait as long as TWINT flag is set.
USART_writeString("...");    
    while ((TWCR & (1 << TWINT)) == 0);

    // Check value of TWI Status Register. Mask prescaler bits. If status different from MR_SLA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MR_SLA_ACK) LCD_sendDataString("ERR:5");
USART_writeString("success!");        
}


void TWI_stopTransmission(void)
{
USART_writeString("\r\n");
USART_writeString("STOP");    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Stop Condition
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
USART_writeString(" success!");   
}


// Use I2C (Two-Wire Protocol) to get data from AMG8833
int TWI_readAMG8833Bytes(uint8_t reg, int len, uint8_t * dest) {
        
    TWI_startTransmission();
    TWI_writeSlaW();
    TWI_writeRegisterAddress(reg);
    TWI_repeatStartTransmission();
    TWI_writeSlaR();
    
USART_writeString("\r\n");        
USART_writeString("In this stage: Start, SLA+W, RegAddr, RepStart, SLA+R is done!");
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // receive Data
    uint8_t twcr, twst = 0;
    int bytesReceived = 0;

USART_writeString("\r\n"); 
USART_writeString("Begin for loop");
    // TWEA - TWI Enable Acknowledge Bit (setting this bit will send back ACK as confirmation of receiving a byte)
    for (twcr = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); len > 0; len--)
    {
USART_writeString(" Durchlauf ");
        // After the last byte has been received, the MR should inform the ST by sending a NACK after the last received data byte.
        if (len == 1) { // if len == 1 then this is the last byte
            twcr = (1 << TWINT) | (1 << TWEN); /* send NOT ACK this time */
USART_writeString("len == 1");
        }
            
        // clear int to start transmission of either ACK or NACK after the last byte
        TWCR = twcr;
            
        // wait for transmission
USART_writeString(" Anfang Warten ");        
        while ((TWCR & (1 << TWINT)) == 0);
USART_writeString(" Fertig Warten ");
            
        switch (twst = (TWSR & 0xF8)) // masked Status Register
        {
            case TW_MR_DATA_NACK:
                len = 0; // force end of the loop
                USART_writeString(" TW_MR_DATA_NACK ");
                // FALLTHROUGH: no "break;" here means that the next case will be executed
            case TW_MR_DATA_ACK:
                *dest++ = TWDR;
                bytesReceived++;
                if(twst == TW_MR_DATA_NACK) TWI_stopTransmission();
                USART_writeString(" TW_MR_DATA_ACK ");
                break;
            default:
            LCD_setCursorTo(0,2);
            LCD_sendDataString("ERR:6");
            TWI_stopTransmission();
        }
    }
    
USART_writeString("\r\n");
USART_writeString("End for loop");    
        
    TWI_stopTransmission();
    return bytesReceived;
}

