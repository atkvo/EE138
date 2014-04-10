/* File: ee138 - lab3t2.c
   Description: Timers Lab Task 2
   - Measure square wave period and convert to Hz. Range of operation: 20Hz to ~9KHz
   Group Members: Johnston Chong, Janice Pham, Andrew Vo
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

double difference_uS= 0;
volatile unsigned int edge_diff = 0, edge_1 = 0, edge_2 = 0;
volatile unsigned int edge_count;
double freq_double; 	//frequency as double for math
unsigned int freq_int; 	//frequency to be printed

void USART_setup(void);
int USART_send(char c, FILE *stream);
void timer_init();

int main(void)
{
	USART_setup();	//set up serial port
	stdout = fdevopen(&USART_send, NULL);	//setup out stream to use serial

	DDRB = 0xFF; //set portB as output
	timer_init(); //initialize timer
	sei();		//enable global sys interrupts

	while(1)
	{
	}

}

void USART_setup(void) //
{
	UCSRC = 0b10000110;		// async, no parity, 8 bit char
	UCSRA = 0b00000000;		// normal speed
	// Baud rate 1200bps with 1Mhz clock
	UBRRL = 0x33; //1200bps
	UBRRH = 0x00;
	UCSRB = 0b00011000;		// Enable Tx Rx, no interrupts
}

int USART_send(char c, FILE *stream){
	// wait until UDRE flag is set to logic 1
	while ((UCSRA & (1<<UDRE)) == 0x00){;}
	UDR = c; // Write character to UDR for transmission
}

void timer_init()
{
	TCCR1A = 0b00000000;
	TCCR1B = 0b11000001; //noice cancellor & rising edge detect
	TIMSK  = 0b00100000; //enable ICR
}


ISR(TIMER1_CAPT_vect) //interrupt on input capture interrupt
{
	if(edge_count==0){
		edge_1 = ICR1;
		edge_count=1; //edge 1 found, look for edge 2 next
	}
	else if (edge_count == 1)
	{
		edge_2 = ICR1;
		edge_diff = edge_2-edge_1;
		difference_uS = edge_2-edge_1; //the time between edge pulses  = period
		difference_uS = difference_uS * .000001;
		freq_double = 1/difference_uS;
		freq_int = freq_double; 	//convert back to unsigned int to print
		PORTB = ~(freq_int>>8);		//shift 8 bits to get high bits to use to visualize with LEDs
//		printf("\n edge_1: %u \t edge_2: %u \t period: %u \t freq: %u\n", edge_1, edge_2, edge_diff, freq_int); //used to debug
		printf("\n Frequency: %u Hz\n", freq_int);
		edge_count=0; //go back to lead edge
	}
}




