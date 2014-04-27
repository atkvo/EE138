// EE138 Group 6
// Members: Johnston Chong, Janice Pham, Andrew Vo
// Project: Elevator 
// Task   : Make design a multi-floor elevator
// Specs  :
//
//	Outside Features * Call button using interrupt (open door)
//
//	Inside Features  * Keypad controls movement up to 9 floors (after delay)
//			 * * On multiple floor req, elevator must stop at each floor
//
//	Door Features    * On destination, open doors for X time before closing
// 			 * * If no other floors selected, close door and stay there
//
// 			 * Sensor for if anything is in between the doors
// 			 * Anything else?
//


 

#include <avr/io.h>
#include <avr/interrupt.h>

// keypad array set as a GLOBAL variable
unsigned char keypad_key[4][4] = {{'1', '2', '3', 'A'},
				{'4', '5', '6', 'B'},
				{'7', '8', '9', 'C'},
				{'*', '0', '#', 'D'}};

// PORT Declarations
// Motors - Johnston
// PD4,5 PWM output
// Timer1
// Use overflow to count time passed

// Bell - Janice
// Use timer2
// PD7 for PWM output

// Keypad 
// Use PortC

// 7Segment 
// PA[7:1]

// ADC0
// PA[0]

// delay timer0


int delay_time = 0; //used to count seconds
// status flags
int call_flag=0;
int sel_flag=0;
int delay_flag = 0;
int fr_flag = 0; //signals floor is reached when 1
int dir_flag = 0; // 0 signals down, 1 signals up 

char nextfloor[9] = {0};
// function names here as well
// 

enum states {
    idle,
    move_x,
    open,
    delay_close,
    delay,
    sort,
    closing
};

unsigned char read_keypad(unsigned char digit_old);
void display_7led(unsigned char a);
void bell();
void door(int d);		// d = 1 open, d = 0 close
void elevator_movement();	// should take into account desired direction
void delay();
void flooraddsort(char digit, int mode);
void timer_init();		// initialize your timers here
int main (void)
{
	unsigned char digit;
	enum states state = idle; // initial state is idle

	DDRA  = 0xF0;		// P.A[7:4] OUT P.A[3:0] IN
	PORTA = 0xFF;		// P.A[3:0] Pullup P.A[7:4] set HIGH
	while(1)
	{
	    switch(state){
	    case idle:
	    	read_keypad();
	    	if (call_flag==1)
		{
		    state=move_x;
		    sel_flag=0;
		}
		else if (sel_flag==1)
		{
		    state=delay;
		    sel_flag=0;
		}
		else
		    state = idle;
	    	break;
	    case move_x:
	    	if(fr_flag==1)
		{
		    state=open; //open door when desired floor reached
		}
		break;
	    case open:
	    	//run func to open door
		state = delay_close;	    	//move to delay before closing door
	    	break;
	    case delay_close:
	    	delay_flag=1;
	    	if (delay_time == 3)
		{
		    state = closing;
		    delay_time =0;
		}
	    	break;
	    case delay: //use ISR routine for counting 3 seconds
	    	delay_flag=1;
	    	digit = read_keypad(); //read keypad (IN PROGRESS)

	    	if (sel_flag==1)
	    	    delay_time = 0;

	    	if (delay_time == 3)
		{
		    state = sort;
		    delay_time = 0;
		}
	    	break;
	    case closing:
	    	//run close door function here
	    	if (sel_flag==1)
		{
		    state = delay;
		    sel_flag = 0;
		}
		else if (/*TERMINATEFLOOR*/)
		{
		    memset(nextfloor, '0', 9); //clear floors
		    state = idle;
		}
		else
		    state = idle;
	    	break;
	    case sort:
	    	sortarray();
	    	state = move_x;
	    	break;
	    default:
	    	;

	    }
	}

	return 1;
}

ISR(TIMER0_OVF_vect)
{
    overflow0_count++;
    if (overflow0_count >=3906) // incr every sec 
    {
    	overflow_count = 0;
    	if (delay_flag==1)
    	    delay_time++;
	else
	    ;
	    
    }

}
// program should consist of multiple functions instead of long main code (make it modular)
//

void sortarray()
{
    // sort array here and return it?
}
void flooraddsort(char digit, int mode) //mode 0 = reset, mode 1 = add
{
    int y;
    if (mode == 0)
    {
    	y=0;
    }
    else
    {

    }
}

void timer_init()
{
    	TCCR0A = 0b00000000;
    	TCCR0B = 0b00000001;
        /*TIMSK  = 0b00000100;*/
        // Please use method below to enable your interrupts
        TIMSK |= (1 << TOIE0); //enable interrupt for timer0

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
		if ((digit == 'A')|(digit == 'B')|(digit == 'C')|(digit == 'D'))
		    call_flag = 1;
		else
		{
		    sel_flag = 1;
		    flooraddsort(digit);
		}

		/*return digit;*/
	}
	else
	    ;
		/*return digit_old;	//if no button press is detected, print old digit*/
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

