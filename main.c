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

volatile uint8_t tick_1ms;
volatile uint16_t timer1; // ds18b20
volatile uint16_t timer2; // refresh LCD
volatile uint16_t timer3; // refresh heating, triac T1
volatile uint16_t timer4; // switch
volatile uint16_t timer5; // menu
volatile uint16_t timer6; // switches UD
volatile uint16_t timer7; // refresh water

extern uint8_t menu_pos;

volatile uint16_t rtc_flag;
volatile uint16_t button_flag;

uint8_t zmienna;

#include "enkoder.h"

static inline void enkoderInit() {
        ENKODER_DDR &= ~(ENKODER_A | ENKODER_B);// | ENKODER_SW); // Piny enkodera jako wejœcie.
        ENKODER_PORT |= ENKODER_A | ENKODER_B;// | ENKODER_SW; // Wewnêtrzne pullupy.
}

encState_t readEncoder() {
        encState_t tmp;
        if (ENKODER_PIN & ENKODER_A) { // Stan na pinie A
                tmp.A = 1;
        } else {
                tmp.A = 0;
        }
        if (ENKODER_PIN & ENKODER_B) { // Stan na pinie B
                tmp.B = 1;
        } else {
                tmp.B = 0;
        }
//        if (ENKODER_PIN & ENKODER_SW) { // Jeœli przycisk nie wciœniêty stan wysoki.
//                tmp.SW = 1; // Przycisk nie wciœniêty.
//        } else {
//                tmp.SW = 0; // Przycisk wciœniêty.
//        }
        return tmp;
}

void encoderEvent(encEvent_t const event) {
        switch (event.rotate) {
        case CW: // Ruch w kierunku wskazówek zegara.
//                lcd_locate(0, 14);
//                lcd_int(1);
                zmienna++;
                timer5 = 4000;
                break;
        case CCW: // Ruch w przeciwnym kierunku do wskazówek zegara.
//				lcd_locate(0, 14);
//				lcd_int(0);
        		timer5 = 4000;
				zmienna--;
                break;
        default:
                break;
        }
        switch (event.button) {
        case SHORT: // Krótkie wciœniêcie przycisku.
//                OCR0A = 0;
                break;
        case LONG: // D³u¿sze wciœniêcie przycisku.
//                OCR0A = 127;
                break;
        case VERY_LONG: // Bardzo d³ugie wciœniêcie przycisku.
//                OCR0A = 255;
                break;
        default:
                break;
        }
}

uint8_t subzero, cel, cel_fract_bits;
uint16_t temp_pobrana;

uint8_t i;

void gpio_init() {
	DDRD &= ~(RTC_INT | BUTTON);
	PORTD |= (RTC_INT | BUTTON);
	EIMSK |= (1 << INT0);// | (1 << INT1);
	EICRA |= (1 << ISC01);// | (1 << ISC11);

//	DDRC &= ~(BUTTON_UP | BUTTON_DOWN);
//	PORTC |= BUTTON_UP | BUTTON_DOWN;

	DDRC |= (1 << PC0); // LCD backlight
	PORTC |= (1 << PC0);

	DDRB |= T1_TRIAC | T2_TRIAC; // LCD backlight
	PORTB &= ~(T1_TRIAC | T2_TRIAC);
}

menu_t menu_gl;
czas_t czas_gl;
czas_t czas_akt;

uint8_t i;

void wejdz_menu() {
//	i++;
//	menu_pos++;
}


void wartoc_dodaj() {
	zmienna++;
}

void wartoc_odejmij() {
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

//
    regEncState(readEncoder); // Rejestracja funkcji odczytuj¹ce stan enkodera.
    regEncAction(encoderEvent); // Rejestracja funkcji wywo³ywanej podczas jakiegoœ zdarzenia enkodera.

    enkoderInit(); // Inicjalizacja pinów enkodera.
//
	gpio_init();
	timer0_init();
	timer2_init();

	PORTC &= ~(1 << PC0);
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

		if(menu_pos && (!timer7)) {
//
//			tick_1ms = 0;
			menu_pobierz_wartosc(menu_pos, &zmienna, &menu_gl);
			menu_ustaw(menu_pos, zmienna, &menu_gl);
			timer7 = 100;
//			_delay_ms(100);
		}

		f_klaw(&PIND, BUTTON, 1000, 4000, 0, wejdz_menu);

		if(!timer1) {
			timer1 = 750;
			if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) {
				temp_pobrana = (cel*10)+cel_fract_bits;
				DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );
			}

		}

		if( rtc_flag && (!menu_pos) ) {
			PORTB ^= (T1_TRIAC | T2_TRIAC);
			rtc_flag = 0;
			pcf8583_read_time(&czas_gl);

			menu_gl.czas.godz = czas_gl.godz;//bcd_to_bin(minuty);
			menu_gl.czas.min = czas_gl.min;//bcd_to_bin(sekundy);
			menu_wyswietl_normalnie(&menu_gl);

			if( (czas_gl.godz >= menu_gl.czas_OD_wodospad.godz) ) {// && (czas_gl.godz < menu_gl.czas_DO_wodospad.godz) ) {
				if( (czas_gl.godz < menu_gl.czas_DO_wodospad.godz) )
					triak_t2(1);
				else
					triak_t2(0);
			}
			else
				triak_t2(0);
		}
//		if(!timer2) {
//			timer2 = 200;
//			menu(255, menu_gl);
//		}
//
		if(!timer3) {
			timer3 = 5000;
			if(temp_pobrana >= menu_gl.temp) {
				triak_t1(0);
			}
			else {
				triak_t1(1);
			}
		}
	}
}


