/*
 * Example for using library TCS3200
 * Created: 21.12.2019 18:23:18
 * Author : Michael
 */ 
#include "myGlobalDefines.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "myUSART.h"
#include "myTCS3200.h"

#define TCS3200_S0  PD6 
#define TCS3200_S1  PD5
#define TCS3200_S2  PD4
#define TCS3200_S3  PD3
#define TCS3200_OUT PD2 // INT0

volatile int tcsOutCount = 0; // INT0 counter; hits on all rising edges

int main(void)
{
  // deactivate global Interrupts during initialization phase
  cli();

  // initialize USART
  USART_init();

  // construct TCS320 object
  TCS3200 tcs(&tcsOutCount, TCS3200_S0, TCS3200_S1, TCS3200_S2, TCS3200_S3, TCS3200_OUT);
  
  // Enable INT0 (PD2) interrupts
  EIMSK = (1 << INT0);
  
  // The rising edge of INT0 (PD2) generates an interrupt request.
  EICRA = (1 << ISC01) | (1 << ISC00);
      
  // activate global interrupt flag
  sei();

  USART_writeStringLn("Start calibration on white background...");
  tcs.calibrate();
  USART_writeStringLn("... calibration done!");

  while(1)//void loop()
  {
    USART_Headline("Red");
    USART_writeStringLn(int162str(tcs.measureColor(1)));

    USART_Headline("Green");
    USART_writeStringLn(int162str(tcs.measureColor(2)));

    USART_Headline("Blue");
    USART_writeStringLn(int162str(tcs.measureColor(3)));
  }
}


ISR(INT0_vect)
{
  // count rising edges on INT0
  // this reflects the frequency for an applied filter
  tcsOutCount++;
}