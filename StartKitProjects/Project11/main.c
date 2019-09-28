/*
 * ArduinoStarterKitProject11.c
 *
 * Created: 26.09.2019 20:58:42
 * Author : Michael
 */ 
// https://www.mikrocontroller.net/articles/HD44780
// https://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung

#include <avr/io.h>
#include "myLCD.h"

int main(void)
{
    // Initialize LCD display
    LCD_init();

    // start Cursor at Home
    LCD_setCursorHome();
    LCD_sendDataByte('T');
    LCD_sendDataByte('e');
    LCD_sendDataByte('s');
    LCD_sendDataByte('t');

    // set cursor to the beginning of second line
    LCD_setCursorTo(0, 2);

    // send an entire string to display
    LCD_sendDataString("Hello World!");
    
    //LCD_setCursorTo(4,3);

    while(1)
    {
        
    }

    return 0;
}
