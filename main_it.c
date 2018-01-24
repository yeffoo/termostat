/*
 * main_it.c
 *
 *  Created on: 17-01-2018
 *      Author: Jeff
 */

#include "main.h"

extern volatile uint16_t timer1; // ds18b20
extern volatile uint16_t timer2; // refresh LCD
extern volatile uint16_t timer3; // refresh heating
extern volatile uint16_t timer4; // switch
extern volatile uint16_t timer5; // menu
extern volatile uint16_t timer6; // switches UD

extern volatile uint16_t rtc_flag;
extern volatile uint16_t button_flag;

ISR(TIMER2_COMPA_vect) {
	uint16_t x;
	x = timer1;
	if(x)
		timer1 = --x;
	x = timer2;
	if(x)
		timer2 = --x;
	x = timer3;
	if(x)
		timer3 = --x;
	x = timer4;
	if(x)
		timer4 = --x;
	x = timer5;
	if(x)
		timer5 = --x;
	x = timer6;
	if(x)
		timer6 = --x;
}

ISR(INT0_vect) {
	rtc_flag = 1;
}

ISR(INT1_vect) {
	button_flag = 1;
}

//ISR(PCINT2_vect) {
//	button_flag = 1;
//}
