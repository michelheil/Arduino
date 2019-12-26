/*
 * Example for using library INA219
 * Created: 26.12.2019 22:23:18
 * Author : Michael
 */ 

#include "myGlobalDefines.h"

#include <util/delay.h>
#include "myINA219.h"

#include "myUSART.h"

int main(void)
{
  // Initialize Serial (USART)
  USART_init();

  while(1)
  {
    _delay_ms(2000);
    USART_newLine();
  }
}
