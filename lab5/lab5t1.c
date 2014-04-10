/*
 * lab5t1.c
 * Read an analog voltage and display it on LEDs
 * Note: Read ADCL before ADCH
 *  Created on: Apr 4, 2014
 *      Author: andrew
 */

#include <avr/io.h>
#include<avr/interrupt.h>

void adc_setup();

int main(void)
{
	DDRB = 0xFF;		// set portB as output
	adc_setup();
	sei();
	while(1){
			ADCSRA |= (1 << ADSC);
	}
}

ISR(ADC_vect) // Executed when ADIF=1 (ADC has finished conversion)
{
	unsigned int volt_high = ADCH;
	PORTB = ~volt_high; //display ADCH on LED (ignore 2^0 and 2^1 bits in ADCL)
}

void adc_setup()
{
	// adc source ADC6
	// ref voltage AVCC = 5V
	// allign left
	// steps: ADCMUX, ADCH/ADCL, ADCCSRA, SFIOR

	ADMUX = 0b01100110; // AVCC, Left Alligned, ADC6
	ADCSRA = 0b10001001; 	//enable ADC, enable ADIE, prescale=2
							//ADIF is 1 when conv done
}
