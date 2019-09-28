/*
 * myLCD.h
 *
 * library to sending data to a 1602 LCD in 4-bit mode
 *
 * Created: 26.09.2019 20:58:42
 * Author : Michael
 */ 

#ifndef MYLCD_H
#define MYLCD_H

////////////////////////////////////////////////////////////////////////////////
// Define pin settings for a 4-bit data transfer mode
// Ensure that  all 4 pins are consecutive pin within one and the same port!

// LCD DB4-DB7 linked to:
#define LCD_PORT      PORTD
#define LCD_DDR       DDRD // Data Direction Register (1=output, 0=input)
#define LCD_DB        PD4  // starting bit within port (means PD7..5 are also linked)

// LCD RS linked to:
#define LCD_RS        PD2 // (1=data, 0=command)

// LCD E (Enable) linked to:
#define LCD_E         PD3 // (1=impulse for data)


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

void LCD_init(void);
void LCD_clearDisplay(void);
void LCD_sendCommandByte(uint8_t data);
void LCD_sendDataByte(uint8_t data);
void LCD_sendDataString(const char *data);
void LCD_setCursorHome(void);
void LCD_setCursorTo(uint8_t col, uint8_t row);


////////////////////////////////////////////////////////////////////////////////
// Definition eines benutzerdefinierten Sonderzeichens.
// data muss auf ein Array[8] mit den Zeilencodes des zu definierenden Zeichens
// zeigen. Später können diese mit lcd_data(0-7) aufgerufen werden
void lcd_generatechar(uint8_t startadresse, const uint8_t *data);


#endif // MYLCD_H