/*
 * myTCS3200.c
 *
 * Created: 21.12.2019 10:50:07
 *  Author: Michael
 */ 

#include "myTCS3200.h"
#include "myGlobalDefines.h"
#include <avr/io.h>
#include <util/delay.h>


TCS3200::TCS3200(int * counter, uint8_t s0p, uint8_t s1p, uint8_t s2p, uint8_t s3p, uint8_t outp):
   interruptCounter(counter), s0Pin(s0p), s1Pin(s1p), s2Pin(s2p), s3Pin(s3p), outPin(outp)
{
  // set data direction registers
  sbi(DDRD, s0Pin);
  sbi(DDRD, s1Pin);
  sbi(DDRD, s2Pin);
  sbi(DDRD, s3Pin);
  cbi(DDRD, outPin);

  /* Output Frequency Sclaing (f_0)
  S0  S1  Output Frequency Scaling (f_0)
  L   L   Power down
  L   H   2%
  H   L   20%
  H   H   100% // Arduino not able to process
  */
  sbi(PORTD, s0Pin);
  cbi(PORTD, s1Pin);
}

TCS3200::~TCS3200() {}

void TCS3200::calibrate(void)
  {
    calibResult.red   = measureTicks(1);
    calibResult.green = measureTicks(2);
    calibResult.blue  = measureTicks(3);

    // calibration on white background
    calibResult.redCalibFact   = (float)max_color_value / (float)calibResult.red;    // R-Wert
    calibResult.greenCalibFact = (float)max_color_value / (float)calibResult.green;  // G-Wert
    calibResult.blueCalibFact  = (float)max_color_value / (float)calibResult.blue;   // B-Wert
  }


int TCS3200::measureTicks(int color) // 1=Red, 2=Green, 3=Blue, default=noFilter
{
  // select color
  switch(color)
  {
    case 1: // red
      colorSelection(0, 0);
      break;
    case 2: // green
      colorSelection(1, 1);
      break;
    case 3: // blue
      colorSelection(0, 1);
      break;
    default: // no filter
      colorSelection(1, 0);
  }

  // re-set global counter to zero; to prepare new measurement
  *interruptCounter = 0;
  
  // measure for one second
  _delay_ms(1000);

  // return amount of rising edges of OUT pin 
  // multiplied with calibration factor.
  return (*interruptCounter);
}

int TCS3200::measureColor(int color) // 1=Red, 2=Green, 3=Blue, default=noFilter
{
  switch(color)
  {
    case 1: // red
      redTicks = measureTicks(color);
      return (int)(redTicks * calibResult.redCalibFact);
      break;
    case 2: // green
      greenTicks = measureTicks(color);
      return (int)(greenTicks * calibResult.greenCalibFact);
      break;
    case 3: // blue
      blueTicks = measureTicks(color);
      return (int)(blueTicks * calibResult.blueCalibFact);
      break;
    default: // no filter
      return 255;
  }
}

void TCS3200::colorSelection(int s2Level, int s3Level)
{
  /*
  S2  S3  Photodiode Type
  L   L   Red
  L   H   Blue
  H   L   Clear (no filter)
  H   H   Green
  */
  SET_BIT_LEVEL(PORTD, s2Pin, s2Level);
  SET_BIT_LEVEL(PORTD, s3Pin, s3Level);
}
