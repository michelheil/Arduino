/*
 * Example for using library TCS3200
 * Created: 21.12.2019 18:23:18
 * Author : Michael
 */ 

#include <Arduino.h>
#include "myGlobalDefines.h"

#include <TimerOne.h>
#define S0 6
#define S1 5
#define S2 4
#define S3 3
#define OUT 2

int g_count = 0;
int g_array[3];
int g_flag = 0;
float g_SF[3];
void TSC_Init()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  digitalWrite(S0, LOW);
  digitalWrite(S1, HIGH);
}


void TSC_FilterColor(int Level01, int Level02)
{
  if(Level01 != 0)
  Level01 = HIGH;

  if(Level02 != 0)
  Level02 = HIGH;

  digitalWrite(S2, Level01);
  digitalWrite(S3, Level02);
}

void TSC_Count()
{
  g_count ++ ;
}



void TSC_WB(int Level0, int Level1)
{
  g_count = 0;
  g_flag ++;
  TSC_FilterColor(Level0, Level1);
  Timer1.setPeriod(1000000);
}



void TSC_Callback()
{
  switch(g_flag)
  {
    case 0:
      Serial.println("->WB Start");
      TSC_WB(LOW, LOW); // Filter ohne Rot
      break;
    case 1:
      Serial.print("->Frequency R=");
      Serial.println(g_count);
      g_array[0] = g_count;
      TSC_WB(HIGH, HIGH); // Filter ohne Grün
      break;
    case 2:
      Serial.print("->Frequency G=");
      Serial.println(g_count);
      g_array[1] = g_count;
      TSC_WB(LOW, HIGH); // Filter ohne Blau
      break;
    case 3:
      Serial.print("->Frequency B=");
      Serial.println(g_count);
      Serial.println("->WB End");
      g_array[2] = g_count;
      TSC_WB(HIGH, LOW); // Kein Filter
      break;
    default:
      g_count = 0;
      break;
  }
}




void setup()
{
  TSC_Init();
  Serial.begin(9600);
  Timer1.initialize();
  Timer1.attachInterrupt(TSC_Callback);
  attachInterrupt(0, TSC_Count, RISING);
  delay(4000);
  for(int i=0; i<3; i++)
    Serial.println(g_array[i]);

  // calibration on white background
  g_SF[0] = 255.0/ g_array[0]; //R-Wert (calibration factor, such that 255 is maximum)
  g_SF[1] = 255.0/ g_array[1] ; //G-Wert
  g_SF[2] = 255.0/ g_array[2] ; //B-Wert
  Serial.println(g_SF[0]);
  Serial.println(g_SF[1]);
  Serial.println(g_SF[2]);
}

void loop()
{
  g_flag = 0;
  for(int i=0; i<3; i++)
    Serial.println(int(g_array[i] * g_SF[i]));
  delay(4000);
}






























/*
#include <util/delay.h>
#include "myTCS3200.h"

#include "myUSART.h"

int main(void)
{
  // Initialize Serial (USART)
  USART_init();

  pinMode

  while(1)
  {
    _delay_ms(2000);
    USART_newLine();
  }
}
*/