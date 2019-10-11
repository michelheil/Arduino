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
- schreibe Logger
*/

#include "globalDefines.h"

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "myLCD.h"
#include "myUSART.h"
#include "myAMG8833.h"


int main(void)
{
    float amgTherm;
    float amgGrid[AMG8833_GRID_PIXELS_X][AMG8833_GRID_PIXELS_Y];
    
    // Initialize LCD display, TWI ports and AMG8833 device
    LCD_init(); // includes clear display
    USART_init(); // includes writing of a newLine
    AMG8833_init(AMG8833_PCTL_NORMAL_MODE, AMG8833_RST_INITIAL_RESET, AMG8833_FPSC_10FPS, AMG8833_INTC_INTEN_REACTIVE);

    // start LCD Cursor at Home
    LCD_setCursorHome();
    LCD_sendDataString("Temp:");
    
    while (1) 
    {
        LCD_setCursorTo(5, 1);
        
        // read out Thermistor value and print it on display
        amgTherm = AMG8833_readThermistor();
        LCD_sendDataFloat(amgTherm);
        LCD_sendDataString(" C");
        
        // Write to beginning of second line in LCD
        LCD_setCursorTo(0, 2);
        
        // Read out Grid values and display one value on LCD
        AMG8833_readGrid(&amgGrid[0][0]);
        LCD_sendDataString("Pixel_88");
        LCD_sendDataFloat(amgGrid[7][7]);
        
        // Send entire 8x8 matrix to USART
        for(int ii = 0; ii < 8; ii++)
        {
            for(int jj = 0; jj < 8; jj++)
            {
                USART_writeFloat(amgGrid[ii][jj]);
                USART_writeString(" ");
                if (jj == 7) USART_newLine();
            }
        }
 
        // repeat measure every 10 seconds
        _delay_ms(1000);
    }
    
    return 0;

}






