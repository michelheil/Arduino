## Used Features
- read analog input pin
- write strings and uint16_t to USART
- use 16-bit PWM to steer motor
- use H-Bruecke to run motor
- use Pin Change interrupt (PCINT0_vect)
- calibrate analog input values

## Used Libraries
- "globalDefines.h"
- avr/io.h
- avr/interrupt.h // includes cli(), sei(), ISR(*_vector)
- stdintio.h
- util/delay.h
- "myADC.h"
- "myUSART.h"
- "myVariableTypes.h"
