/*
 * LoopingLouie.c
 *
 * Created: 18.09.2011 17:26:48
 *  Author: Michael
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int LEDSchalten = 0;

ISR(TIM0_OVF_vect)
{
	static int Zaehler = 0;
	
	Zaehler++;
	
	if (Zaehler == 31)
	{
		Zaehler = 0;
		PORTA ^= (1<<PA7);
	}
	
}



int main(void)
{
    int Taste = 0;
	
	//Dip-Switch PA2
	//Taste PB1
	
	DDRA |= (1<<DDA7); //PA7 = Ausgang
	PORTA &= ~(1<<PA7); //LED aus
	
	TCCR0A |= (1<<WGM01)|(1<<WGM00); //Timer 0 im Fast CTC Mode
	TCCR0B |= (1<<CS02 )|(1<<CS00 );  //Timer 0 mit 8MHz/1024
	TIMSK0 |= (1<<TOIE0);
	
	sei();
	
	
	while(1)
    {
        if ((PINB & (1<<PINB1)) == 0)
		{
		  if (Taste == 0)
		  {
		    PORTA ^= (1<<PA7);	
			Taste = 1;			  
		  }
		}
		else
		  Taste = 0;	  
    }
}