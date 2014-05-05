/*
 * elevator_rev01.c
 *
 *  Created on: May 5, 2014
 *      Author: andrew
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

// *** GENERIC GLOBAL VARIABLES HERE ***
unsigned int keypad_key[4][4] = {{1, 2, 3, 44},
				{4, 5, 6, 33},
				{7, 8, 9, 22},
				{69, 0, 69, 11}};
enum states {
    idle,
    move_x,
    open,
    delay_close,
    delay,
    adj_distance,
    closing
};


unsigned int delay_time = 0; //used to count seconds (timer0)
unsigned int delay_time_move;
volatile int next_distance;
volatile int overflow_count1;
volatile int overflow_count0;
volatile int current_floor = 1;
volatile int floor_selection_pointer = 1;
volatile int floor_distance_pointer = 1;
volatile int call_floor_distance = 0;
// *** GENERIC GLOBAL VARIABLES END ***


// *** status flags BEGIN ***
volatile bool elevator_flag;		// 1 means elevator is in up mode, 0 means down mode
volatile bool door_flag;			// 1 means door is in open mode, 0 means close mode.
volatile bool move_flag=0;
volatile bool call_flag=0;
volatile bool delay_flag = 0;
volatile bool dir_flag = 1; // 0 signals down, 1 signals up


// *** FUNCTIONS HERE ***
void read_keypad();
void display_7led(int a);
void bell();
void move_door();						// function to have door open or close
void door_check();						// function for door closing transition time, and sensor checking
void move_elevator(int number_of_floor); // function to have elevator moving up or down
void timer_init();		// initialize your timers here

int main (void)
{
	enum states state;
	state = idle; //initial state is in idle

	DDRA  = 0xF0;		// P.A[7:4] OUT P.A[3:0] IN
	PORTA = 0xFF;		// P.A[3:0] Pullup P.A[7:4] set HIGH
	DDRB  = 0xFF;		// P.B for 7seg
	PORTB = 0xFF;		//
	DDRC  = 0xFF;		// Port C for debugging purposes
	PORTC = 0x00;
	timer_init();		// initialize timer
	sei();				// enable global interrupts

	current_floor=1;
	while(1)
	{
		display_7led(current_floor);
		switch (state)
		{

		case idle:
		{
			read_keypad();
			if(call_flag==1)
			{
				state=move_x;
			}
			break;
		}

		case move_x:
		{
			if(call_flag==1)
			{
				move_elevator(call_floor_distance);	// move to called floor
				call_floor_distance = 0;
				call_flag=0; 			// reset call flag
				state=open;
			}
			break;
		}
		case open:
		{
			move_door(1);		// open the door
			state=delay_close;
			break;
		}
		case delay_close:
		{
			delay_flag = 1;		//begin counting delay_time
			if(delay_time == 3)	// wait 3 sec before closing door
			{
				state = closing;	// close door;
				delay_flag = 0;		// turn off delay flag
				delay_time = 0;		// reset delay_time
			}
			break;
		}
		case delay:
		{

			delay_flag = 1;		// begin counting delay_time

			break;
		}
		case closing:
		{
			move_door(0); 		// close the door
			state = idle;
			break;
		}
		case adj_distance:
		{

			break;
		}
		default:
			;
		} //end of switch

	} //end of while


}

void timer_init() // All timer configurations go here
{
    // TIMER0 begin
    	TCCR0 = 0b00000001; //no prescale
    	TCNT0 = 0;
    // TIMER0 end

    // TIMER1 begin
	//WGM[3:0] = 1110 for fast PWM & TOP = ICR1 (period width)
	//CS1[2:0] = 001 for 1MHz no prescale
	//COM1A[1:0] = 10 for CLEAR OC1A on match (to OCR1A)
	//COM1B[1:0] = 10
//	TCCR1A = 0b10100010;
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011001;
    // TIMER1 end

        /*TIMSK  = 0b00000100;*/
        // Please use method below to enable your interrupts
        // TIMSK is used for all timers
	// usage: TIMSK |= (1 << desiredbit)
	TIMSK |= 0b00000100;	//enable timer1 ovf int
    TIMSK |= (1 << TOIE0);	//enable interrupt for timer0

}

void move_elevator(int number_of_floor) //will also update current floor
{
	unsigned int period = 5000;				// PWM T=5ms or f=200Hz
	unsigned int duty_cycle;				// duty cycle
	if(dir_flag == 1)
		duty_cycle = 70;		// set duty cycle
	else
		duty_cycle = 30;
	unsigned int high_time = (period/100)*duty_cycle;	// PWM highTime=3.5ms
	unsigned int delay_time_per_floor = 500;		// time to one floor up or down
	delay_time_move = number_of_floor * delay_time_per_floor; 	//set delay time
	ICR1 = period;						// set PWM time period to ICR1

	high_time = (period/100)*duty_cycle;			// set PWM high time to OCR1A
	OCR1A = high_time;
	DDRD = 0b00100000;		// enable PD5
	overflow_count1 =  0;
	//door_flag=0;		// set interrupt acknowledge flag to 0

	move_flag = 1;

	while(move_flag==1){;}	// if interrupt acknowledge flag is 1,quit loop

	DDRD = 0b00000000;		// disable PortD

	if(dir_flag==1)
		current_floor = current_floor + number_of_floor;
	else if (dir_flag == 0)
		current_floor = current_floor - number_of_floor;

//	fr_flag = 1;			// flag that floor has been reached

}

ISR(TIMER0_OVF_vect)
{
	if (delay_flag==1)
	{
    overflow_count0++;
		if (overflow_count0 >=3906) // incr every sec
		{
			overflow_count0 = 0;
			delay_time++;
		}
    }

}

ISR(TIMER1_OVF_vect)			// ISR with timer overflow interrupt argument
{

	if(move_flag == 1)
	{
		overflow_count1++;			// increase when overflow reached
	}
	else if (door_flag==1)
		overflow_count1++;
	if(overflow_count1 >= delay_time_move)	// if count is (greater/equal) passed
	{
		overflow_count1 = 0;	       	// reset overflow counter
		door_flag = 0;		// set interrupt acknowlegde flag to 1 after delay time
		move_flag=0;
	}
}

void move_door(int door_motion)	//1 = open, 0 = close
{
	unsigned int period = 5000;				// T=5ms or f=200Hz
	unsigned int duty_cycle;				// Duty Cycle
	if(door_motion == 1){
		duty_cycle = 70;
	}		// assign duty cycle to open the door
	else // assign duty cycle to close the door
	{
//			door_check();			// check and wait for door close
			duty_cycle = 40;
	}
	unsigned int high_time = (period/100)*duty_cycle;	// PWM high time=
	unsigned int delay_time_door = 500;			// time fo servo to open or close door
	delay_time_move = delay_time_door;
	ICR1 = period;


	high_time = (period/100)*duty_cycle;		// PWM high time
	OCR1B = high_time;
	DDRD = 0b00010000;
	overflow_count1 = 0;
	door_flag=1;
	while(door_flag==1){;}			//

	DDRD = 0b00000000;				// disable PortD pins
}


void read_keypad()
{
	unsigned int digit;
	unsigned int y = 0;
	unsigned int x = 0;
	unsigned int pressed = 0;
	unsigned int porta_signals = (0b00000000);
	unsigned int bitcheck = 3;

	for(unsigned int row = 3; row>=0; row--){
		porta_signals = (1<<(row+4)); //shift 0b1 over 5, 6, 7 bits
		PORTA = ~porta_signals;		//make sure things are done with active low
		//begin checking columns ...
		x = 0;
		bitcheck = 3;

		for(unsigned int column = 0; column <= 3; column++)
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

	if (pressed==1) //pressed=1 means a key has been pressed
	{

		digit = keypad_key[y][x]; // returns a digit based on y,x coord

		if ((digit == 44)|(digit == 33)|(digit ==  22)|(digit == 11))
		{

			call_flag = 1;
			digit = digit/11;

			call_floor_distance = digit - current_floor;
			if (call_floor_distance < 0)
			{
				call_floor_distance = call_floor_distance*(-1);
				dir_flag = 0;		// if floor is called below current floor, move down
			}
			else
				dir_flag = 1;
			PORTC=~digit;
		}
	}
	else
		;
}

void display_7led(int a)
{
	switch(a)
	{
		case 0:
			PORTB = 0x40;
			break;
		case 1:
			PORTB = 0x79;
			break;
		case 2:
			PORTB = 0x24;
			break;
		case 3:
			PORTB = 0x30;
			break;
		case 4:
			PORTB = 0x19;
			break;
		case 5:
			PORTB = 0x12;
			break;
		case 6:
			PORTB = 0x02;
			break;
		case 7:
			PORTB = 0x78;
			break;
		case 8:
			PORTB = 0x00;
			break;
		case 9:
			PORTB = 0x10;
			break;

	}
}
