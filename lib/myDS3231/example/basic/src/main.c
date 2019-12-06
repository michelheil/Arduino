/*
 * Example for using library DS3231
 * Created: 06.12.2019 18:23:18
 * Author : Michael
 */ 

#include "myGlobalDefines.h"

#include <avr/io.h>
#include <util/delay.h>
#include "myDS3231.h"

#include "myUSART.h"

int main(void)
{
  // Initialize Serial (USART)
  USART_init();

  // Initialize TWI on Arduino such taht DS3231 can be used
  DS3231_init();

  while(1)
  {
    // display day-month-year
    char dmy[11];
    DS3231_getDMYString(&dmy[0]);
    USART_writeString("Date: ");
    USART_writeString(&dmy[0]);
    
    // display time
    char time[9];
    DS3231_getTimeString(&time[0]);
    USART_writeString("; Time: ");
    USART_writeString(&time[0]);

    _delay_ms(2000);
    USART_newLine();
  }
}
