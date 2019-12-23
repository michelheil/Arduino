/*
 * Example for using library TCS3200
 * Created: 21.12.2019 18:23:18
 * Author : Michael
 */ 
#include <Arduino.h>
#include "myGlobalDefines.h"

#include <TimerOne.h>
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

struct rgb tscColors;
int tscOutCount = 0; // INT0 counter; hits on all rising edges
int colorFilterSwitch = 0; // color filter switch; switches every time the Timer hits the compare value

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
  H   H   100%
  */
  cbi(PORTD, TCS3200_S0);
  sbi(PORTD, TCS3200_S1);
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



void TSC_WB(int s2, int s3)
{
  // reset counter of interrupt INT0
  tscOutCount = 0;

  // increment switch color selection flag
  colorFilterSwitch++;

  // select color
  TSC3200_colorSelection(s2, s3);

  // set timer to 1 second
  // during that time the INT0 interrupt will use 
  // tscOutCount to count rising edges
  Timer1.setPeriod(1000000);
}



void TSC_Callback()
{
  switch(colorFilterSwitch)
  {
    case 0:
      Serial.println("->WB Start");
      TSC_WB(LOW, LOW); // select red
      break;
    case 1:
      Serial.print("->Frequency R=");
      Serial.println(tscOutCount);
      tscColors.red = tscOutCount;
      TSC_WB(HIGH, HIGH); // select green
      break;
    case 2:
      Serial.print("->Frequency G=");
      Serial.println(tscOutCount);
      tscColors.green = tscOutCount;
      TSC_WB(LOW, HIGH); // select blue
      break;
    case 3:
      Serial.print("->Frequency B=");
      Serial.println(tscOutCount);
      Serial.println("->WB End");
      tscColors.blue = tscOutCount;
      TSC_WB(HIGH, LOW); // no filter
      break;
    default:
      tscOutCount = 0;
      break;
  }
}




void setup()
{
  TSC3200_init();
  Serial.begin(9600);
  Timer1.initialize();
  Timer1.attachInterrupt(TSC_Callback);
  attachInterrupt(0, TSC3200_count, RISING);
  delay(4000);
  Serial.println(tscColors.red);
  Serial.println(tscColors.green);
  Serial.println(tscColors.blue);

  // calibration on white background
  tscColors.redCalibFact = 255.0 / tscColors.red; // R-Wert
  tscColors.greenCalibFact = 255.0 / tscColors.green; // G-Wert
  tscColors.blueCalibFact = 255.0 / tscColors.blue; // B-Wert
  Serial.println(tscColors.redCalibFact);
  Serial.println(tscColors.greenCalibFact);
  Serial.println(tscColors.blueCalibFact);
}

void loop()
{
  colorFilterSwitch = 0;
  Serial.println(int(tscColors.red * tscColors.redCalibFact));
  Serial.println(int(tscColors.green * tscColors.greenCalibFact));
  Serial.println(int(tscColors.blue * tscColors.blueCalibFact));
  delay(4000);
}