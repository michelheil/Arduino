/*
    Programm to use hardware USART of Arduino Uno (ATMega328P)
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
 * Created: 03.09.2019 22:50:56
 *  Author: Michael
 */ 
#ifdef __cplusplus
extern "C" {
#endif

# ifndef MYUSART_H_
# define MYUSART_H_

#include <avr/io.h> // required for uint types

#define USART_MAX_INPUT_STRING_LENGTH 64

/*
 * Initialize USART
 * 
 * Set Baud Rate defined in global constant variables
 * Enable receiver (RX) and transmitter (TX)
 * Enable RX Complete Interrupt
 * Set frame format:
 *   * asynchronous USART
 *   * parity mode disabled
 *   * 1stop bit
 *   * 8-bit data
 */
void USART_init(void);

/*
 * Send unsigned char to TX
 * 
 * data: byte that will be transmitted via USART
 */
void USART_sendChar(unsigned char data);

/*
 * Send a string to TX
 * 
 * stringPtr: pointer to a char that will be transmitted via USART
 */
void USART_writeString(char* stringPtr);

/*
 * Send a string to TX followed by a line feed
 * 
 * stringPtr: pointer to a char that will be transmitted via USART
 */
void USART_writeStringLn(char * stringPtr);

/*
 * Send a float variable to TX
 * 
 * val: float type that will be transmitted via USART
 */
void USART_writeFloat(float val);

/*
 * Receive an unsigned char from RX
 * 
 * returns: unsigned char from the USART data register (UDR0)
 */
unsigned char USART_receiveChar(void);


/*
 * Receive a string from RX
 *
 * stringPtr: Pointer to char for storing the received string
 * 
 * returns: length of received string
 */
unsigned char USART_getString(volatile char * stringPtr);

/*
 * Receive a string from RX and echoes it back to TX
 *
 * stringPtr: Pointer to char for storing the received string
 * 
 * returns: length of received string
 */
unsigned char USART_getStringWithEcho(char * stringPtr);

/*
 * Write a new line to TX
 */
void USART_newLine();

/*
 * Write a new head line to TX
 */
void USART_Headline(char * stringPtr);

# endif /* MYUSART_H_ */

#ifdef __cplusplus
}
#endif
