/* File: ee138 - lab2t2.c
   Description: Serial Comm Task 2
   Group Members: Johnston Chong, Janice Pham, Andrew Vo
*/

#include <avr/io.h>

void USART_setup(void);
unsigned char USART_receive(void);
void display_7led(unsigned char a);

unsigned char read_keypad(unsigned char digit_old);
unsigned char USART_receive(void);
unsigned char keypad_key[4][4] =	{{'1', '2', '3', 'A'},
									{'4', '5', '6', 'B'},
									{'7', '8', '9', 'C'},
									{'*', '0', '#', 'D'}};

int main(void)
{
	USART_setup();

	DDRA = 0xF0;         // set PORTA[7:4] OUT, PORTA[3:0] IN
	PORTA = 0xFF;		// turn on PULLUP res for PORTA[3:0] and A[7:4] HIGH
	DDRB = 0xFF;         // set PORTB for output
	PORTB = 0x00;        // turn ON all LEDs initially

    while(1)
    {
    	display_7led(USART_receive());
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

//from PC to board
unsigned char USART_receive(void){
	// Wait until RXC flag = 1
	while ((UCSRA & (1<<RXC)) == 0x00){;}
	// Read the received char from UDR
	return (UDR);
}

void display_7led(unsigned char a)
{
	switch(a)
	{
		case '0':
		PORTB = 0x40;
		break;
		case '1':
		PORTB = 0x79;
		break;
		case '2':
		PORTB = 0x24;
		break;
		case '3':
		PORTB = 0x30;
		break;
		case '4':
		PORTB = 0x19;
		break;
		case '5':
		PORTB = 0x12;
		break;
		case '6':
		PORTB = 0x02;
		break;
		case '7':
		PORTB = 0x78;
		break;
		case '8':
		PORTB = 0x00;
		break;
		case '9':
		PORTB = 0x10;
		break;
		case 'A':
		PORTB = 0x08;
		break;
		case 'B':
		PORTB = 0x03;
		break;
		case 'C':
		PORTB = 0x46;
		break;
		case 'D':
		PORTB = 0x21;
		break;
		case '*':
		PORTB = 0x23;
		break;
		case '#':
		PORTB = 0x0F;
		break;
	}
}
