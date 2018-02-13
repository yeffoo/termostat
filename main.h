/*
 * main.h
 *
 *  Created on: 16-01-2018
 *      Author: Jeff
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "LCD/lcd44780.h"

/*

 		W		H
 ~		A		E
 A	 	T		A
 C	 	E		T
 	 	R

 */

#define RTC_INT 	(1 << PD2)
#define ENCODER_INT	(1 << PD3)
#define BUTTON		(1 << PD1)

#define T1_TRIAC	(1 << PB7)
#define T2_TRIAC 	(1 << PB6)

#define T1_ON		PORTB |= T1_TRIAC;
#define T1_OFF		PORTB &= ~T1_TRIAC;
#define T2_ON		PORTB |= T2_TRIAC;
#define T2_OFF		PORTB &= ~T2_TRIAC;

#define BUTTON_UP	(1 << PC2)
#define BUTTON_DOWN	(1 << PC3)

#define ENKODER_DDR DDRC
#define ENKODER_PIN PINC
#define ENKODER_PORT PORTC
#define ENKODER_A (1 << PC3)
#define ENKODER_B (1 << PC2)
#define ENKODER_SW //BUTTON

#define BACKLIGHT_ON PORTC &= ~(1 << PC0);
#define BACKLIGHT_OFF PORTC |= (1 << PC0);

typedef struct {
	uint8_t godz;
	uint8_t min;
} czas_t;

typedef struct {
	czas_t czas;
	uint16_t temp;
	czas_t czas_OD_led1;
	czas_t czas_DO_led1;
	czas_t czas_OD_led2;
	czas_t czas_DO_led2;
	uint8_t jasnosc_led1;
	uint8_t jasnosc_led2;
	czas_t czas_OD_wodospad;
	czas_t czas_DO_wodospad;
} menu_t;

#endif /* MAIN_H_ */
