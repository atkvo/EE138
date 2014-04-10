/* File: ee138 - lab4t1.c
   Description: PWM Lab
   Group Members: Johnston Chong, Janice Pham, Andrew Vo
*/
#include <avr/io.h>
#include <stdio.h>

void timer_init();
int USART_send(char c, FILE *stream);
int USART_receive(FILE *stream);
void USART_setup();
void calculate(int f, int d);
int main(void)
{
	unsigned int freq_int, duty_int;
	DDRD=0b00100000; //set OC1A as output (D.5)
	timer_init();
	USART_setup();

	printf("\n Enter initial [Hz] [Duty Cycle%%]: ");
	scanf("%u %u", &freq_int, &duty_int);
	calculate(freq_int, duty_int);
	printf("\nCurrent Period(ICR1) [uS]: %u \t Time High[uS]: %u \n", ICR1, OCR1A);

	while(1)
	{
		printf("\n\n Enter [Hz] [Duty Cycle%%]: ");
		scanf("%u %u", &freq_int, &duty_int);
		calculate(freq_int, duty_int);
		printf("Current Period(ICR1) [uS]: %u \t Time High[uS]: %u \n", ICR1, OCR1A);	}
}
void timer_init()
{
	//WGM[3:0] = 1110 for fast PWM & TOP = ICR1 (period width)
	//CS1[2:0] = 001 for 1MHz no prescale
	//COM1A[1:0] = 10 for CLEAR OC1A on match (to OCR1A)
	TCCR1A = 0b10000010;
	TCCR1B = 0b00011001;
}

int USART_send(char c, FILE *stream){
	// wait until UDRE flag is set to logic 1
	while ((UCSRA & (1<<UDRE)) == 0x00){;}
	UDR = c; // Write character to UDR for transmission
}

int USART_receive(FILE *stream){
	// wait until RXC flag is set to logic 1
	while ((UCSRA & (1<<RXC)) == 0x00){;}
	return (UDR); // Read the received character from UDR
}

void USART_setup()
{
	stdout = fdevopen(&USART_send, NULL);
	stdin = fdevopen(NULL, &USART_receive);
	UCSRC = 0b10000110;		// async, no parity, 8 bit char
	UCSRA = 0b00000000;		// normal speed
	// Baud rate 1200bps with 1Mhz clock
	UBRRL = 0x33;
	UBRRH = 0x00;
	UCSRB = 0b00011000;		// Enable Tx Rx, no interrupts

}

//will convert inputted freq/duty% to period[uS] and the OCR1A value[uS]
void calculate(int f, int d)	
{
	double freq = f;
	double duty = d;
	double period, T_h;
	int period_int, T_h_int;
	period = (1/freq)*1000000;
	period_int = period;
	ICR1 = period_int;  //get period (uS) from Hz

	T_h  = (duty/100)*period;
	T_h_int = T_h;
	OCR1A=T_h_int;	// T_h=period*dutycycle%
}
