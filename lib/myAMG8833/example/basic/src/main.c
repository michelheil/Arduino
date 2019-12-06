/*
 * Example for using library myAMG8833
 * Created: 06.12.2019 18:23:18
 * Author : Michael
 */ 

#include "myGlobalDefines.h"

#include <avr/io.h>
#include <util/delay.h>
#include "myAMG8833.h"

#include "myUSART.h"


int main(void)
{
  // Initialize Serial (USART)
  USART_init();

  // Initialize AMG8833 device
  AMG8833_init(AMG8833_PCTL_NORMAL_MODE, AMG8833_RST_INITIAL_RESET, AMG8833_FPSC_10FPS, AMG8833_INTC_INTEN_REACTIVE);

  while(1)
  {
    // Read AMG8833 Thermistor value
    float amgThermistor = AMG8833_readThermistor();

    // Print Thermistor value to serial monitor
    USART_writeString("Thermistor: ");
    USART_writeFloat(amgThermistor);

    // Read AMG8833 grid values of size 8x8
    float amgGrid[AMG8833_GRID_PIXELS_X][AMG8833_GRID_PIXELS_Y];
    AMG8833_readGrid(&amgGrid[0][0]);

    // Print Pixel[1][1] to serial monitor
    USART_writeString("; Grid Pixel[1][1]: ");
    USART_writeFloat(amgGrid[0][0]);

    // Print Pixel[8][8] to serial monitor
    USART_writeString("; Grid Pixel[8][8]: ");
    USART_writeFloat(amgGrid[7][7]);
    
    _delay_ms(2000);
    USART_newLine();
  }
}
