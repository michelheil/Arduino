/*
 * myLCD.c
 *
 * library to sending data to a 1602 LCD in 4-bit mode
 * Check SPLC780D Controller/Driver
 *
 * Created: 26.09.2019 20:58:42
 * Author : Michael
 */ 

#include "globalDefines.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h> // www.nongnu.org/avr-libc//user-manual/group__avr__stdlib.html
#include "myLCD.h"


// Creates the start signal for reading or writing data
static void LCD_startReadWriteSignal(void)
{
    sbi(LCD_PORT, LCD_E);       // set pin to 1 which gives the signal to read/write the data
    _delay_ms(LCD_COMMAND_MS);  // it takes some time to read the data
    cbi(LCD_PORT, LCD_E);       // set pin to 0 to stop reading/writing
}

// send the four high bits of a byte through 4-bit LCD_PORT without affecting other LCD_PORT pins
static void LCD_sendHighNibble(uint8_t data)
{
    data &= 0xF0;                       // mask higher bits as only they should be send
    
    LCD_PORT &= ~(0xF0 >> (4-LCD_DB));  // delete masking (=> higher 4 bits are set to 0, lower 4 bits keep as they were)
    LCD_PORT |= (data >> (4-LCD_DB));   // set the 4 higher bits
    LCD_startReadWriteSignal();
}

// Initializing the LCD following the instruction in data sheet
void LCD_init(void)
{
    // set the four data, the RS and the E pins as output
    uint8_t pins = (0x0F << LCD_DB) | (1 << LCD_RS) | (1 << LCD_E);
    LCD_DDR |= pins;
    
    // set all values of output port to 0
    LCD_PORT &= ~pins;
    
    // wait for booting up
    _delay_ms(LCD_BOOTUP_MS);
    
    // according to data sheet a reset has to be done 3 times
    LCD_sendHighNibble(LCD_SOFT_RESET);
    _delay_ms(LCD_SOFT_RESET1_MS);
    LCD_startReadWriteSignal();
    _delay_us(LCD_SOFT_RESET2_US);
    LCD_startReadWriteSignal();
    _delay_us(LCD_SOFT_RESET2_US);
    
    // Function Set: activate 4-bit mode
    LCD_sendHighNibble(LCD_SET_FUNCTION | LCD_FUNCTION_4BIT);
    _delay_ms(LCD_COMMAND_MS);

    // only after activating 4-bit modus we can use full 8 bits for function setting
    // (even though we still send them as two 4-bits)
    // Function Set: activate 4-bit mode, activate two lines in display, activate 5x8 dots character fonts
    LCD_sendCommandByte(LCD_SET_FUNCTION | LCD_FUNCTION_4BIT | LCD_FUNCTION_2LINE | LCD_FUNCTION_5X8);
    
    // Display Control: display on, cursor off, blinking off
    LCD_sendCommandByte(LCD_SET_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINKING_OFF);
    
    // Entry Mode Set: Cursor increment, no shifting
    LCD_sendCommandByte(LCD_SET_ENTRY | LCD_ENTRY_INCREASE | LCD_ENTRY_NOSHIFT);
    
    LCD_clearDisplay();
}

// send a data byte to LCD
void LCD_sendDataByte(uint8_t data)
{
    LCD_PORT |= (1<<LCD_RS);      // set RS to 1 as this activates data register
    
    LCD_sendHighNibble(data);     // send hi-nibble
    LCD_sendHighNibble(data<<4);  // send lo-nibble
    
    _delay_ms(LCD_COMMAND_MS);
}

// send a command byte to LCD
void LCD_sendCommandByte(uint8_t data)
{
    LCD_PORT &= ~(1<<LCD_RS);     // set RS to 0 as this activates command register
    
    LCD_sendHighNibble(data);     // send hi-nibble
    LCD_sendHighNibble(data<<4);  // send lo-nibble
    
    _delay_ms(LCD_COMMAND_MS);
}

// send command to clear display
void LCD_clearDisplay(void)
{
    LCD_sendCommandByte(LCD_CLEAR_DISPLAY);
    _delay_ms(LCD_CLEAR_DISPLAY_MS);
}

// send command to set cursor to home position
void LCD_setCursorHome(void)
{
    LCD_sendCommandByte(LCD_CURSOR_HOME);
    _delay_ms(LCD_CURSOR_RETURN_MS);
}

// send command to setting the cursor based on the column (starting with 0) and the row (starting with 1)
void LCD_setCursorTo(uint8_t col, uint8_t row)
{
    uint8_t data;
    
    switch (row)
    {
        case 1:
            data = LCD_SET_DDADR + LCD_DDADR_FIRST_LINE + col;
        break;
        
        case 2:
            data = LCD_SET_DDADR + LCD_DDADR_SECOND_LINE + col;
        break;
        
        default:
            LCD_clearDisplay();
            LCD_setCursorHome();
            LCD_sendDataString("ERROR: ERR001");
        return;
    }
    
    LCD_sendCommandByte(data);
}

// sends data as a string to the current cursor position
void LCD_sendDataString(const char *data)
{
    while(*data != '\0') {
        LCD_sendDataByte(*data++);    
    }
}

// sends data as a unsinged char to the current cursor position
void LCD_sendDataUint(unsigned char *data)
{
    while(*data != '\0') {
        LCD_sendDataByte(*data++);
    }
}

// sends unsigned int (16-bit) to the current cursor position
void LCD_sendDataUint16(uint16_t val)
{
    static char stringBuffer[2];
	itoa(val, stringBuffer, 10);
    LCD_sendDataString(&stringBuffer[0]);

}

// send data as float to the current cursor position
void LCD_sendDataFloat(float val)
{
    // create Buffer
    char stringBuffer[LCD_DISPLAY_VALUE_LENGTH+1]; // +1 for the ending '\0'
    dtostrf(val, LCD_DISPLAY_VALUE_LENGTH, LCD_DISPLAY_PRECISION, stringBuffer);
    LCD_sendDataString(&stringBuffer[0]);
}




////////////////////////////////////////////////////////////////////////////////
// Schreibt ein Zeichen in den Character Generator RAM
void lcd_generatechar(uint8_t startadresse, const uint8_t *data)
{
    // Startposition des Zeichens einstellen
    LCD_sendCommandByte(LCD_SET_CGADR | (startadresse<<3)); //Startadressen: 0;1;2;3;4;5;6;7
    
    // Bitmuster �bertragen
    for (uint8_t i=0; i<8; i++)
    {
        LCD_sendDataByte(data[i]);
    }
    LCD_sendCommandByte(LCD_SET_DDADR); //DRAM auf 0 setzen
}