/*
 * menu.c
 *
 *  Created on: 21-01-2018
 *      Author: Jeff
 */

#include "menu.h"

extern uint8_t subzero, cel, cel_fract_bits;
extern uint16_t temp_pobrana;

extern volatile uint16_t timer4; // switch
extern volatile uint16_t timer5; // menu
extern volatile uint16_t timer6; // switches UD

uint8_t menu_state, menu_pos;

void f_klaw_UD(volatile uint8_t *KPIN, uint8_t key_mask, uint16_t time, void (*func)(void)) {
	static uint8_t key_state;
	static uint8_t last_key_state;
	uint8_t key_press;
	key_press = !(*KPIN & key_mask);

	if( key_press && !key_state ) {
		timer6 = 20;
		key_state = 1; // debouncing
	}
	if( key_press && (key_state == 1) ) {
		timer6 = time;
		key_state = 2;
//		func();
	}
	if ( key_press && (key_state == 2) && !timer6 ) {
		key_state = 0;
		func();
	}
}

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
	if( key_state  ) {
		if( key_press && debounce==key_state && !timer4 ) {
			key_state = go_rep;
			timer4=3;
			last_key = key_mask;
			if(timer5) {
				timer5 = 5000;
				menu_pos++;
			}
		} else
		if( !key_press && key_state>debounce && key_state<rep ) {
			if(push_proc) {
//				menu_pos++;
//				push_proc();						/* KEY_UP */
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
			if(rep_proc) { // po 4s wchodzi do menu
//				menu_state = 1;
				timer5 = 5000;
				rep_proc();						/* KEY_REP */
			}
		}
	}

	if( key_state>=wait_rep && !key_press ) {
		key_state = idle;
		last_key = 0;
	}
}

enum emenu_ustaw {czas_wodospad_godz_OD, czas_wodospad_min_OD, czas_wodospad_godz_DO, czas_wodospad_min_DO,
				  czas_led1_godz_OD, czas_led1_min_OD, czas_led1_godz_DO, czas_led1_min_DO,
				  czas_led2_godz_OD, czas_led2_min_OD, czas_led2_godz_DO, czas_led2_min_DO,
				  jasnosc_led1_ustaw,
				  jasnosc_led2_ustaw,
				  czas_systemowy_ustaw,
				  temperatura_ustaw};

enum emenu_wyswietl {temperatura=1, czas_led1, czas_led2, jasnosc_led1, jasnosc_led2, czas_wodospad, czas_systemowy};

extern menu_t menu_gl;

void dodaj() {
	menu_gl.temp++;
}

void menu_wyswietl(uint8_t wybor, menu_t *menu) {
//	enum emenu {temp, czas_led1, czas_led2, jasnosc_led1, jasnosc_led2, czas_wodospad, czas};
	switch(wybor) {
	case temperatura:
		f_klaw_UD(&PIND, (1 << PD1), 200, dodaj);
		wyswietl_napis("Ustaw temp.");
		lcd_locate(1, 6);
		lcd_int(menu->temp);
		break;
	case czas_led1:
		wyswietl_napis("Czas LED1");
		lcd_locate(1, 0);
		lcd_int(menu->czas_OD_led1.godz);
		lcd_char(':');
//		lcd_int(menu->czas_OD_led1.min);
		if(menu->czas_OD_led1.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_OD_led1.min);
		}
		else
			lcd_int(menu->czas_OD_led1.min);
		lcd_locate(1, 8);
		lcd_int(menu->czas_DO_led1.godz);
		lcd_char(':');
//		lcd_int(menu->czas_DO_led1.min);
		if(menu->czas_DO_led1.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_DO_led1.min);
		}
		else
			lcd_int(menu->czas_DO_led1.min);
		break;
	case czas_led2:
		wyswietl_napis("Czas LED2");
		lcd_locate(1, 0);
		lcd_int(menu->czas_OD_led2.godz);
		lcd_char(':');
		if(menu->czas_OD_led2.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_OD_led2.min);
		}
		else
			lcd_int(menu->czas_OD_led2.min);
//		lcd_int(menu->czas_OD_led2.min);
		lcd_locate(1, 8);
		lcd_int(menu->czas_DO_led2.godz);
		lcd_char(':');
//		lcd_int(menu->czas_DO_led2.min);
		if(menu->czas_DO_led2.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_DO_led2.min);
		}
		else
			lcd_int(menu->czas_DO_led2.min);

		break;
	case jasnosc_led1:
		wyswietl_napis("Jasnosc LED1");
		lcd_locate(1, 7);
		lcd_int(menu->jasnosc_led1);
		break;
	case jasnosc_led2:
		wyswietl_napis("Jasnosc LED2");
		lcd_locate(1, 7);
		lcd_int(menu->jasnosc_led2);
		break;
	case czas_wodospad:
		wyswietl_napis("Czas wodospad");
		lcd_locate(1, 0);
		lcd_int(menu->czas_OD_wodospad.godz);
		lcd_char(':');
//		lcd_int(menu->czas_OD_wodospad.min);
		if(menu->czas_OD_wodospad.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_OD_wodospad.min);
		}
		else
			lcd_int(menu->czas_OD_wodospad.min);
		lcd_locate(1, 8);
		lcd_int(menu->czas_DO_wodospad.godz);
		lcd_char(':');
//		lcd_int(menu->czas_DO_wodospad.min);
		if(menu->czas_DO_wodospad.min <10) {
			lcd_int(0);
			lcd_int(menu->czas_DO_wodospad.min);
		}
		else
			lcd_int(menu->czas_DO_wodospad.min);
		break;
	case czas_systemowy:
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
	default:
		wyswietl_napis("Temp.: ");
		lcd_int(temp_pobrana/10);
		lcd_char('.');
		lcd_int(temp_pobrana%10);
		lcd_locate(0, 14);
//		lcd_int(i);
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

extern uint8_t i;

uint8_t menu_ustaw() {
	if( (menu_pos == 1) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(1);
		return 1;
	}
	else if( (menu_pos == 2) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(2);
		return 2;
	}
	else if( (menu_pos == 3) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(3);
		return 3;
	}
	else if( (menu_pos == 4) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(4);
		return 4;
	}
	else if( (menu_pos == 5) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(5);
		return 5;
	}
	else if( (menu_pos == 6) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(6);
		return 6;
	}
	else if( (menu_pos == 7) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(7);
		return 7;
	}
	else if( (menu_pos == 8) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(8);
		return 8;
	}
	else if( (menu_pos == 9) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(9);
		return 9;
	}
	else if( (menu_pos == 10) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(10);
		return 10;
	}
	else if( (menu_pos == 11) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(11);
		return 11;
	}
	else if( (menu_pos == 12) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(12);
		return 12;
	}
	else if( (menu_pos == 13) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(13);
		return 13;
	}
	else if( (menu_pos == 14) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(14);
		return 14;
	}
	else if( (menu_pos == 15) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(15);
		return 15;
	}
	else if( (menu_pos == 16) && timer5 ) {
		//timer5 = 5000;
		lcd_locate(1, 14);
		lcd_int(16);
		return 16;
	}
	else if( (menu_pos == 17) && timer5 ) {
		//timer5 = 5000;
		menu_pos = 1;
		lcd_locate(1, 14);
		lcd_int(17);
		return 17;
	}
	else if( !timer5 ) {
		menu_pos = 0;
//		lcd_locate(0, 13);
//		lcd_char('_');
		return 255;
	}
	return 0;
}

void menu_domyslne(menu_t *menu) {
	menu->temp = 200;
	menu->czas.godz = 12;
	menu->czas.min = 30;
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

void ustaw_temp(menu_t *menu, uint8_t temp) {
	menu->temp = temp;
}

void ustaw_czas(czas_t czas) {
	czas.godz = 12;
	czas.min = 30;
}

void ustaw_czas_led1(menu_t *menu, czas_t czas_od, czas_t czas_do) {
	menu->czas_OD_led1 = czas_od;
	menu->czas_DO_led1 = czas_do;
}

void ustaw_czas_led2(menu_t *menu, czas_t czas_od, czas_t czas_do) {
	menu->czas_OD_led2 = czas_od;
	menu->czas_DO_led2 = czas_do;
}

void ustaw_jasnosc_led1(menu_t *menu, uint8_t jasnosc) {
	menu->jasnosc_led1 = jasnosc;
}

void ustaw_jasnosc_led2(menu_t *menu, uint8_t jasnosc) {
	menu->jasnosc_led2 = jasnosc;
}

void ustaw_czas_wodospad(menu_t *menu, czas_t czas_od, czas_t czas_do) {
	menu->czas_OD_wodospad.godz = czas_od.godz;
	menu->czas_OD_wodospad.min = czas_od.min;
	menu->czas_DO_wodospad.godz = czas_do.godz;
	menu->czas_DO_wodospad.min = czas_do.min;
}

void przekaznik(uint8_t wybor) {

}

void triak(uint8_t wybor) {

}

void wyswietl_napis(char *str) {
	lcd_cls();
	lcd_locate(0,0);
	lcd_str(str);
}

//enum

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
	eeprom_write(0x000A, menu->czas_OD_led1.godz);
	eeprom_write(0x000B, menu->czas_OD_led1.min);
	eeprom_write(0x000C, menu->czas_DO_led1.godz);
	eeprom_write(0x000D, menu->czas_DO_led1.min);
	eeprom_write(0x000E, menu->temp);
}

menu_t wczytaj_ustawienia() {
	menu_t menu;
	menu.czas_OD_wodospad.godz = eeprom_read(0x0000);
	menu.czas_OD_wodospad.min = eeprom_read(0x0001);
	menu.czas_DO_wodospad.godz = eeprom_read(0x0002);
	menu.czas_DO_wodospad.min = eeprom_read(0x0003);
	menu.jasnosc_led1 = eeprom_read(0x0004);
	menu.jasnosc_led2 = eeprom_read(0x0005);
	menu.czas_OD_led1.godz = eeprom_read(0x0006);
	menu.czas_OD_led1.min = eeprom_read(0x0007);
	menu.czas_DO_led1.godz = eeprom_read(0x0008);
	menu.czas_DO_led1.min = eeprom_read(0x0009);
	menu.czas_OD_led1.godz = eeprom_read(0x000A);
	menu.czas_OD_led1.min = eeprom_read(0x000B);
	menu.czas_DO_led1.godz = eeprom_read(0x000C);
	menu.czas_DO_led1.min = eeprom_read(0x000D);
	menu.temp = eeprom_read(0x000E);
	return menu;
}
