/*
    Programm to sending data to a 1602 LCD in 4-bit mode
    Copyright (C) 2019  Michael Heil

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 
 *
 * Created: 26.09.2019 20:58:42
 * Author : Michael
 */ 

#ifndef MYLCD_H
#define MYLCD_H

////////////////////////////////////////////////////////////////////////////////
// Define pin settings for a 4-bit data transfer mode
// Ensure that all 4 pins are consecutive pins 
// and the other two pins (RS, E) are within one and the same port!!!

// LCD DB4-DB7 linked to:
#define LCD_PORT      PORTB
#define LCD_DDR       DDRB // Data Direction Register (1=output, 0=input)
#define LCD_DB        PB2  // starting bit within port (means PB5, PB4, PB3 are also linked)

// LCD RS linked to:
#define LCD_RS        PB0 // (1=data, 0=command)

// LCD E (Enable) linked to:
#define LCD_E         PB1 // (1=impulse for data)

// define float precision on display
#define LCD_DISPLAY_PRECISION       2 // amount of decimal digits
#define LCD_DISPLAY_VALUE_LENGTH    (LCD_DISPLAY_PRECISION + 4) // + '.' + 2 digits before decimal point + sign

// if dtostre is used (and not dtostrf)
#define LCD_DTOSTR_ALWAYS_SIGN  0x01
#define LCD_DTOSTR_PLUS_SIGN    0x02 /* put '+' rather than ' ' */
// Usage: dtostre(thermFloatValue, stringBuffer, LCD_DISPLAY_PRECISION, (LCD_DTOSTR_PLUS_SIGN | LCD_DTOSTR_ALWAYS_SIGN));

////////////////////////////////////////////////////////////////////////////////
// E-Cycle times (MS=milliseconds, US=microseconds)

#define LCD_BOOTUP_MS           15
#define LCD_SOFT_RESET1_MS      5
#define LCD_SOFT_RESET2_US      1

#define LCD_CLEAR_DISPLAY_MS    3
#define LCD_CURSOR_RETURN_MS    3
#define LCD_COMMAND_MS          5

////////////////////////////////////////////////////////////////////////////////
// Display Data RAM Address
#define LCD_DDADR_FIRST_LINE    0x00
#define LCD_DDADR_SECOND_LINE   0x40


////////////////////////////////////////////////////////////////////////////////
// Options for LCD commands can be used by LCD_sendCommandByte

// Clear Display -------------- 0b00000001
#define LCD_CLEAR_DISPLAY       0x01

// Cursor Home ---------------- 0b0000001x
#define LCD_CURSOR_HOME         0x02

// Set Entry Mode ------------- 0b000001xx
#define LCD_SET_ENTRY           0x04

#define LCD_ENTRY_DECREASE      0x00
#define LCD_ENTRY_INCREASE      0x02
#define LCD_ENTRY_NOSHIFT       0x00
#define LCD_ENTRY_SHIFT         0x01

// Set Display ---------------- 0b00001xxx
#define LCD_SET_DISPLAY         0x08

#define LCD_DISPLAY_OFF         0x00
#define LCD_DISPLAY_ON          0x04
#define LCD_CURSOR_OFF          0x00
#define LCD_CURSOR_ON           0x02
#define LCD_BLINKING_OFF        0x00
#define LCD_BLINKING_ON         0x01

// Set Shift ------------------ 0b0001xxxx
#define LCD_SET_SHIFT           0x10

#define LCD_CURSOR_MOVE         0x00
#define LCD_DISPLAY_SHIFT       0x08
#define LCD_SHIFT_LEFT          0x00
#define LCD_SHIFT_RIGHT         0x04

// Set Function --------------- 0b001xxxxx
#define LCD_SET_FUNCTION        0x20

#define LCD_FUNCTION_4BIT       0x00
#define LCD_FUNCTION_8BIT       0x10
#define LCD_FUNCTION_1LINE      0x00
#define LCD_FUNCTION_2LINE      0x08
#define LCD_FUNCTION_5X8        0x00
#define LCD_FUNCTION_5X10       0x04

#define LCD_SOFT_RESET          0x30

// Set Character Generator RAM Address --------- 0b01xxxxxx
#define LCD_SET_CGADR           0x40

// Set Display Data RAM Address 0b1xxxxxxx
#define LCD_SET_DDADR           0x80

////////////////////////////////////////////////////////////////////////////////
// functions
/**
 * @brief Initializes the LCD display
 * 
 * - set the four data, the RS and the E pins as output
 * - set all values of output port to 0
 * - wait for booting up
 * - according to data sheet a reset has to be done 3 times
 * - Function Set: e.g. activate 4-bit mode
 * - Function Set: e.g. activate 4-bit mode, activate two lines in display, activate 5x8 dots character fonts
 * - Display Control: e.g. display on, cursor off, blinking off
 * - Entry Mode Set: e.g. Cursor increment, no shifting
 * - clear display
  */
void LCD_init(void);

/**
 * @brief clear display (which implicitly sets cursor to home)
 * 
 */
void LCD_clearDisplay(void);

/**
 * @brief Display a const string to current cursor position 
 * 
 * @param data pointer to char of string that is displayed
 */
void LCD_sendDataString(const char *data);

/**
 * @brief Display an unsigned char as string to current cursor position 
 * 
 * @param data pointer to unsinged char of string that is displayed
 */
void LCD_sendDataUint(unsigned char *data);

/**
 * @brief Display unsigned int to the current cursor position
 * 
 * @param val unsingned int (16-bit) value that is displayed
 */
void LCD_sendDataUint16(uint16_t val);

/**
 * @brief Display a floating value with two decimals to current cursor position 
 * 
 * @param val float value that is displayed
 */
void LCD_sendDataFloat(float val);

/**
 * @brief Set cursore of the LCD to first position ("home")
 * 
 */
void LCD_setCursorHome(void);

/**
 * @brief Set the cursor based on the column and the row
 * 
 * @param col column of display starting with 0
 * @param row row of display starting with 1
 */
void LCD_setCursorTo(uint8_t col, uint8_t row);


////////////////////////////////////////////////////////////////////////////////
// Definition eines benutzerdefinierten Sonderzeichens.
// data muss auf ein Array[8] mit den Zeilencodes des zu definierenden Zeichens
// zeigen. Spaeter koennen diese mit lcd_data(0-7) aufgerufen werden
void lcd_generatechar(uint8_t startadresse, const uint8_t *data);


#endif // MYLCD_H