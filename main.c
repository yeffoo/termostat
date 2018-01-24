/*
 * main.c
 *
 *  Created on: 16-01-2018
 *      Author: Jeff
 */

#include "main.h"
#include "timer.h"
#include "1Wire/ds18x20.h"
#include "twi.h"
#include "pcf8583.h"

#include "menu.h"
#include "eeprom.h"

volatile uint16_t tick_1ms;
volatile uint16_t timer1; // ds18b20
volatile uint16_t timer2; // refresh LCD
volatile uint16_t timer3; // refresh heating
volatile uint16_t timer4; // switch
volatile uint16_t timer5; // menu
volatile uint16_t timer6; // switches UD

extern uint8_t menu_state, menu_pos;

volatile uint16_t rtc_flag;
volatile uint16_t button_flag;

// PCINT6
#define RTC_INT (1 << PD2)
#define BUTTON	(1 << PD3)
#define RELAY
#define TRIAC

#define BUTTON_UP	(1 << PD1)
#define BUTTON_DOWN	(1 << PD4)

uint8_t subzero, cel, cel_fract_bits;
uint16_t temp_pobrana;

uint8_t i;

void gpio_init() {
	DDRD &= ~(RTC_INT | BUTTON | BUTTON_UP | BUTTON_DOWN);
	PORTD |= (RTC_INT | BUTTON | BUTTON_UP | BUTTON_DOWN);
	EIMSK |= (1 << INT0);// | (1 << INT1);
	EICRA |= (1 << ISC01);// | (1 << ISC11);
	// PCINT20
//	PCMSK2 |= (1 << PCINT20);
//	PCICR |= (1 << PCIE2);
//	PORTD |= BUTTON;
}

menu_t menu_gl;
czas_t czas_gl;
czas_t czas_akt;

uint8_t i;

void wejdz_menu() {
	i++;
}

uint8_t zmienna;

void wartoc_dodaj() {
	if(i == 1) {
		menu_gl.temp++;
		timer5 = 5000;
	}
	else if(i == 2) {
		menu_gl.czas_OD_led1.godz++;
		timer5 = 5000;
	}
	zmienna++;
}

void wartoc_odejmij() {
	if(i == 1) {
		menu_gl.temp--;
		timer5 = 5000;
	}
	else if(i == 2) {
		menu_gl.czas_OD_led1.godz--;
		timer5 = 5000;
	}
	zmienna--;
}

int main() {
//	menu_domyslne(&menu_gl);
//	zapisz_ustawienia(&menu_gl);
 	menu_gl = wczytaj_ustawienia(&menu_gl);

	lcd_init();

	search_sensors();

	DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
	_delay_ms(750);

	if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) {
		temp_pobrana = (cel*10)+cel_fract_bits;
		DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
	}

	gpio_init();
//	timer0_init();
	timer2_init();

//	czas_akt.godz = bin2bcd(16);
//	czas_akt.min = bin2bcd(33);
//	pcf8583_set_time(&czas_akt);

	sei();

	while(1) {

//		if( (menu_state == 1) ) {
//			timer5 = 5000;
//			lcd_locate(1, 14);
//			lcd_int(0);
//		}

		if(menu_pos)
			i = menu_ustaw();
		else
			i = 255;
		f_klaw(&PIND, BUTTON, 2000, 3000, wejdz_menu, wejdz_menu);
		f_klaw_UD(&PIND, BUTTON_UP, 500, wartoc_dodaj);
		f_klaw_UD(&PIND, BUTTON_DOWN, 500, wartoc_odejmij);

		if(!timer1) {
			timer1 = 750;
			if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) {
				temp_pobrana = (cel*10)+cel_fract_bits;
				DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
			}

//			if( (menu_gl->czas_OD_wodospad.godz >= menu_gl->czas.godz) && (menu_gl->czas_OD_wodospad.min >= menu_gl->czas.min) ) {
//				przekaznik(1);
//			}
//			else if ( (menu_gl->czas_OD_wodospad.godz < menu_gl->czas.godz) && (menu_gl->czas_OD_wodospad.min < menu_gl->czas.min) )
//				przekaznik(0);


//			menu(255, &menu_gl);
		}

		if( rtc_flag ) {
			rtc_flag = 0;
			pcf8583_read_time(&czas_gl);

			menu_gl.czas.godz = czas_gl.godz;//bcd_to_bin(minuty);
			menu_gl.czas.min = czas_gl.min;//bcd_to_bin(sekundy);
			menu_wyswietl(i, &menu_gl);
//			lcd_locate(0, 14);
//			lcd_int(zmienna);
//			if(i >= 8)
//				i = 0;
		}
//		if(!timer2) {
//			timer2 = 200;
//			menu(255, menu_gl);
//		}
//
//		if(!timer3) {
//			timer3 = 5000;
//			if(temp_pobrana >= menu_gl->temp) {
//				triak(0);
//			}
//			else {
//				triak(1);
//			}
//		}
	}
}


