/*
 * myUSART.h
 *
 * Created: 03.09.2019 22:50:56
 *  Author: Michael
 */ 


# ifndef MYUSART_H_
# define MYUSART_H_

#include "globalDefines.h"

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#define USART_MAX_INPUT_STRING_LENGTH 64

void USART_init(void);
void USART_sendChar(unsigned char data);
void USART_writeString(char* stringPtr);
void USART_writeStringLn(char * stringPtr);
void USART_writeFloat(float val);
unsigned char USART_receiveChar(void);
unsigned char USART_getStringWithEcho(char * stringPtr);
void USART_newLine();
void USART_Headline(char * stringPtr);

// helper functions
char* float2str(float floatValue);
char* uint82str(uint8_t uint8Value);
char* uint162str(uint16_t uint16Value);
char* uint322str(uint32_t uint32Value);

# endif /* MYUSART_H_ */