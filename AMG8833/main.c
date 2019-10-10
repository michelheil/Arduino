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
- schreibe Logger
*/

#define F_CPU                   16000000L

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "myLCD.h"
#include "myUSART.h"
#include "myTWI.h"
#include "myAMG8833.h"


int main(void)
{
    float thermFloatValue;
    uint16_t results[AMG8833_GRID_PIXELS];
    uint16_t resultsM[8][8];

    // Initialize LCD display, TWI ports and AMG8833 device
    LCD_init(); // includes clear display
    USART_init(); // includes writing of a newLine
    AMG8833_init();

    // start LCD Cursor at Home
    LCD_setCursorHome();
    LCD_sendDataString("Temp:");

    while (1) 
    {
        LCD_setCursorTo(5, 1);
        
        // read out Thermistor value and print it on display
        thermFloatValue = TWI_readBytesFromAddressRaw(AMG8833_SLAVE_ADDRESS, AMG8833_TTHL, AMG8833_THERMISTOR_BYTES) * AMG8833_THERMISTOR_CONVERSION;
        LCD_sendDataFloat(thermFloatValue);
        LCD_sendDataString(" C");
        
        // Write to beginning of second line in LCD
        LCD_setCursorTo(0, 2);
        
        TWI_readPairBytesFromAddressRaw(AMG8833_SLAVE_ADDRESS, AMG8833_T01L, AMG8833_GRID_PIXELS, &results[0]);        float gridValue = TWI_int12ToFloat(results[63]) * AMG8833_PIXEL_CONVERSION;
        LCD_sendDataFloat(gridValue);
        
        TWI_readPairBytesFromAddressRaw(AMG8833_SLAVE_ADDRESS, AMG8833_T01L, AMG8833_GRID_PIXELS, &resultsM[0][0]);        float gridValueM = TWI_int12ToFloat(resultsM[7][7]) * AMG8833_PIXEL_CONVERSION;
        LCD_sendDataFloat(gridValueM);
                
        for(int ii = 0; ii < 8; ii++) 
        {
            for(int jj = 0; jj < 8; jj++)
            {
                float res = TWI_int12ToFloat(resultsM[ii][jj]) * AMG8833_PIXEL_CONVERSION;
                USART_writeFloat(res);
                USART_writeString(" ");
                if (jj == 7) USART_newLine();
            }
        }    
    
        // repeat measure every 10 seconds
        _delay_ms(10000);
    }
    
    return 0;

}






