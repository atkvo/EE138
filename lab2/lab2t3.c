/* File: ee138 - lab2t3.c
   Description: Serial Comm Task 3
   Group Members: Johnston Chong, Janice Pham, Andrew Vo
*/

#include <avr/io.h>
#include <stdio.h>

void USART_setup(void);
int USART_send(char c, FILE *stream);
int USART_receive(FILE *stream);

int main(void)
{
	unsigned int a, b;
	USART_setup();

	stdout = fdevopen(&USART_send, NULL);
	stdin = fdevopen(NULL, &USART_receive);

    while(1)
    {
		printf("\n Enter a b: ");
		scanf("%d %d", &a, &b);
		printf("\nfirst = %d, second =%d, sum =%d, diff=%d, mult=%d, quotient=%d", a, b, a + b, a - b, a*b, a/b);
    }
}

void USART_setup(void) //
{
	UCSRC = 0b10000110;		// async, no parity, 8 bit char
	UCSRA = 0b00000000;		// normal speed
	// Baud rate 1200bps with 1Mhz clock
	UBRRL = 0x33;
	UBRRH = 0x00;
	UCSRB = 0b00011000;		// Enable Tx Rx, no interrupts
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
