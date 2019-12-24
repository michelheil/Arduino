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

#define MAX_COLOR_VALUE (255.0f)

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

int         tscOutCount = 0; // INT0 counter; hits on all rising edges
struct rgb  tscColors;
int         newRed, newGreen, newBlue;

void        TSC3200_init(void); // public
struct rgb  TSC3200_calibrate(void); // public
int         TCS3200_measureColor(int color); // public
void        TSC3200_colorSelection(int s2, int s3); // private
void        TSC3200_count(); // ISR(INT0_vect)


void setup()
{
  TSC3200_init();
  USART_init();

  attachInterrupt(0, TSC3200_count, RISING);
  
  USART_writeStringLn("Start calibration on white background");
  tscColors = TSC3200_calibrate();
  USART_writeStringLn(int162str(tscColors.red));
  USART_writeStringLn(int162str(tscColors.green));
  USART_writeStringLn(int162str(tscColors.blue));
  USART_writeFloat(tscColors.redCalibFact);   USART_newLine();
  USART_writeFloat(tscColors.greenCalibFact); USART_newLine();
  USART_writeFloat(tscColors.blueCalibFact);  USART_newLine();
 
}
void loop()
  {
    USART_Headline("Red");
    newRed = TCS3200_measureColor(1);
    USART_writeStringLn(int162str(newRed));
    USART_writeFloat(newRed * tscColors.redCalibFact);

    USART_Headline("Green");
    newGreen = TCS3200_measureColor(2);
    USART_writeStringLn(int162str(newGreen));
    USART_writeFloat(newGreen * tscColors.greenCalibFact);

    USART_Headline("Blue");
    newBlue = TCS3200_measureColor(3);
    USART_writeStringLn(int162str(newBlue));
    USART_writeFloat(newBlue * tscColors.blueCalibFact);
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

struct rgb TSC3200_calibrate(void)
  {
    struct rgb calibResult;
    calibResult.red   = TCS3200_measureColor(1);
    calibResult.green = TCS3200_measureColor(2);
    calibResult.blue  = TCS3200_measureColor(3);

    // calibration on white background
    calibResult.redCalibFact    = MAX_COLOR_VALUE / calibResult.red;    // R-Wert
    calibResult.greenCalibFact  = MAX_COLOR_VALUE / calibResult.green;  // G-Wert
    calibResult.blueCalibFact   = MAX_COLOR_VALUE / calibResult.blue;   // B-Wert

    return calibResult;
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

  // re-set global counter to zero, to allow new measurement
  tscOutCount = 0;
  
  // measure for c. one second
  _delay_ms(1000);

  // return amount of rising edges of OUT pin.
  // the amount will be incremented in the interrupt INT0 service 
  // routine during the above 1 sec delay.
  return tscOutCount;
}
