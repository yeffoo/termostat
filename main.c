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

volatile uint16_t timer1; // ds18b20
volatile uint16_t timer3; // refresh heating, triac T1, LED's
volatile uint16_t timer4; // switch, debouncing
volatile uint16_t timer5; // menu timing
volatile uint16_t timer6; // switches UD
volatile uint16_t timer7; // ?? zmienic

extern uint8_t menu_pos;

volatile uint16_t rtc_flag;

volatile int8_t zmienna;

uint8_t subzero, cel, cel_fract_bits;
uint16_t temp_pobrana;

void gpio_init() {
	DDRD &= ~(RTC_INT | ENCODER_INT | BUTTON);
	PORTD |= (RTC_INT | ENCODER_INT | BUTTON);

	EIMSK |= (1 << INT0);	// RTC
	EICRA |= (1 << ISC01);

	EIMSK |= (1 << INT1);	// ENCODER_B
	EICRA |= (1 << ISC11);

	DDRC &= ~ENKODER_A;
	PORTC |= ENKODER_A;

	DDRC |= (1 << PC0); // LCD backlight
	PORTC |= (1 << PC0);

	DDRB |= T1_TRIAC | T2_TRIAC; // LCD backlight
	PORTB &= ~(T1_TRIAC | T2_TRIAC);
}

menu_t menu_gl;
czas_t czas_gl;
czas_t czas_akt;

#define LED1_CONNECTED TCCR0A |= (1 << COM0A1);
#define LED2_CONNECTED TCCR0A |= (1 << COM0B1);

#define LED1_DISCONNECTED TCCR0A &= ~(1 << COM0A1);
#define LED2_DISCONNECTED TCCR0A &= ~(1 << COM0B1);

int main() {
	eeprom_init();
//	menu_domyslne(&menu_gl);
//	zapisz_ustawienia(&menu_gl);
 	wczytaj_ustawienia(&menu_gl);

	lcd_init();

	search_sensors();

	DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
	_delay_ms(750);

	if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) {
		temp_pobrana = (cel*10)+cel_fract_bits;
		DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
	}

	gpio_init();
	timer0_init();
	timer2_init();

	PORTC &= ~(1 << PC0);

	sei();

	uint8_t state_flag = 0;

	BACKLIGHT_ON;

	while(1) {

// wejscie do menu, nastawy parametrow
		if(menu_pos &&  (!timer7)) {
//			BACKLIGHT_ON;
			menu_ustaw(menu_pos, zmienna, &menu_gl);
			if(zmienna) {
//				menu_ustaw(menu_pos, zmienna, &menu_gl);
				zmienna = 0;

			}
			timer7 = 100;
			state_flag = 1;
		}
		else {
// gdy ustawienie nieaktywne, to nastepuje zapis czasu do RTC oraz zapis nastaw do pamieci EEPROM
		if(	state_flag == 1 ) {
//				BACKLIGHT_OFF;
			state_flag = 0;
			czas_akt.godz = bin2bcd( menu_gl.czas.godz );
			czas_akt.min = bin2bcd( menu_gl.czas.min );
			pcf8583_set_time(&czas_akt);
			zapisz_ustawienia(&menu_gl);
		}
		}

// obsluga przycisku
// po 4s wejscie do menu, kazde kolejne przycisniecie powoduje przejscie do nastepnej pozycji w menu
		f_klaw(&PIND, BUTTON, 1000, 4000, 0, 0);

// odczyt temperatury
		if(!timer1) {
			timer1 = 750;
			if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) {
				temp_pobrana = (cel*10)+cel_fract_bits;
				DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
			}
		}

		if( rtc_flag && (!menu_pos) ) {
			rtc_flag = 0;
			pcf8583_read_time(&czas_gl);

			menu_gl.czas.godz = czas_gl.godz;//bcd_to_bin(minuty);
			menu_gl.czas.min = czas_gl.min;//bcd_to_bin(sekundy);
			menu_wyswietl_normalnie(&menu_gl, &zmienna);
		}
// grzanie, wodospad oraz oswietlenie
		if(!timer3) {
			timer3 = 5000;
			if(temp_pobrana < menu_gl.temp) {
				T1_ON; // grzanie wy³
			} else {
				T1_OFF; // grzanie w³
			}
			if( czas_gl.min >= menu_gl.czas_OD_wodospad.godz && czas_gl.min < menu_gl.czas_DO_wodospad.godz ) {
				T2_ON;
			} else {
				T2_OFF;
			}
			if( czas_gl.min >= menu_gl.czas_OD_led1.godz && czas_gl.min < menu_gl.czas_DO_led1.godz ) {
				LED1_CONNECTED;
			} else {
				LED1_DISCONNECTED;
			}
			if( czas_gl.min >= menu_gl.czas_OD_led2.godz && czas_gl.min < menu_gl.czas_DO_led2.godz ) {
				LED2_CONNECTED;
			} else {
				LED2_DISCONNECTED;
			}
		}
	}
}


