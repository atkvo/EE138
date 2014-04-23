// EE138 Group 6
// Members: Johnston Chong, Janice Pham, Andrew Vo
// Project: Elevator 
// Task   : Make design a multi-floor elevator
// Specs  : Outside Features * Call button using interrupt (open door)
//
// 			Inside Features  * Keypad controls movement up to 9 floors (after delay)
// 							 * * On multiple floor req, elevator must stop at each floor
//
// 			Door Features    * On destination, open doors for X time before closing
// 							 * * If no other floors selected, close door and stay there
//
// 			Extra Credit 	 * Bell for door close / destination reached, alarm, etc
// 							 * Sensor for if anything is in between the doors
// 							 * Anything else?
//


 

#include <avr/io.h>
#include <avr/interrupt.h>

// keypad array set as a GLOBAL variable
unsigned char keypad_key[4][4] = {{'1', '2', '3', 'A'},
								{'4', '5', '6', 'B'},
								{'7', '8', '9', 'C'},
								{'*', '0', '#', 'D'}};


// Define state machine names here
// function names here as well
//
unsigned char read_keypad(unsigned char digit_old);
void display_7led(unsigned char a);



int main (void)
{
	unsigned char digit;
	
	DDRA  = 0xF0;		// P.A[7:4] OUT P.A[3:0] IN
	PORTA = 0xFF;		// P.A[3:0] Pullup P.A[7:4] set HIGH
	while(1)
	{
	}

	return 1;
}

// program should consist of multiple functions instead of long main code (make it modular)
//

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

