/*
 * lab5t2.c
 * Read an analog voltage and display it on serial
 * Note: Read ADCL before ADCH
 *  Created on: Apr 4, 2014
 *      Author: andrew
 */

#include <avr/io.h>
#include<avr/interrupt.h>
#include <stdio.h>

long unsigned int calc_voltage(double a, double b);
void adc_setup();
int USART_send(char c, FILE *stream);
void USART_setup();

volatile unsigned int volt_low, volt_high;
volatile unsigned long int voltage_final;

int main(void)
{
	adc_setup();
	USART_setup();
	sei();
	while(1){
			ADCSRA |= (1 << ADSC); // begin ADC
	}
}

ISR(ADC_vect) // Executed when ADIF=1 (ADC has finished conversion)
{
	volt_low = ADCL; 			// store ADCL
	volt_high = (ADCH & 0x03);	//store 2 rightmost bits of ADCH

	voltage_final = calc_voltage(volt_high, volt_low); // do math
	printf("\n VOLTAGE = %lu mV ", voltage_final);
}
long unsigned int calc_voltage(double high, double low)
{
	long unsigned int voltage;
	voltage = ((256*high + low)/1023) * 5000;
	return voltage;
}

void adc_setup()
{
	// adc source ADC6
	// ref voltage AVCC = 5V
	// allign left
	// steps: ADCMUX, ADCH/ADCL, ADCCSRA, SFIOR

	ADMUX = 0b01000110; // AVCC, right-alligned, ADC6
	ADCSRA = 0b10001011; 	//enable ADC, enable ADIE, prescale=2
}

int USART_send(char c, FILE *stream){
	// wait until UDRE flag is set to logic 1
	while ((UCSRA & (1<<UDRE)) == 0x00){;}
	UDR = c; // Write character to UDR for transmission
}

void USART_setup()
{
	stdout = fdevopen(&USART_send, NULL);
	UCSRC = 0b10000110;		// async, no parity, 8 bit char
	UCSRA = 0b00000000;		// normal speed
	// UBRR = 0x0019 Baud rate 2400bps with 1Mhz clock
	UBRRL = 0x19;
	UBRRH = 0x00;
	UCSRB = 0b00011000;		// Enable Tx Rx, no interrupts
}

