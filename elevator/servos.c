/*
 * finalProject_servo1.c
 *
 * Created: 4/29/2014 9:36:44 PM
 *  Servo
 */ 

// This program generates PWM signals for the servos
// Servo1 and servo2 will alternatively turn ON and OFF.
// PD4 and PD5 are PWM signal pins for the servos.
// I think the servo got one speed only, it turns CW or CCW.

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

volatile int overflow_count;

void timer_init();
 
 int main(void)
{
	// servo operating parameters: f=200Hz or T=5ms
	// dutyCycle>50% servo turns CCW (facing up)
	// dutyCycle<50% servo turns CW (facing up)
	unsigned int period = 5000; // T=5ms or f=200Hz
	unsigned int duty_cycle = 70; // 70% 
	unsigned int high_time = (period/100)*duty_cycle; //highTime=3.5ms
	overflow_count = 0;
	timer_init();
	DDRD=0b00100000;
	ICR1 = period;	
	OCR1A = high_time;
	OCR1B = high_time;
	sei();                      // enable global sys interrupts
	while(1)
	{
		;
	}
}

void timer_init()
{
	//WGM[3:0] = 1110 for fast PWM & TOP = ICR1 (period width)
	//CS1[2:0] = 001 for 1MHz no prescale
	//COM1A[1:0] = 10 for CLEAR OC1A on match (to OCR1A)
	//COM1B[1:0] = 10
	TCCR1A = 0b10100010;
	TCCR1B = 0b00011001;
	TIMSK  = 0b00000100; // enable timer overflow interrupt
}

ISR(TIMER1_OVF_vect) // ISR with timer overflow interrupt argument
{
	overflow_count++;       // increase when overflow reached
	if(overflow_count >= 100) // if count is (greater/equal) passed
	{							// I pick a random number ~100
		overflow_count = 0;  // reset overflow counter
		 DDRD ^= 0b00110000; // toggle PD4(OC1B) and PD5(OC1A)
	}	
}