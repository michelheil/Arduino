/*
 * Example for using library TCS3200
 * Created: 21.12.2019 18:23:18
 * Author : Michael
 */ 

#include <Arduino.h>
#include "myGlobalDefines.h"

#include <TimerOne.h>
#include <util/delay.h>

#define SET_BIT_LEVEL(PORT, bit, level) ( ((level) == 0) ? (cbi(PORT, bit)) : (sbi(PORT, bit)) )

#define TCS3200_S0  PD6 
#define TCS3200_S1  PD5
#define TCS3200_S2  PD4
#define TCS3200_S3  PD3
#define TCS3200_OUT PD2 // INT0

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


void TSC3200_colorSelection(int Level01, int Level02)
{
  /*
  S2  S3  Photodiode Type
  L   L   Red
  L   H   Blue
  H   L   Clear (no filter)
  H   H   Green
  */
  SET_BIT_LEVEL(PORTD, TCS3200_S2, Level01);
  SET_BIT_LEVEL(PORTD, TCS3200_S3, Level02);
}

void TSC3200_count()
{
  // count rising edges on INT0
  // this reflects the frequency for an applied filter
  tscOutCount ++;
}



void TSC_WB(int Level0, int Level1)
{
  // set INTO Interrupt counter to 0
  tscOutCount = 0;

  // ensure that next time another color filter is applied
  colorFilterSwitch ++;

  // select color filter
  TSC3200_colorSelection(Level0, Level1);

  // count for 1 second (= 1000000 microsends)
  // during that time the INT0 interrupt will use 
  // g_count to count rising edges
  Timer1.setPeriod(1000000);
}


void TSC_Callback()
{
  switch(colorFilterSwitch)
  {
    case 0:
      Serial.println("->WB Start");
      TSC_WB(LOW, LOW); // Filter Rot
      Serial.print("->Frequency R=");
      Serial.println(tscOutCount);
      tscColors.red = tscOutCount;
      break;
    case 1:
      TSC_WB(HIGH, HIGH); // Filter Grün
      Serial.print("->Frequency G=");
      Serial.println(tscOutCount);
      tscColors.green = tscOutCount;
      break;
    case 2:
      TSC_WB(LOW, HIGH); // Filter Blau
      Serial.print("->Frequency B=");
      Serial.println(tscOutCount);
      Serial.println("->WB End");
      tscColors.blue = tscOutCount;
      break;
    case 3:
      TSC_WB(HIGH, LOW); // Kein Filter
      break;
    default:
      tscOutCount = 0;
      break;
  }
}

int main(void)
{
  TSC3200_init();
  Serial.begin(9600);
  Timer1.initialize();
  Timer1.attachInterrupt(TSC_Callback);
  attachInterrupt(0, TSC3200_count, RISING); // count rising edges on INTO (Pin 0)
  _delay_ms(4000);
  Serial.println(tscColors.red);
  Serial.println(tscColors.green);
  Serial.println(tscColors.blue);

  // calibration on white background
  tscColors.redCalibFact = 255.0/ tscColors.red; // R-Wert (calibration factor, such that 255 is maximum)
  tscColors.greenCalibFact = 255.0/ tscColors.green; // G-Wert
  tscColors.blueCalibFact = 255.0/ tscColors.blue; // B-Wert
  Serial.println(tscColors.redCalibFact);
  Serial.println(tscColors.greenCalibFact);
  Serial.println(tscColors.blueCalibFact);

  while(1)
  {
    colorFilterSwitch = 0;
    _delay_ms(4000);
    Serial.println(int(tscColors.redCalibFact * tscColors.red));
    Serial.println(int(tscColors.greenCalibFact * tscColors.green));
    Serial.println(int(tscColors.blueCalibFact * tscColors.blue));
  }
}
