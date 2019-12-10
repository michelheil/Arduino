/**
 * @page USART_page myUSART.h
 * @brief Programm to apply USART functionality of ATMega328P
 * 
 * @date 09.10.2019 18:41:08
 * @author Michael
 * @copyright GNU General Public License
 */

#ifdef __cplusplus
extern "C" {
#endif

# ifndef MYUSART_H_
# define MYUSART_H_

#include <avr/io.h> // required for uint types

#define USART_MAX_INPUT_STRING_LENGTH 64 /**< Limit for receiving bytes over UART (TX) */

/**
 * @brief Initialize USART functionality on ATMega328P
 * 
 * @details The following steps are handled during initialisation
 * Set Baud Rate defined in global constant variables @n
 * Enable receiver (RX) and transmitter (TX) @n
 * Enable RX Complete Interrupt @n
 * Set frame format: @n
 *   @li asynchronous USART
 *   @li parity mode disabled
 *   @li 1stop bit
 *   @li 8-bit data
 */
void USART_init(void);

/**
 * @brief Send unsigned char to TX
 * 
 * @param data byte that will be transmitted via USART
 */
void USART_sendChar(unsigned char data);

/**
 * @brief Send a string to TX
 * 
 * @param stringPtr pointer to a char that will be transmitted via USART
 */
void USART_writeString(char const * stringPtr);

/**
 * @brief Send a string to TX followed by a line feed
 * 
 * @param stringPtr pointer to a char that will be transmitted via USART
 */
void USART_writeStringLn(char const * stringPtr);

/**
 * @brief Send a float variable to TX
 * 
 * @param val float type that will be transmitted via USART
 */
void USART_writeFloat(float val);

/**
 * @brief Receive an unsigned char from RX
 * 
 * returns: unsigned char from the USART data register (UDR0)
 */
unsigned char USART_receiveChar(void);

/**
 * @brief Receive a string from RX
 *
 * @param stringPtr Pointer to char for storing the received string
 * 
 * @return length of received string
 */
unsigned char USART_getString(volatile char * stringPtr);

/**
 * @brief Receive a string from RX and echoes it back to TX
 *
 * @param stringPtr Pointer to char for storing the received string
 * 
 * @return length of received string
 */
unsigned char USART_getStringWithEcho(char * stringPtr);

/**
 * @brief Write a new line to TX
 */
void USART_newLine();

/**
 * @brief Write a new head line to TX
 * 
 * @param stringPtr Pointer to char with content of head line
 */
void USART_Headline(char * stringPtr);

# endif /* MYUSART_H_ */

#ifdef __cplusplus
}
#endif
