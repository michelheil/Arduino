/*
 * Example for using library TCS3200
 * Created: 21.12.2019 18:23:18
 * Author : Michael
 */ 
#include <Arduino.h>
#include "myGlobalDefines.h"

#include <TimerOne.h>
#include <util/delay.h>

#include "myUSART.h"
#include "myTCS3200.h"

#define TCS3200_S0  PD6 
#define TCS3200_S1  PD5
#define TCS3200_S2  PD4
#define TCS3200_S3  PD3
#define TCS3200_OUT PD2 // INT0

int tcsOutCount = 0; // INT0 counter; hits on all rising edges
void TSC3200_count(void); // callback function that is called in Service Routine for INT0

TCS3200 tcs(&tcsOutCount, TCS3200_S0, TCS3200_S1, TCS3200_S2, TCS3200_S3, TCS3200_OUT);

void setup()
{
  USART_init();
  attachInterrupt(0, TSC3200_count, RISING);

  USART_writeStringLn("Start calibration on white background...");
  tcs.calibrate();
  USART_writeStringLn("... calibration done!");
}

void loop()
{
  USART_Headline("Red");
  USART_writeStringLn(int162str(tcs.measureColor(1)));

  USART_Headline("Green");
  USART_writeStringLn(int162str(tcs.measureColor(2)));

  USART_Headline("Blue");
  USART_writeStringLn(int162str(tcs.measureColor(3)));
}

void TSC3200_count(void)
{
  // count rising edges on INT0
  // this reflects the frequency for an applied filter
  tcsOutCount++;
}