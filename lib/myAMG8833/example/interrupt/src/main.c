/*
 * Example for using library myAMG8833
 * Created: 06.12.2019 18:23:18
 * Author : Michael
 */ 

#include "myGlobalDefines.h"

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "myAMG8833.h"

#include "myUSART.h"

// Define Interrupt level for AMG8833
// Interrupt flag  -This flag indicates whether Interrupt is generated  or not when INT control register is activated.
// High (VDD) : Interrupt is not generated
// Low (0V)   : Interrupt is generated
//
// Interrupt value
// 1 LSB has 12 bit resolution (sign + 11 bit) which is equivalent to 0.25 Celsius and it is indicated as two's complement form.
// Example 1: 30 degree Celcius => 30*4 = 120 => LOW: 0b01111000, HIGH: 0b00000000
// Example 2: 25 degree Celcius => 25*4 = 100 => LOW: 0b01100100, HIGH: 0b00000000
#define AMG8833_INT_UPPER_LEVEL_LOW  0b01111000 // 120 => 30 degree Celcius
#define AMG8833_INT_UPPER_LEVEL_HIGH 0b00000000 // positive sign

// define global variables to collect input string through Interrupt Service Routine (ISR)
volatile uint8_t thermInterruptFlag = 0;

int main(void)
{
  // deactivate global Interrupts during initialization phase
  cli();

  USART_init();

  float amgGrid[AMG8833_GRID_PIXELS_X][AMG8833_GRID_PIXELS_Y];
      
  // Initialize AMG8833 device
  AMG8833_init(AMG8833_PCTL_NORMAL_MODE, AMG8833_RST_INITIAL_RESET, AMG8833_FPSC_10FPS, AMG8833_INTC_INTEN_REACTIVE);
      
  // activate moving average
  AMG8833_setMovingAverage(1);
  
  // activate Interrupt with upper limit
  AMG8833_setInterruptUpperLimit(AMG8833_INT_UPPER_LEVEL_HIGH, AMG8833_INT_UPPER_LEVEL_LOW);

  // define Input and Output pins
  cbi(DDRD, PD3); // set as input pin connected to INT from AMG8833 (actually not required as INT1 is activated)
  
  // Enabled INT0 (PD2) and INT1 (PD3) interrupts
  EIMSK = (1 << INT1);
  
  // The rising edge of INT0 (PD2) generates an interrupt request.
  // The falling edge of INT1 generates an interrupt 
  EICRA = (1 << ISC11) | (0 << ISC10);
      
  // activate global interrupt flag
  sei();

  while(1)
  {
    // AMG Interrupt (Temperature)
    if(thermInterruptFlag == 1) 
    { 
      sbi(PORTD, PD4); // indicate interrupt action

      AMG8833_readGrid(&amgGrid[0][0]); // read AMG8833 grid values

      // iterate through all pixels and (a) calculate maximum value and (b) concatenate values to string
      for(int row = 0; row < AMG8833_GRID_PIXELS_X; row++) {
        char buff[50] = "";
        for(int col = 0; col < AMG8833_GRID_PIXELS_Y; col++) {
          strcat(&buff[0], float2str(amgGrid[row][col]));
          if( !(col == (AMG8833_GRID_PIXELS_Y - 1)) ) strcat(&buff[0], ";");
        }
        USART_writeStringLn(&buff[0]);
        buff[0] = 0; // reset buffer for grid string output
      }

      cbi(PORTD, PD4); // indicate end of interrupt action
      
      thermInterruptFlag = 0; // reset interrupt flag   
    }
  }
}

// AMG Interrupt (Temperature)
ISR(INT1_vect)
{
    thermInterruptFlag = 1;
}
