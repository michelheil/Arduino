/*
 * LoopingLouie.c
 *
 * Created: 18.09.2011 17:26:48
 *  Author: Michael
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char CompareMatch       = 32;
unsigned char FrequenzGewechselt = 0;
unsigned char TasteAbfragen      = 0;
unsigned char Tastegedrueckt     = 0;

ISR(TIM0_OVF_vect)         //32Hz
{
	static unsigned char Zaehler = 0;
	
	if (Tastegedrueckt == 0)
	  TasteAbfragen = 1;
	
	if (FrequenzGewechselt == 1)
	{
		FrequenzGewechselt = 0;
		Zaehler            = 0;
	}
	
	Zaehler++;
	
	if (Zaehler == CompareMatch)
	{ 
		Zaehler = 0;
		PORTA ^= (1<<PA7);
	}
	
}



void FrequenzErhoehen(void)
{
	if (CompareMatch > 4)
	{
      CompareMatch = (CompareMatch >> 1);
	  FrequenzGewechselt = 1;
	}	  
}


void FrequenzNiederiger(void)
{
	if (CompareMatch < 128)
	{
      CompareMatch = (CompareMatch << 1);
	  FrequenzGewechselt = 1;
	}	
}


int main(void)
{
    int Taste            = 0;
	
	//Dip-Switch PA2
	//Taste PB1
	
	DDRA |= (1<<DDA7)|(1<<DDA6); //PA7 = rote LED, PA6 = rote LED (Motor)
	PORTA &= ~(1<<PA7); //LED aus
	PORTA |= (1<<PA6);  //LED an
	
	TCCR0A |= (1<<WGM01)|(1<<WGM00);  //Timer 0 im Fast PWM Mode
	TCCR0B |= (1<<CS02 )|(1<<CS00 );  //Timer 0 mit 8MHz/1024
	TIMSK0 |= (1<<TOIE0);             //ISR enabled
	/*
	TCCR1A |= (1<<WGM11)|(1<<WGM10); //Timer 1 im Fast PWM Mode
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS11 )|(1<<CS10 );  //Timer 1 mit 8MHz/64
	TIMSK1 |= (1<<TOIE0);
	*/
	sei();
	
	
	while(1)
    {
        if((PINB & (1<<PINB1)) == 0)                //Taster gedrueckt?
		{
          Tastegedrueckt   = 1;
		  
	      if (TasteAbfragen == 1)                       //jede 30ms
		  {   
            TasteAbfragen    = 0;
		  
		    PORTA ^= (1<<PA6);  //LED switchen
		  
		    if ((PINA & (1<<PINA2)) == 0)          //DIP-Switch0 
			  FrequenzErhoehen();
		    else 
		      FrequenzNiederiger();
	      }					
		}
		else
		  Tastegedrueckt = 0;	
    }
}