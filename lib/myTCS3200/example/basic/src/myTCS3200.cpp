/*
 * myTCS3200.cpp
 *
 * Created: 21.12.2019 10:50:07
 *  Author: Michael
 */ 

#include "myTCS3200.h"
#include "myGlobalDefines.h"
#include <avr/io.h>
#include <util/delay.h>

/**
 * @brief Construct a new TCS3200::TCS3200 object
 * @details Data Direction Registers and Output Frequency Scaling are set
 * Output Frequency Sclaing (f_0)
 * S0  S1  Output Frequency Scaling (f_0)
 * L   L   Power down
 * L   H   2%
 * H   L   20%
 * H   H   100% // Arduino not able to process
 * 
 * @param counter used to measure the color frequencies
 * @param s0p pin S0
 * @param s1p pin S1
 * @param s2p pin S2
 * @param s3p pin S3
 * @param outp pin OUT
 */
TCS3200::TCS3200(int * counter, uint8_t s0p, uint8_t s1p, uint8_t s2p, uint8_t s3p, uint8_t outp):
   interruptCounter(counter), s0Pin(s0p), s1Pin(s1p), s2Pin(s2p), s3Pin(s3p), outPin(outp)
{
  // set data direction registers
  sbi(DDRD, s0Pin);
  sbi(DDRD, s1Pin);
  sbi(DDRD, s2Pin);
  sbi(DDRD, s3Pin);
  cbi(DDRD, outPin);

  // output frequency scaling
  sbi(PORTD, s0Pin);
  cbi(PORTD, s1Pin);
}

/**
 * @brief Destroy the TCS3200::TCS3200 object
 * @details currently not in use
 */
TCS3200::~TCS3200() {}

/**
 * @brief measure and store reference colors
 */
void TCS3200::calibrate(void)
  {
    // store measured ticks per color during calibration
    calibResult.red   = measureTicks(1);
    calibResult.green = measureTicks(2);
    calibResult.blue  = measureTicks(3);

    // calibration on white background
    calibResult.redCalibFact   = (float)max_color_value / (float)calibResult.red;    // R-Wert
    calibResult.greenCalibFact = (float)max_color_value / (float)calibResult.green;  // G-Wert
    calibResult.blueCalibFact  = (float)max_color_value / (float)calibResult.blue;   // B-Wert
  }

/**
 * @brief measures the OUT activities of a color
 * 
 * @param color select color 1=Red, 2=Green, 3=Blue, default=noFilter
 * @return int amount of interrupts calls (OUT pin high) during measurement
 */
int TCS3200::measureTicks(int color) 
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
  return (*interruptCounter);
}

/**
 * @brief measure calibrated color value
 * 
 * @param color select color 1=Red, 2=Green, 3=Blue, default=noFilter
 * @return int calibrated color value
 */
int TCS3200::measureColor(int color) 
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

/**
 * @brief dependent on pins S2 and S3, select color photodiodes for each color
 * @details 
 *  S2  S3  Photodiode Type
 *  L   L   Red
 *  L   H   Blue
 *  H   L   Clear (no filter)
 *  H   H   Green
 *
 * @param s2Level status for pin S2
 * @param s3Level status for pin S3
 */
void TCS3200::colorSelection(int s2Level, int s3Level)
{
  SET_BIT_LEVEL(PORTD, s2Pin, s2Level);
  SET_BIT_LEVEL(PORTD, s3Pin, s3Level);
}
