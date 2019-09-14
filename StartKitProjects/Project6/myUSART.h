/*
 * myUSART.h
 *
 * Created: 03.09.2019 22:50:56
 *  Author: Michael
 */ 


# ifndef MYUSART_H_
# define MYUSART_H_

void USART_init(void);
void USART_sendChar(unsigned char data);
unsigned char USART_receive(void);
void USART_writeString(char* StringPtr);

// helper functions
char* float2str(float floatValue);
char* uint162char(uint16_t uint16Value);
char* uint322char(uint32_t uint32Value);

# endif /* MYUSART_H_ */