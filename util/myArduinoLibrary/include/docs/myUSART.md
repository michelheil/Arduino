## Short Description
The Universal Synchronous and Asynchronous serial Receiver and Transmitter (USART) is a highly flexible
serial communication device.

### Reference to Data Sheet ATMega328P
[Download ATMega328P data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)

Chapter 20. USART0

Features
- Full Duplex Operation (Independent Serial Receive and Transmit Registers)
- Asynchronous or Synchronous Operation
- Master or Slave Clocked Synchronous Operation
- High Resolution Baud Rate Generator
- Supports Serial Frames with 5, 6, 7, 8, or 9 Data Bits and 1 or 2 Stop Bits
- Odd or Even Parity Generation and Parity Check Supported by Hardware
- Data OverRun Detection
- Framing Error Detection
- Noise Filtering Includes False Start Bit Detection and Digital Low Pass Filter
- Three Separate Interrupts on TX Complete, TX Data Register Empty and RX Complete
- Multi-processor Communication Mode
- Double Speed Asynchronous Communication Mode


## Library Documentation

### Dependencies
* <avr/io.h>
* "myGlobalDefines.h"

### Init function
The USART has to be initialized before any communication can take place. The initialization process normally
consists of setting the baud rate, setting frame format and enabling the Transmitter or the Receiver depending
on the usage. For interrupt driven USART operation, the Global Interrupt Flag should be cleared (and interrupts
globally disabled) when doing the initialization.

```c
void USART_init(void)
{
    uint8_t trash; // define variable for initial reading of Data Register (UDR0)
    
	// set baud rate
	// the USART baud rate register consist of 12 bits
	// 4 bits in "H" for high frequencies and
	// 8 bits in "L" for low frequencies
	UBRR0H = (uint8_t) (USART_SETTING >> 8); // move value 8 bits to right as the other 8 bits are stored in "L"
	UBRR0L = (uint8_t) (USART_SETTING);

	// enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    
    // enabled RX Complete Interrupt
    sbi(UCSR0B, RXCIE0);

	// set frame format: asynchronous USART, parity mode disabled, 1stop bit, 8-bit data
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
    
    trash = UDR0;
    (void) trash;
    USART_newLine();
}
```

### APIs
Send unsigned char to TX

```void USART_sendChar(unsigned char data);```

Send a string to TX

```void USART_writeString(char* stringPtr);```

Send a string to TX followed by a line feed

```void USART_writeStringLn(char * stringPtr);```

Send a float variable to TX

```void USART_writeFloat(float val);```

Receive an unsigned char from RX

```unsigned char USART_receiveChar(void);```

Receive a string from RX

```unsigned char USART_getString(volatile char * stringPtr);```

Receive a string from RX and echoes it back to TX

```unsigned char USART_getStringWithEcho(char * stringPtr);```

Write a new line to TX

```void USART_newLine();```

Write a new head line to TX

```void USART_Headline(char * stringPtr);```


### Helper Functions
N/A
