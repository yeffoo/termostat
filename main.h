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

typedef struct {
	uint8_t godz;
	uint8_t min;
} czas_t;

typedef struct {
	czas_t czas;
	uint8_t temp;
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
