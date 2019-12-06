/*
 * ArduinoStarterKitProject2.c
 *
 * Created: 31.08.2019 19:49:33
 * Author : Michael
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRD = (1 << DDD3) | (1 << DDD4) | (1 << DDD5); // 0b00111000
	
    while (1) {
		
		if(PIND & (1 << PD2)) {
			PORTD = 0x00 | (1 << PD3);
			_delay_ms(1000);
			PORTD = 0x00 | (1 << PD4);
			_delay_ms(1000);
		} else {
			PORTD = 0x00 | (1 << PD5);
		}
    }
}
