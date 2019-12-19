#include "myGlobalDefines.h"
#include <util/delay.h>

#include "myHCSR04.h"
#include "myTC16.h"

#include "myUSART.h"

#define HCSR04_TRIGGER_PIN PD4
#define HCSR04_ECHO_PIN PB0

int main(void)
{
  // define sensor class
  HCSR04 myHCSR04;

  // Initialize USART
  USART_init();

  // Initialize Timer/Counter 16-bit and store tickPerSecond conversion factor
  float tc16_ticksPerSecond = TC16_init();

  // Initialiye sensor by specifying the trigger and echo pins
  myHCSR04.init(HCSR04_TRIGGER_PIN, HCSR04_ECHO_PIN, tc16_ticksPerSecond);

  while(1)
  {
    _delay_ms(1000);

    float distanceInCmFromClass = myHCSR04.measureDistanceInCm();
    USART_writeString("Distance in Centimeters: ");
    USART_writeFloat(distanceInCmFromClass);
    USART_newLine();
  }
}

