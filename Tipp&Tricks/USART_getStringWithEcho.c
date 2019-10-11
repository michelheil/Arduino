/*
 * USART_receive.c
 *
 * Created: 11.10.2019 17:15:55
 * Author : Michael
 */ 

#include <avr/io.h>

#include "myUSART.h"


int main(void)
{
    char userInput[USART_MAX_INPUT_STRING_LENGTH+1];
    
    USART_init();
    USART_newLine();
    
    USART_writeStringLn("Hallo Bitte gib einen Text ein:");
    
    /* Replace with your application code */
    while (1) 
    {
        USART_getStringWithEcho(&userInput[0]);
    }
}

