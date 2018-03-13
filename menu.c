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
		BACKLIGHT_OFF;
	}
	else
		BACKLIGHT_ON;

	if( key_state ) {
		if( key_press && debounce==key_state && !timer4 ) {
			timer5 = 4000;
			key_state = go_rep;
			timer4=3;
			last_key = key_mask;

			if( menu_pos ) {
				timer5 = 5000;
				menu_pos++;
			}
			if( menu_pos >= 14 )
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
	}
}

enum emenu_wyswietl {temperatura=1, czas_led1, czas_led2, jasnosc_led1, jasnosc_led2, czas_wodospad, czas_systemowy};

void menu_wyswietl_normalnie(menu_t *menu) {
	lcd_cls();
	wyswietl_napis("Temp: ");
	lcd_int(temp_pobrana/10);
	lcd_char('.');
	lcd_int(temp_pobrana%10);
	lcd_locate(1, 6);
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
				  czas_grzanie_godz_OD, czas_grzanie_godz_DO,
				  czas_led1_godz_OD, czas_led1_godz_DO,
				  czas_led2_godz_OD, czas_led2_godz_DO,
				  jasnosc_led1_ustaw,
				  jasnosc_led2_ustaw,
				  temperatura_ustaw,
				  czas_systemowy_godz, czas_systemowy_min};

void menu_ustaw(uint8_t wybor, int8_t wartosc, menu_t *menu) {
	switch(wybor) {
	case czas_wodospad_godz_OD:
		menu->czas_OD_wodospad.godz += wartosc;
		if( menu->czas_OD_wodospad.godz >= menu->czas_DO_wodospad.godz )
			menu->czas_OD_wodospad.godz = menu->czas_DO_wodospad.godz;
		if( menu->czas_OD_wodospad.godz > 23 )
			menu->czas_OD_wodospad.godz = 0;
		else if( menu->czas_OD_wodospad.godz < 0 )
			menu->czas_OD_wodospad.godz = 23;
		wyswietl_napis("Czas woda     ->");
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
		menu->czas_DO_wodospad.godz += wartosc;
		if( menu->czas_DO_wodospad.godz <= menu->czas_OD_wodospad.godz )
			menu->czas_DO_wodospad.godz = menu->czas_OD_wodospad.godz;
		if( menu->czas_DO_wodospad.godz > 23 )
			menu->czas_DO_wodospad.godz = 0;
		else if( menu->czas_DO_wodospad.godz < 0 )
			menu->czas_DO_wodospad.godz = 23;
		wyswietl_napis("Czas woda     <-");
		lcd_locate(1, 11);
		lcd_int(menu->czas_DO_wodospad.godz);
		lcd_char(':');
		if(menu->czas_DO_wodospad.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_DO_wodospad.min);
		}
		else
			lcd_int(menu->czas_DO_wodospad.min);
		break;
	case czas_grzanie_godz_OD:
		menu->czas_OD_grzanie.godz += wartosc;
		if( menu->czas_OD_grzanie.godz >= menu->czas_DO_grzanie.godz )
			menu->czas_OD_grzanie.godz = menu->czas_DO_grzanie.godz;
		if( menu->czas_OD_grzanie.godz > 23 )
			menu->czas_OD_grzanie.godz = 0;
		else if( menu->czas_OD_grzanie.godz < 0 )
			menu->czas_OD_grzanie.godz = 23;
		wyswietl_napis("Czas grzanie  ->");
		lcd_locate(1, 0);
		lcd_int(menu->czas_OD_grzanie.godz);
		lcd_char(':');
		if(menu->czas_OD_grzanie.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_OD_grzanie.min);
		}
		else
			lcd_int(menu->czas_OD_grzanie.min);
		break;
	case czas_grzanie_godz_DO:
		menu->czas_DO_grzanie.godz += wartosc;
		if( menu->czas_DO_grzanie.godz <= menu->czas_OD_grzanie.godz )
			menu->czas_DO_grzanie.godz = menu->czas_OD_grzanie.godz;
		if( menu->czas_DO_grzanie.godz > 23 )
			menu->czas_DO_grzanie.godz = 0;
		else if( menu->czas_DO_grzanie.godz < 0 )
			menu->czas_DO_grzanie.godz = 23;
		wyswietl_napis("Czas grzanie  <-");
		lcd_locate(1, 11);
		lcd_int(menu->czas_DO_grzanie.godz);
		lcd_char(':');
		if(menu->czas_DO_grzanie.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_DO_grzanie.min);
		}
		else
			lcd_int(menu->czas_DO_grzanie.min);
		break;
	case czas_led1_godz_OD:
		menu->czas_OD_led1.godz += wartosc;
		if( menu->czas_OD_led1.godz >= menu->czas_DO_led1.godz )
			menu->czas_OD_led1.godz = menu->czas_DO_led1.godz;
		if( menu->czas_OD_led1.godz > 23 )
			menu->czas_OD_led1.godz = 0;
		else if( menu->czas_OD_led1.godz < 0 )
			menu->czas_OD_led1.godz = 23;
		wyswietl_napis("Czas LED1     ->");
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
		menu->czas_DO_led1.godz += wartosc;
		if( menu->czas_DO_led1.godz <= menu->czas_OD_led1.godz )
			menu->czas_DO_led1.godz = menu->czas_OD_led1.godz;
		if( menu->czas_DO_led1.godz > 23 )
			menu->czas_DO_led1.godz = 0;
		else if( menu->czas_DO_led1.godz < 0 )
			menu->czas_DO_led1.godz = 23;
		wyswietl_napis("Czas LED1     <-");
		lcd_locate(1, 11);
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
		menu->czas_OD_led2.godz += wartosc;
		if( menu->czas_OD_led2.godz >= menu->czas_DO_led2.godz )
			menu->czas_OD_led2.godz = menu->czas_DO_led2.godz;
		if( menu->czas_OD_led2.godz > 23 )
			menu->czas_OD_led2.godz = 0;
		else if( menu->czas_OD_led2.godz < 0 )
			menu->czas_OD_led2.godz = 23;
		wyswietl_napis("Czas LED2     ->");
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
		menu->czas_DO_led2.godz += wartosc;
		if( menu->czas_DO_led2.godz <= menu->czas_OD_led2.godz )
			menu->czas_DO_led2.godz = menu->czas_OD_led2.godz;
		if( menu->czas_DO_led2.godz > 23 )
			menu->czas_DO_led2.godz = 0;
		else if( menu->czas_DO_led2.godz < 0 )
			menu->czas_DO_led2.godz = 23;
		wyswietl_napis("Czas LED2     <-");
		lcd_locate(1, 11);
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
		lcd_locate(1, 7);
		lcd_int(menu->jasnosc_led1);
		OCR0A = menu->jasnosc_led1;
		break;
	case jasnosc_led2_ustaw:
		menu->jasnosc_led2 += wartosc;
		wyswietl_napis("Jasnosc LED2");
		lcd_locate(1, 7);
		lcd_int(menu->jasnosc_led2);
		OCR0B = menu->jasnosc_led2;
		break;
	case temperatura_ustaw:
		menu->temp += (uint16_t)wartosc;
		if( menu->temp >= 370 )
			menu->temp = 0;
		wyswietl_napis("Ustaw temp.");
		lcd_locate(1, 6);
		lcd_int( (menu->temp)/10 );
		lcd_char('.');
		lcd_int( (menu->temp)%10 );
		break;
	case czas_systemowy_godz:
		menu->czas.godz += wartosc;
		if( menu->czas.godz > 23 )
			menu->czas.godz = 0;
		else if( menu->czas.godz < 0 )
			menu->czas.godz = 23;
		wyswietl_napis("Ustaw czas godz");
		lcd_locate(1, 5);
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
		if( menu->czas.min > 59 )
			menu->czas.min = 0;
		else if( menu->czas.min < 0 )
			menu->czas.min = 59;
		wyswietl_napis("Ustaw czas min");
		lcd_locate(1, 5);
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
	menu->temp = 290;
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
	menu->czas_OD_grzanie.godz = 7;
	menu->czas_OD_grzanie.min = 0;
	menu->czas_DO_grzanie.godz = 19;
	menu->czas_DO_grzanie.min = 0;
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
	eeprom_write(0x000E, menu->czas_OD_grzanie.godz);
	eeprom_write(0x000F, menu->czas_OD_grzanie.min);
	eeprom_write(0x0010, menu->czas_DO_grzanie.godz);
	eeprom_write(0x0011, menu->czas_DO_grzanie.min);
	eeprom_write(0x0012, (menu->temp));
	eeprom_write(0x0013, (menu->temp >> 8));
}

void wczytaj_ustawienia(menu_t *menu) {
	uint8_t temp;
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
	menu->czas_OD_grzanie.godz = eeprom_read(0x000E);
	menu->czas_OD_grzanie.min = eeprom_read(0x000F);
	menu->czas_DO_grzanie.godz = eeprom_read(0x0010);
	menu->czas_DO_grzanie.min = eeprom_read(0x0011);
	temp = eeprom_read(0x0012);
	menu->temp = (temp);
	temp = eeprom_read(0x0013);
	menu->temp |= (temp << 8);
}
