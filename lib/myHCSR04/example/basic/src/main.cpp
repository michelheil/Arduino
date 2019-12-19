#include "myGlobalDefines.h"
#include <util/delay.h>

#include "myTC16.h"
#include "myHCSR04.h"
#include "myUSART.h"

// Attention:
// DDRD, DDRB, PORTD, and PINB are hard coded in HCSR04 class
#define HCSR04_TRIGGER_PIN PD4
#define HCSR04_ECHO_PIN PB0

int main(void)
{
  // Initialize USART
  USART_init();

  // Initialize Timer/Counter 16-bit and store tickPerSecond conversion factor
  float tc16_ticksPerSecond = TC16_init(1024);

  // Initialize sensor by specifying the trigger and echo pins
  HCSR04 myHCSR04(HCSR04_TRIGGER_PIN, HCSR04_ECHO_PIN, tc16_ticksPerSecond);

  while(1)
  {
    _delay_ms(100);

    float distanceInCm = myHCSR04.measureDistanceInCm(80);
    USART_writeString("Distance in Centimeters: ");
    USART_writeFloat(distanceInCm);
    USART_newLine();
  }
}

