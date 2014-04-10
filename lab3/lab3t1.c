/* File: ee138 - lab3t1.c
   Description: Timers Lab
   Group Members: Johnston Chong, Janice Pham, Andrew Vo
*/
#include <avr/io.h>
#include <avr/interrupt.h>

volatile int overflow_count;

void timer_init();

int main(void)
{
	overflow_count = 0;
	DDRB = 0xFF; //set portB as output
	PORTB = 0b10101010; //turn on odd leds (active low)
	timer_init();	//initialize timer
	sei();		//enable global sys interrupts
	while(1)
	{
	}
}
void timer_init()	//initializes timer1
{
	TCCR1A = 0b00000000; //normal mode, no output compares
	TCCR1B = 0b00000001; //no prescale
	TIMSK  = 0b00000100; //enable timer overflow interrupt
}

ISR(TIMER1_OVF_vect) //ISR with timer overflow interrupt argument
{
	overflow_count++; //increase when overflow reached
	if (overflow_count >=46){ //when overflow = 46 times, ~ 3sec passed
		overflow_count = 0; //reset counter
		PORTB ^= 0b11111111; //toggle all ports using XOR
	}
}



