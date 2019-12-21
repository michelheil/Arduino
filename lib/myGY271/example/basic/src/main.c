/*
 * Example for using library GY271
 * Created: 20.12.2019 18:23:18
 * Author : Michael
 */ 

#include "myGlobalDefines.h"

#include <util/delay.h>

#include "myTWI.h"
#include "myUSART.h"
#include "myGY271.h"

int main(void)
{
  struct xyzInt16Values xyzGY271;

  // Initialize Serial (USART)
  USART_init();

  // Initialiye TWI communication (default 400kHz)
  TWI_init();
  
  // Initialize GY271 module by doing software reset and setting operating mode
  GY271_init();

  while(1)
  {
    xyzGY271 = GY271_getThreeAxisValues();

    USART_writeString("(x,y,z) Values: ");
    USART_writeString(int162str(xyzGY271.x)); USART_writeString(" ");
    USART_writeString(int162str(xyzGY271.y)); USART_writeString(" ");
    USART_writeString(int162str(xyzGY271.z)); USART_writeStringLn(" ");

    _delay_ms(1000);
  }
}
