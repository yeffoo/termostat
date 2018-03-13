/*
 * main_it.c
 *
 *  Created on: 17-01-2018
 *      Author: Jeff
 */

#include "main.h"

extern volatile uint16_t timer1; // ds18b20
extern volatile uint16_t timer3; // refresh heating
extern volatile uint16_t timer4; // switch
extern volatile uint16_t timer5; // menu

extern volatile uint16_t rtc_flag;
extern volatile int8_t zmienna;

ISR(TIMER2_COMPA_vect) {
	uint16_t x;

	x = timer1;
	if(x)
		timer1 = --x;
	x = timer3;
	if(x)
		timer3 = --x;
	x = timer4;
	if(x)
		timer4 = --x;
	x = timer5;
	if(x)
		timer5 = --x;
}

ISR(INT0_vect) {
	rtc_flag = 1;
}

ISR(INT1_vect) {
	timer5 = 5000;
	if( !(PINC & ENKODER_A) ) {
		zmienna = 1;
	}
	else
		zmienna = -1;
}
