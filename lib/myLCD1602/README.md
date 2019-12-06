## Description
LCD1602 module

- Display construction: 16 Characters * 2 Lines
- Display mode: STN(Y/G) 
- Display type: Positive Transmissive
- Backlight: LED/5.0V(Y/G)
- Viewing direction: 6 o’clock
- Operating temperature: 0 to 50°C
- Storage temperature: -10 to 60°C
- Controller: SPLC780D or Equivalence 
- Driving voltage: Single power 
- Driving method: 1/16 duty, 1/5 bias 
- Type: COB (Chip On Board) 
- Number of data line: 6800 4/8-bit parallel
- Connector: PIN

### Driver
The SPLC780D, a dot-matrix LCD controller and driver from SUNPLUS, is a unique design for displaying alpha-numeric,
Japanese-Kana characters and symbols. The SPLC780D provides two types of interfaces to MPU: 4-bit and 8-bit interfaces.
The transferring speed of 8-bit is twice faster than 4-bit.

Source: [Data Sheets](https://github.com/michelheil/Arduino/lib/myLCD1602/datasheet)

### Pictures of LCD
<img src="https://github.com/michelheil/Arduino/blob/master/lib/myLCD1602/pictures/LCD1602_Front.jpg" width="250"> <img src="https://github.com/michelheil/Arduino/blob/master/lib/myLCD1602/pictures/LCD1602_Back.jpg" width="250">


## Library Documentation
### Dependencies
* <avr/io.h>
* <util/delay.h>
* <stdlib.h>
* "myGlobalDefines"

### Init function
Initializing the LCD following the instruction in data sheet

```c
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
```


### APIs
Clear display (which implicitly sets cursor to home)

```void LCD_clearDisplay(void);```

Display a const string to current cursor position 

```void LCD_sendDataString(const char *data);```

Display an unsigned char as string to current cursor position 

```void LCD_sendDataUint(unsigned char *data);```

Display unsigned int to the current cursor position

```void LCD_sendDataUint16(uint16_t val);```

Display a floating value with two decimals to current cursor position 

```void LCD_sendDataFloat(float val);```

Set cursor of the LCD to first position ("home")

```void LCD_setCursorHome(void);```

Set the cursor based on the column and the row

```void LCD_setCursorTo(uint8_t col, uint8_t row);```


#### Helper Functions
Creates the start signal for reading or writing data

```static void LCD_startReadWriteSignal(void)```

Send the four high bits of a byte through 4-bit LCD_PORT without affecting other LCD_PORT pins

```static void LCD_sendHighNibble(uint8_t data)```

Send data byte to LCD

```void LCD_sendDataByte(uint8_t data)```

Send command byte to LCD

```void LCD_sendCommandByte(uint8_t data)```


## Example
Motivation and what to do
### Picture of wiring
Fritzing
### Dependencies
Which other utils are required
