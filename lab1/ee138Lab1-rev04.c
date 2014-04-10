/* File: ee138 - lab1.c
   Description: User keypad inputs will be displayed on 7-segment display
   Group Members: Johnston Chong, Janice Pham, Andrew Vo
*/
#include <avr/io.h>     // AVR header file for all registers/pins

unsigned char read_keypad(unsigned char digit_old);
//read_keypad is the main function that scans down rows & columns
void display_7led(unsigned char a);

// keypad array set as a GLOBAL variable
unsigned char keypad_key[4][4] = {{'1', '2', '3', 'A'},
								{'4', '5', '6', 'B'},
								{'7', '8', '9', 'C'},
								{'*', '0', '#', 'D'}};
int main(void){
	unsigned char digit;		//where digit to be printed will be held
	unsigned char digit_old = '0'; //where the previous digit will be held

	DDRA = 0xF0;         // set PORTA[7:4] OUT, PORTA[3:0] IN
   PORTA = 0xFF;		// turn on PULLUP res for PORTA[3:0] and A[7:4] HIGH
	DDRB = 0xFF;         // set PORTB for output
   PORTB = 0x00;        // turn ON all LEDs initially

	while(1){
		digit = read_keypad(digit_old);
		digit_old = digit;	//store old digit incase no button is pressed on next cycle
		display_7led(digit);
	}
	return 1;
}

unsigned char read_keypad(unsigned char digit_old){
	unsigned char digit;
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
		digit = keypad_key[y][x]; // returns a digit based on y,x coord
		return digit;
	}
	else
		return digit_old;	//if no button press is detected, print old digit
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
