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
#include <util/delay.h>
#include "myLCD.h"
#include "myUSART.h"
#include "myTWI.h"

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
#include <util/twi.h>
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
#define AMG8833_GRID_PIXELS             64      // Thermistor value consist of 8x8 pixels. Value of Pixel is stored in two Bytes.
                                                // Bits T7..0 in lower byte and T10..8 in higher byte.
                                                // Bit3 in higher byte carries sign (1=minus; 0=plus)
#define AMG8833_T01L                    0x80    // Pixel 1 Temperature Register (lower level)
#define AMG8833_T01H                    0x81    // Pixel 1 Temperature Register (higher level)
#define AMG8833_T64L                    0xFE    // Pixel 64
#define AMG8833_T64H                    0xFF    // Pixel 64
#define AMG8833_PIXEL_CONVERSION        0.25    // According to data sheet: "1 LSB has 12 bit resolution (11 bit + sign) which is equivalent to 0.25"

// setting TWAR is only required when the ATmega328p is in slave mode
// TWAR = (AMG8833_SLAVE_ADDRESS << 1); // move one bit to left as bit0 of TWAR is used for General Call



// AMG8833 specific functions
int     AMG8833_init();

int main(void)
{
    float thermFloatValue;

    // Initialize LCD display, TWI ports and AMG8833 device
    LCD_init(); // includes clear display
    TWI_init();
    USART_init(); // includes writing of a newLine
    AMG8833_init();

    // start LCD Cursor at Home
    LCD_setCursorHome();
    LCD_sendDataString("Temperature:");

    while (1) 
    {
        // Write to beginning of second line in LCD
        LCD_setCursorTo(0, 2);
        
        // read out Thermistor value and print it on display
        thermFloatValue = TWI_readBytesFromAddressRaw(AMG8833_SLAVE_ADDRESS, AMG8833_TTHL, AMG8833_THERMISTOR_BYTES) * AMG8833_THERMISTOR_CONVERSION;
        LCD_sendDataFloat(thermFloatValue);
        LCD_sendDataString(" C");
        
        // repeat measure every 5 seconds
        _delay_ms(5000);
    }
    
    return 0;

}

// entering normal mode for AMG8833, restarting and setting general registers
int AMG8833_init()
{
    /////////////////////////////////////////////////
    // set Operating Mode
    USART_Headline("Set Operating Mode:");
    TWI_startTransmission();
    TWI_writeSlaW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(AMG8833_PCTL);
    TWI_writeByte(AMG8833_PCTL_NORMAL_MODE);
    TWI_stopTransmission();


    /////////////////////////////////////////////////
    // software reset
    USART_Headline("Perform Software Reset:");
    TWI_startTransmission();
    TWI_writeSlaW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(AMG8833_RST);
    TWI_writeByte(AMG8833_RST_INITIAL_RESET);
    TWI_stopTransmission();
    _delay_ms(500);

    /////////////////////////////////////////////////
    // set frame rate
    USART_Headline("Set Frame Rate:");
    TWI_startTransmission();
    TWI_writeSlaW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(AMG8833_FPSC);
    TWI_writeByte(AMG8833_FPSC_10FPS);
    TWI_stopTransmission();
    _delay_ms(500);

    /////////////////////////////////////////////////
    // disable interrupts
    USART_Headline("Disable Interrupts:");
    TWI_startTransmission();
    TWI_writeSlaW(AMG8833_SLA_W);
    TWI_writeRegisterAddress(AMG8833_INTC);
    TWI_writeByte(AMG8833_INTC_INTEN_REACTIVE);
    TWI_stopTransmission();

    _delay_ms(100);

    return 0;
}







