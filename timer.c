/*
 * timer.c
 *
 *  Created on: 17-01-2018
 *      Author: Jeff
 */

#include "timer.h"

void timer0_init() {
	DDRD |= (1 << PD5) | (1 << PD6);

	TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // output
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // fast PWM
	TCCR0B |= (1 << CS02); // /256

	OCR0A = 25;	// PD6
	OCR0B = 25;	// PD5
}

void timer2_init() {
//	TCCR2A |= (1 << COM2A1); // Clear OC0A on Compare Match
	TCCR2A |= (1 << WGM21); // CTC
	TCCR2B |= (1 << CS22); // Prescaler 64, f=125 kHz
	TIMSK2 |= (1 << OCIE2A); // Compare interrupt
	OCR2A = 125;
}
