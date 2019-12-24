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

#define TCS3200_S0  PD6 
#define TCS3200_S1  PD5
#define TCS3200_S2  PD4
#define TCS3200_S3  PD3
#define TCS3200_OUT PD2 // INT0

#define SET_BIT_LEVEL(PORT, bit, level) ( ((level) == 0) ? (cbi(PORT, bit)) : (sbi(PORT, bit)) )


struct rgb {

  // stores the tscOutCount for each color (red, green, blue)
  int red;
  int green;
  int blue;

  // calibration factors for each color (red, green, blue)
  float redCalibFact;
  float greenCalibFact;
  float blueCalibFact;
};

int tscOutCount = 0; // INT0 counter; hits on all rising edges
struct rgb tscColors;
int newRed, newGreen, newBlue;

void TSC3200_init(void);
void TSC3200_colorSelection(int s2, int s3);
void TSC3200_count();
int TCS3200_measureColor(int color);


void setup()
{
  TSC3200_init();
  USART_init();

  attachInterrupt(0, TSC3200_count, RISING);
  USART_writeStringLn("Start calibration on white background");
  tscColors.red   = TCS3200_measureColor(1);
  tscColors.green = TCS3200_measureColor(2);
  tscColors.blue  = TCS3200_measureColor(3);
  USART_writeStringLn(int162str(tscColors.red));
  USART_writeStringLn(int162str(tscColors.green));
  USART_writeStringLn(int162str(tscColors.blue));

  // calibration on white background
  tscColors.redCalibFact    = 255.0f  / tscColors.red;    // R-Wert
  tscColors.greenCalibFact  = 255.0f  / tscColors.green;  // G-Wert
  tscColors.blueCalibFact   = 255.0f  / tscColors.blue;   // B-Wert
  USART_writeFloat(tscColors.redCalibFact); USART_newLine();
  USART_writeFloat(tscColors.greenCalibFact); USART_newLine();
  USART_writeFloat(tscColors.blueCalibFact); USART_newLine();
}
void loop()
  {
    newRed = TCS3200_measureColor(1);
    USART_Headline("Red");
    USART_writeStringLn(int162str(newRed));
    USART_writeFloat(newRed * tscColors.redCalibFact);

    newGreen = TCS3200_measureColor(2);
    USART_Headline("Green");
    USART_writeStringLn(int162str(newGreen));
    USART_writeFloat(newGreen * tscColors.greenCalibFact);

    newBlue = TCS3200_measureColor(3);
    USART_Headline("Blue");
    USART_writeStringLn(int162str(newBlue));
    USART_writeFloat(newBlue * tscColors.blueCalibFact);

    _delay_ms(1000);
  }


void TSC3200_init(void)
{
  // set data direction registers
  sbi(DDRD, TCS3200_S0);
  sbi(DDRD, TCS3200_S1);
  sbi(DDRD, TCS3200_S2);
  sbi(DDRD, TCS3200_S3);
  cbi(DDRD, TCS3200_OUT);

  /* Output Frequency Sclaing (f_0)
  S0  S1  Output Frequency Scaling (f_0)
  L   L   Power down
  L   H   2%
  H   L   20%
  H   H   100% // Arduino not able to process
  */
  sbi(PORTD, TCS3200_S0);
  cbi(PORTD, TCS3200_S1);
}


void TSC3200_colorSelection(int s2, int s3)
{
  /*
  S2  S3  Photodiode Type
  L   L   Red
  L   H   Blue
  H   L   Clear (no filter)
  H   H   Green
  */
  SET_BIT_LEVEL(PORTD, TCS3200_S2, s2);
  SET_BIT_LEVEL(PORTD, TCS3200_S3, s3);
}

void TSC3200_count()
{
  // count rising edges on INT0
  // this reflects the frequency for an applied filter
  tscOutCount ++;
}


int TCS3200_measureColor(int color) // 1=Red, 2=Green, 3=Blue, default=noFilter
{
  // re-set count to zero, to start new measurement
  tscOutCount = 0;

  // select color
  switch(color)
  {
    case 1: // red
      TSC3200_colorSelection(LOW, LOW);
      break;
    case 2: // green
      TSC3200_colorSelection(HIGH, HIGH);
      break;
    case 3: // blue
      TSC3200_colorSelection(LOW, HIGH);
      break;
    default: // no filter
      TSC3200_colorSelection(HIGH, LOW);
  }
  

  // measure for 1 second
  _delay_ms(1000);

  // return amount of rising edges of OUT pin
  return tscOutCount;
}
