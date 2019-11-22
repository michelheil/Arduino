/*
 * myUSART.h
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
 * Sends unsigned char to TX
 * 
 * data: byte that will be transmitted via USART
 */
void USART_sendChar(unsigned char data);

/*
 * Sends a string to TX
 * 
 * stringPtr: pointer to a char that will be transmitted via USART
 */
void USART_writeString(char* stringPtr);

/*
 * Sends a string to TX followed by a line feed
 * 
 * stringPtr: pointer to a char that will be transmitted via USART
 */
void USART_writeStringLn(char * stringPtr);

/*
 * Sends a float variable to TX
 * 
 * val: float type that will be transmitted via USART
 */
void USART_writeFloat(float val);

/*
 * Receives an unsigned char from RX
 * 
 * returns: unsigned char from the USART data register (UDR0)
 */
unsigned char USART_receiveChar(void);


/*
 * Receives a string from RX
 *
 * stringPtr: Pointer to char for storing the received string
 * 
 * returns: length of received string
 */
unsigned char USART_getString(volatile char * stringPtr);

/*
 * Receives a string from RX and echoes it back to TX
 *
 * stringPtr: Pointer to char for storing the received string
 * 
 * returns: length of received string
 */
unsigned char USART_getStringWithEcho(char * stringPtr);

/*
 * Writes a new line to TX
 */
void USART_newLine();

/*
 * Writes a new head line to TX
 */
void USART_Headline(char * stringPtr);

# endif /* MYUSART_H_ */

#ifdef __cplusplus
}
#endif