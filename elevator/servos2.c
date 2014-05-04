
// This program generates PWM signals for the servos
// Servo1 and servo2 will alternatively turn ON and OFF.
// PD4 and PD5 are PWM signal pins for the servos.
// I think the servo got one speed only, it turns CW or CCW.
// servo operating parameters: f=200Hz or T=5ms
// dutyCycle>50% servo turns CCW (facing up)
// dutyCycle<50% servo turns CW (facing up)

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

volatile int overflow_count;		 // overflow counter
volatile bool interrupt_ack_flag;	 // flag in the ISR for time delay
volatile bool elevator_flag;		// 1 means elevator is in up mode, 0 means down mode
volatile bool door_flag;			// 1 means door is in open mode, 0 means close mode.

void move_elevator(int number_of_floor); // function to have elevator moving up or down
void move_door();						// function to have door open or close
void door_check();						// function for door closing transition time, and sensor checking
void timer_init();						// function for timer initialization

int delay_time = 0;

 
 int main(void)
{
	overflow_count = 0;
	timer_init();					// timer initialization

	unsigned elevator_flag = 0		// 0 means down, 1 means up  
	unsigned number_of_floor = 0;  // number of floors to the destination            
						
	while(1)
	{
	//	.....
	// door_flag=1;
	// move_door();
	// door_flag=0;
	// move_door();
	
	}
}

void timer_init()
{	
	//WGM[3:0] = 1110 for fast PWM & TOP = ICR1 (period width)
	TCCR1A = 0b10100010; //CS1[2:0] = 001 for 1MHz no prescale
	TCCR1B = 0b00011001;  //COM1A[1:0] = 10 for CLEAR OC1A on match (to OCR1A) //COM1B[1:0] = 10
	TIMSK  = 0b00000100; // enable timer overflow interrupt
}

void move_elevator(int number_of_floor) 
{
	unsigned int period = 5000;							// PWM T=5ms or f=200Hz
	unsigned int duty_cycle;							// duty cycle
	unsigned int high_time = (period/100)*duty_cycle;	// PWM highTime=3.5ms
	unsigned int delay_time_per_floor = 300;			// time to one floor up or down
	delay_time = number_of_floor * delay_time_per_floor; //set delay time 
	ICR1 = period;										// set PWM time period to ICR1
	if(elevator_flag == 1) { duty_cycle = 70; }			// set duty cycle
	else if(elevator_flat == 0) { duty_cycle = 30; }
	high_time = (period/100)*duty_cycle;				// set PWM high time to OCR1A
	OCR1A = high_time;
	DDRD = 0b00100000;									// enable PD5
	overflow_count =  0;
	interrupt_ack_flag=0;								// set interrupt acknowledge flag to 0
	sei();												// enable interrupt subsystem globally
	while(interrupt_ack_flag==0){;}						// if interrupt acknowledge flag is 1,quit loop 
	cli();												// disable interrupt
	DDRD = 0b00000000;									// disable PortD
}

void move_door()
{
	unsigned int period = 5000;							// T=5ms or f=200Hz
	unsigned int duty_cycle;							// Duty Cycle
	unsigned int high_time = (period/100)*duty_cycle;	// PWM high time=
	unsigned int delay_time_door = 100;					// time fo servo to open or close door
	delay_time = number_of_floor * delay_time_door;
	ICR1 = period;
									
	if(door_flag == 1){ duty_cycle = 70;}				// assign duty cycle to open the door
	else												// assign duty cycle to close the door
		{
			door_check();								// check and wait for door close
			duty_cycle = 30;
		}
	high_time = (period/100)*duty_cycle;				 // PWM high time
	OCR1A = high_time;					
	DDRD = 0b00010000;
	overflow_count = 0;
	sei();												// enable interrupt subsystem globally
	interrupt_ack_flag=0;								// set acknoledgement interupt 
	while(interrupt_ack_flag==0){;}						// 
	cli();												// disable interrupt
	DDRD = 0b00000000;									// disable PortD pins
}

void door_check()
{
	unsigned int delay_door_transition_time = 200;	// transmition time to close the door
	delay_time = delay_door_transition_time; 
	overflow_count = 0;
	interrupt_ack_flag=0;
	sei();
	while(interrupt_ack_flag==0){;}
	cli();
}



ISR(TIMER1_OVF_vect)					// ISR with timer overflow interrupt argument
{
	overflow_count++;					 // increase when overflow reached
	if(overflow_count >= delay_time)	 // if count is (greater/equal) passed
	{						
		overflow_count = 0;				// reset overflow counter
		interrupt_ack_flag = 1;		 // set interrupt acknowlegde flag to 1 after delay time
	}	
}