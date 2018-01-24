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

void f_klaw_UD(volatile uint8_t *KPIN, uint8_t key_mask, uint16_t time, void (*func)(void));

void f_klaw(volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) );

void menu_wyswietl(uint8_t wybor, menu_t *menu);

uint8_t menu_ustaw();

void menu_domyslne(menu_t *menu);

void ustaw_temp(menu_t *menu, uint8_t temp);

void ustaw_czas(czas_t czas);

void ustaw_czas_led1(menu_t *menu, czas_t czas_od, czas_t czas_do);

void ustaw_czas_led2(menu_t *menu, czas_t czas_od, czas_t czas_do);

void ustaw_jasnosc_led1(menu_t *menu, uint8_t jasnosc);

void ustaw_jasnosc_led2(menu_t *menu, uint8_t jasnosc);

void ustaw_czas_wodospad(menu_t *menu, czas_t czas_od, czas_t czas_do);

void przekaznik(uint8_t wybor);

void triak(uint8_t wybor);

void wyswietl_napis(char *str);

void zapisz_ustawienia(menu_t *menu);

menu_t wczytaj_ustawienia();

#endif /* MENU_H_ */
