## Used Features
- read analog input pin
- write strings and uint16_t to USART
- use Timer/Counter 1A interrupt (TIMER1_COMPA_vect)
- calibrate analog input values

## Used Libraries
- "globalDefines.h"
- avr/io.h
- avr/interrupt.h // includes cli(), sei(), ISR(*_vector)
- stdio.h
- stdlib.h
- util/delay.h
- "myADC.h"
- "myUSART.h"
