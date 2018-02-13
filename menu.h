/*
 * menu.h
 *
 *  Created on: 21-01-2018
 *      Author: Jeff
 */

#ifndef MENU_H_
#define MENU_H_

#include "main.h"

#include "eeprom.h"

void f_klaw(volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) );

void menu_wyswietl_normalnie(menu_t *menu, uint8_t *var);

void menu_ustaw(uint8_t wybor, int8_t wartosc, menu_t *menu);

void menu_domyslne(menu_t *menu);

void wyswietl_napis(char *str);

void zapisz_ustawienia(menu_t *menu);

void wczytaj_ustawienia(menu_t *menu);

#endif /* MENU_H_ */
