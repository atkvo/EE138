
/* File: ee138 - lab2t1.c
   Description: Serial Comm Task 1
   Group Members: Johnston Chong, Janice Pham, Andrew Vo
*/

#include <avr/io.h>     // AVR header file for all registers/pins

unsigned char read_keypad(unsigned char digit_old);
//read_keypad is the main function that scans down rows & columns

void USART_setup();
void USART_send(unsigned char data);

// keypad array set as a GLOBAL variable
unsigned char keypad_key[4][4] = {{'1', '2', '3', 'A'},
								{'4', '5', '6', 'B'},
								{'7', '8', '9', 'C'},
								{'*', '0', '#', 'D'}};
int main(void){
	unsigned char data;		//where digit to be printed will be held
	unsigned char data_old = '0'; //where the previous digit will be held

	USART_setup();		//initialize USART

	DDRA = 0xF0;         // set PORTA[7:4] OUT, PORTA[3:0] IN
   PORTA = 0xFF;		// turn on PULLUP res for PORTA[3:0] and A[7:4] HIGH

	while(1){
		data = read_keypad(data_old);
		data_old = data;	//store old digit incase no button is pressed on next cycle
		USART_send(data);
	}
	return 1;
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

void USART_send(unsigned char data){
	// Wait until UDRE flag = 1
	while ((UCSRA & (1<<UDRE)) == 0x00){;}
	// Write char to UDR for transmission
	UDR = data;
}

unsigned char read_keypad(unsigned char data_old){
	unsigned char data;
	unsigned char y = 0;
	unsigned char x = 0;
	unsigned char pressed = 0;
	unsigned char porta_signals = (0b00000000);
	unsigned char bitcheck = 3;

	for(unsigned char row = 3; row>=0; row--){
		porta_signals = (1<<(row+4)); //shift 0b1 over 5, 6, 7 bits
		PORTA = ~porta_signals;		//make sure things are done with active low
		//begin checking columns ...
		x = 0;
		bitcheck = 3;
		for(unsigned char column = 0; column <= 3; column++)
		{
			if(!bit_is_set(PINA, bitcheck))
			{
				pressed = 1;
				break;
			}
			x++;
			bitcheck--;
		}
		//... done checking columns
		if (pressed==1)
		{
			break;
		}
		else
			y++; //y is to mark ROW
	}

	if (pressed==1){ //pressed=1 means a key has been pressed
		data = keypad_key[y][x]; // returns a digit based on y,x coord
		return data;
	}
	else
		return data_old;	//if no button press is detected, print old digit
}

