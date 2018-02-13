/*
 * menu.c
 *
 *  Created on: 21-01-2018
 *      Author: Jeff
 */

#include "menu.h"
#include "pcf8583.h"

extern uint16_t temp_pobrana;

extern volatile uint16_t timer4; // switch
extern volatile uint16_t timer5; // menu

uint8_t menu_pos;

void f_klaw(volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) ) {

	enum KS {idle, debounce, go_rep, wait_rep, rep};

	static enum KS key_state;
	static uint8_t last_key;
	uint8_t key_press;

	if( last_key && last_key != key_mask ) return;

	key_press = !(*KPIN & key_mask);

	if( key_press && !key_state ) {
		key_state = debounce;
		timer4 = 5;
	} else
	if( !timer5 ) {
		menu_pos = 0;
//		BACKLIGHT_OFF;
	}

	if( key_state  ) {
//		timer5 = 4000;
//		BACKLIGHT_ON;
		if( key_press && debounce==key_state && !timer4 ) {

			key_state = go_rep;
			timer4=3;
			last_key = key_mask;

			if( menu_pos ) {
				timer5 = 4000;
				menu_pos++;
			}
			if( menu_pos >= 12 )
				menu_pos = 1;
		} else
		if( !key_press && key_state>debounce && key_state<rep ) {
			if(push_proc) {
				push_proc();
			}
			key_state=idle;
			last_key = 0;
		} else
		if( key_press && go_rep==key_state && !timer4 ) {
			if(!rep_time) rep_time=20;
			if(!rep_wait) rep_wait=150;
			key_state = wait_rep;
			timer4=rep_wait;
		} else
		if( key_press && wait_rep==key_state && !timer4 ) {
			key_state = rep;
		} else
		if( key_press && rep==key_state && !timer4 ) {
			timer4 = rep_time;
			menu_pos++;
			timer5 = 4000;
			if(rep_proc) { // po 4s wchodzi do menu
				rep_proc();						/* KEY_REP */
			}
		}
	}

	if( key_state>=wait_rep && !key_press ) {
		key_state = idle;
		last_key = 0;
//		BACKLIGHT_OFF;
	}
}

enum emenu_wyswietl {temperatura=1, czas_led1, czas_led2, jasnosc_led1, jasnosc_led2, czas_wodospad, czas_systemowy};

void menu_wyswietl_normalnie(menu_t *menu, uint8_t *var) {
	lcd_cls();
	wyswietl_napis("Temp.: ");
	lcd_int(temp_pobrana/10);
	lcd_char('.');
	lcd_int(temp_pobrana%10);
	lcd_locate(1, 5);
	lcd_int(menu->czas.godz);
	lcd_char(':');
	if(menu->czas.min <10) {
		lcd_int(0);
		lcd_int(menu->czas.min);
	}
	else
		lcd_int(menu->czas.min);
}

enum emenu_ustaw {czas_wodospad_godz_OD=1, czas_wodospad_godz_DO,
				  czas_led1_godz_OD, czas_led1_godz_DO,
				  czas_led2_godz_OD, czas_led2_godz_DO,
				  jasnosc_led1_ustaw,
				  jasnosc_led2_ustaw,
				  temperatura_ustaw,
				  czas_systemowy_godz, czas_systemowy_min};

void menu_ustaw(uint8_t wybor, int8_t wartosc, menu_t *menu) {
	switch(wybor) {
	case czas_wodospad_godz_OD:
		if( menu->czas_OD_wodospad.godz >=24 )
			menu->czas_OD_wodospad.godz = 0;
		menu->czas_OD_wodospad.godz += wartosc;
		wyswietl_napis("Czas woda ->");
		lcd_locate(1, 0);
		lcd_int(menu->czas_OD_wodospad.godz);
		lcd_char(':');
		if(menu->czas_OD_wodospad.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_OD_wodospad.min);
		}
		else
			lcd_int(menu->czas_OD_wodospad.min);
		break;
	case czas_wodospad_godz_DO:
		if( menu->czas_DO_wodospad.godz >=24 )
			menu->czas_DO_wodospad.godz = 0;
		menu->czas_DO_wodospad.godz += wartosc;
		wyswietl_napis("Czas woda <-");
		lcd_locate(1, 6);
		lcd_int(menu->czas_DO_wodospad.godz);
		lcd_char(':');
		if(menu->czas_DO_wodospad.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_DO_wodospad.min);
		}
		else
			lcd_int(menu->czas_DO_wodospad.min);
		break;
	case czas_led1_godz_OD:
		if( menu->czas_OD_led1.godz >=24 )
			menu->czas_OD_led1.godz = 0;
		menu->czas_OD_led1.godz += wartosc;
		wyswietl_napis("Czas LED1 ->");
		lcd_locate(1, 0);
		lcd_int(menu->czas_OD_led1.godz);
		lcd_char(':');
		if(menu->czas_OD_led1.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_OD_led1.min);
		}
		else
			lcd_int(menu->czas_OD_led1.min);
		break;
	case czas_led1_godz_DO:
		if( menu->czas_DO_led1.godz>=24 )
			menu->czas_DO_led1.godz = 0;
		menu->czas_DO_led1.godz += wartosc;
		wyswietl_napis("Czas LED1 <-");
		lcd_locate(1, 6);
		lcd_int(menu->czas_DO_led1.godz);
		lcd_char(':');
		if(menu->czas_DO_led1.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_DO_led1.min);
		}
		else
			lcd_int(menu->czas_DO_led1.min);
		break;
	case czas_led2_godz_OD:
		if( menu->czas_OD_led2.godz >=24 )
			menu->czas_OD_led2.godz = 0;
		menu->czas_OD_led2.godz += wartosc;
		wyswietl_napis("Czas LED2 ->");
		lcd_locate(1, 0);
		lcd_int(menu->czas_OD_led2.godz);
		lcd_char(':');
		if(menu->czas_OD_led2.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_OD_led2.min);
		}
		else
			lcd_int(menu->czas_OD_led2.min);
		break;
	case czas_led2_godz_DO:
		if( menu->czas_DO_led2.godz>=24 )
			menu->czas_DO_led2.godz = 0;
		menu->czas_DO_led2.godz += wartosc;
		wyswietl_napis("Czas LED2 <-");
		lcd_locate(1, 6);
		lcd_int(menu->czas_DO_led2.godz);
		lcd_char(':');
		if(menu->czas_DO_led2.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_DO_led2.min);
		}
		else
			lcd_int(menu->czas_DO_led2.min);
		break;
	case jasnosc_led1_ustaw:
		menu->jasnosc_led1 += wartosc;
		wyswietl_napis("Jasnosc LED1");
		lcd_locate(1, 0);
		lcd_int(menu->jasnosc_led1);
		OCR0A = menu->jasnosc_led1;
		break;
	case jasnosc_led2_ustaw:
		menu->jasnosc_led2 += wartosc;
		wyswietl_napis("Jasnosc LED2");
		lcd_locate(1, 0);
		lcd_int(menu->jasnosc_led2);
		OCR0B = menu->jasnosc_led2;
		break;
	case temperatura_ustaw:
		if( menu->temp >= 350 )
			menu->temp = 0;
		menu->temp += (uint16_t)wartosc;
		wyswietl_napis("Ustaw temp.");
		lcd_locate(1, 6);
		lcd_int( (menu->temp)/10 );
		lcd_char('.');
		lcd_int( (menu->temp)%10 );
		break;
	case czas_systemowy_godz:
		menu->czas.godz += wartosc;
		wyswietl_napis("Ustaw czas");
		lcd_locate(1, 0);
		lcd_int(menu->czas.godz);
		lcd_char(':');
		if(menu->czas.min <10) {
			lcd_int(0);
			lcd_int(menu->czas.min);
		}
		else
			lcd_int(menu->czas.min);
		break;
	case czas_systemowy_min:
		menu->czas.min += wartosc;
		wyswietl_napis("Ustaw czas");
		lcd_locate(1, 0);
		lcd_int(menu->czas.godz);
		lcd_char(':');
		if(menu->czas.min <10) {
			lcd_int(0);
			lcd_int(menu->czas.min);
		}
		else
			lcd_int(menu->czas.min);
		break;
	}
}

void menu_domyslne(menu_t *menu) {
	menu->temp = 200;
	menu->czas.godz = 12;
	menu->czas.min = 0;
	menu->czas_OD_led1.godz = 8;
	menu->czas_OD_led1.min = 0;
	menu->czas_DO_led1.godz = 18;
	menu->czas_DO_led1.min = 0;
	menu->czas_OD_led2.godz = 18;
	menu->czas_OD_led2.min = 0;
	menu->czas_DO_led2.godz = 20;
	menu->czas_DO_led2.min = 0;
	menu->jasnosc_led1 = 50;
	menu->jasnosc_led2 = 50;
	menu->czas_OD_wodospad.godz = 8;
	menu->czas_OD_wodospad.min = 0;
	menu->czas_DO_wodospad.godz = 20;
	menu->czas_DO_wodospad.min = 0;
}

void wyswietl_napis(char *str) {
	lcd_cls();
	lcd_locate(0,0);
	lcd_str(str);
}

void zapisz_ustawienia(menu_t *menu) {
	eeprom_write(0x0000, menu->czas_OD_wodospad.godz);
	eeprom_write(0x0001, menu->czas_OD_wodospad.min);
	eeprom_write(0x0002, menu->czas_DO_wodospad.godz);
	eeprom_write(0x0003, menu->czas_DO_wodospad.min);
	eeprom_write(0x0004, menu->jasnosc_led1);
	eeprom_write(0x0005, menu->jasnosc_led2);
	eeprom_write(0x0006, menu->czas_OD_led1.godz);
	eeprom_write(0x0007, menu->czas_OD_led1.min);
	eeprom_write(0x0008, menu->czas_DO_led1.godz);
	eeprom_write(0x0009, menu->czas_DO_led1.min);
	eeprom_write(0x000A, menu->czas_OD_led2.godz);
	eeprom_write(0x000B, menu->czas_OD_led2.min);
	eeprom_write(0x000C, menu->czas_DO_led2.godz);
	eeprom_write(0x000D, menu->czas_DO_led2.min);
	eeprom_write(0x000E, menu->temp);
}

void wczytaj_ustawienia(menu_t *menu) {
	menu->czas_OD_wodospad.godz = eeprom_read(0x0000);
	menu->czas_OD_wodospad.min = eeprom_read(0x0001);
	menu->czas_DO_wodospad.godz = eeprom_read(0x0002);
	menu->czas_DO_wodospad.min = eeprom_read(0x0003);
	menu->jasnosc_led1 = eeprom_read(0x0004);
	menu->jasnosc_led2 = eeprom_read(0x0005);
	menu->czas_OD_led1.godz = eeprom_read(0x0006);
	menu->czas_OD_led1.min = eeprom_read(0x0007);
	menu->czas_DO_led1.godz = eeprom_read(0x0008);
	menu->czas_DO_led1.min = eeprom_read(0x0009);
	menu->czas_OD_led2.godz = eeprom_read(0x000A);
	menu->czas_OD_led2.min = eeprom_read(0x000B);
	menu->czas_DO_led2.godz = eeprom_read(0x000C);
	menu->czas_DO_led2.min = eeprom_read(0x000D);
	menu->temp = eeprom_read(0x000E);
}
