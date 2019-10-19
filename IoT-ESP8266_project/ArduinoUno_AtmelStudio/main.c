/*
 * IoT_ESP8266.c
 *
 * Created: 28.09.2019 15:23:18
 * Author : Michael
 */ 

#include "globalDefines.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "myUSART.h"


int main(void)
{
    // initialize USART with baud rate 9600
    USART_init();
    USART_writeStringLn("TEST");
    
    // push button at Pin PD2 as input in Data Direction Register
    cbi(DDRD,PD2);
    
    while(1)
    {
        if (PIND & (1 << PD2)) { // push button is not pressed
            USART_writeStringLn("Send"); // send string
            _delay_ms(1200); // do nothing
        }
        else { // when push button is pressed the write SEND to USART
            _delay_ms(20);
        }
    }

}

