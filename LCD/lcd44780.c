#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>

#include "lcd44780.h"

#define SET_RS 	PORT(LCD_RSPORT) |= (1<<LCD_RS)			
#define CLR_RS 	PORT(LCD_RSPORT) &= ~(1<<LCD_RS)		

#define SET_RW 	PORT(LCD_RWPORT) |= (1<<LCD_RW)			
#define CLR_RW 	PORT(LCD_RWPORT) &= ~(1<<LCD_RW)		

#define SET_E 	PORT(LCD_EPORT) |= (1<<LCD_E)		
#define CLR_E 	PORT(LCD_EPORT) &= ~(1<<LCD_E)		

uint8_t check_BF(void);

static inline void data_dir_out(void) {
	DDR(LCD_D7PORT)	|= (1<<LCD_D7);
	DDR(LCD_D6PORT)	|= (1<<LCD_D6);
	DDR(LCD_D5PORT)	|= (1<<LCD_D5);
	DDR(LCD_D4PORT)	|= (1<<LCD_D4);
}

static inline void data_dir_in(void) {
	DDR(LCD_D7PORT)	&= ~(1<<LCD_D7);
	DDR(LCD_D6PORT)	&= ~(1<<LCD_D6);
	DDR(LCD_D5PORT)	&= ~(1<<LCD_D5);
	DDR(LCD_D4PORT)	&= ~(1<<LCD_D4);
}

static inline void lcd_sendHalf(uint8_t data)  {
	if (data&(1<<0)) PORT(LCD_D4PORT) |= (1<<LCD_D4); else PORT(LCD_D4PORT) &= ~(1<<LCD_D4);
	if (data&(1<<1)) PORT(LCD_D5PORT) |= (1<<LCD_D5); else PORT(LCD_D5PORT) &= ~(1<<LCD_D5);
	if (data&(1<<2)) PORT(LCD_D6PORT) |= (1<<LCD_D6); else PORT(LCD_D6PORT) &= ~(1<<LCD_D6);
	if (data&(1<<3)) PORT(LCD_D7PORT) |= (1<<LCD_D7); else PORT(LCD_D7PORT) &= ~(1<<LCD_D7);
}

#if USE_RW == 1

static inline uint8_t lcd_readHalf(void) {
	uint8_t result=0;

	if(PIN(LCD_D4PORT)&(1<<LCD_D4)) result |= (1<<0);
	if(PIN(LCD_D5PORT)&(1<<LCD_D5)) result |= (1<<1);
	if(PIN(LCD_D6PORT)&(1<<LCD_D6)) result |= (1<<2);
	if(PIN(LCD_D7PORT)&(1<<LCD_D7)) result |= (1<<3);

	return result;
}
#endif

void _lcd_write_byte(unsigned char _data) {
	data_dir_out();

#if USE_RW == 1
	CLR_RW;
#endif

	SET_E;
	lcd_sendHalf(_data >> 4);			
	CLR_E;

	SET_E;
	lcd_sendHalf(_data);				
	CLR_E;

#if USE_RW == 1
	while( (check_BF() & (1<<7)) );
#else
	_delay_us(120);
#endif

}

#if USE_RW == 1

uint8_t _lcd_read_byte(void) {
	uint8_t result=0;
	data_dir_in();

	SET_RW;

	SET_E;
	result = (lcd_readHalf() << 4);	
	CLR_E;

	SET_E;
	result |= lcd_readHalf();			
	CLR_E;

	return result;
}
#endif


#if USE_RW == 1

uint8_t check_BF(void) {
	CLR_RS;
	return _lcd_read_byte();
}
#endif

void lcd_write_cmd(uint8_t cmd) {
	CLR_RS;
	_lcd_write_byte(cmd);
}

void lcd_write_data(uint8_t data) {
	SET_RS;
	_lcd_write_byte(data);
}

#if USE_LCD_CHAR == 1

void lcd_char(char c) {
	lcd_write_data( ( c>=0x80 && c<=0x87 ) ? (c & 0x07) : c);
}
#endif

void lcd_str(char * str) {
	register char znak;
	while ( (znak=*(str++)) )
		lcd_write_data( ( znak>=0x80 && znak<=0x87 ) ? (znak & 0x07) : znak);
}

#if USE_LCD_STR_P == 1

void lcd_str_P(char * str) {
	register char znak;
	while ( (znak=pgm_read_byte(str++)) )
		lcd_write_data( ( (znak>=0x80) && (znak<=0x87) ) ? (znak & 0x07) : znak);
}
#endif


#if USE_LCD_STR_E == 1

void lcd_str_E(char * str) {
	register char znak;
	while(1)
	{
		znak=eeprom_read_byte( (uint8_t *)(str++) );
		if(!znak || znak==0xFF) break;
		else lcd_write_data( ( (znak>=0x80) && (znak<=0x87) ) ? (znak & 0x07) : znak);
	}
}
#endif


#if USE_LCD_INT == 1

void lcd_int(int val) {
	char bufor[17];
	lcd_str( utoa(val, bufor, 10) );
}
#endif

#if USE_LCD_HEX == 1

void lcd_hex(int val)
{
	char bufor[17];
	lcd_str( itoa(val, bufor, 16) );
}
#endif

#if USE_LCD_DEFCHAR == 1

void lcd_defchar(uint8_t nr, uint8_t *def_znak) {
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++) {
		c = *(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_DEFCHAR_P == 1

void lcd_defchar_P(uint8_t nr, uint8_t *def_znak) {
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++) {
		c = pgm_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_DEFCHAR_E == 1

void lcd_defchar_E(uint8_t nr, uint8_t *def_znak) {
	register uint8_t i,c;

	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = eeprom_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_LOCATE == 1

void lcd_locate(uint8_t y, uint8_t x) {
	switch(y) {
		case 0: y = LCD_LINE1; break;

#if (LCD_Y>1)
	    case 1: y = LCD_LINE2; break; 
#endif
#if (LCD_Y>2)
    	case 2: y = LCD_LINE3; break; 
#endif
#if (LCD_Y>3)
    	case 3: y = LCD_LINE4; break; 
#endif
	}

	lcd_write_cmd( (0x80 + y + x) );
}
#endif

void lcd_cls(void) {
	lcd_write_cmd( LCDC_CLS );

	#if USE_RW == 0
		_delay_ms(4.9);
	#endif
}

#if USE_LCD_CURSOR_HOME == 1

void lcd_home(void) {
	lcd_write_cmd( LCDC_CLS|LCDC_HOME );

	#if USE_RW == 0
		_delay_ms(4.9);
	#endif
}
#endif

#if USE_LCD_CURSOR_ON == 1

void lcd_cursor_on(void) {
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON);
}

void lcd_cursor_off(void) {
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON);
}
#endif


#if USE_LCD_CURSOR_BLINK == 1

void lcd_blink_on(void) {
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON|LCDC_BLINKON);
}

void lcd_blink_off(void) {
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON);
}
#endif

void lcd_init(void) {
	data_dir_out();
	DDR(LCD_RSPORT) |= (1<<LCD_RS);
	DDR(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
		DDR(LCD_RWPORT) |= (1<<LCD_RW);
	#endif

	PORT(LCD_RSPORT) |= (1<<LCD_RS);
	PORT(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
		PORT(LCD_RWPORT) |= (1<<LCD_RW);
	#endif

	_delay_ms(15);
	PORT(LCD_EPORT) &= ~(1<<LCD_E);
	PORT(LCD_RSPORT) &= ~(1<<LCD_RS);

	SET_E;
	lcd_sendHalf(0x03);	
	CLR_E;
	_delay_ms(4.1);

	SET_E;
	lcd_sendHalf(0x03);	
	CLR_E;
	_delay_us(100);

	SET_E;
	lcd_sendHalf(0x03);	
	CLR_E;
	_delay_us(100);

	SET_E;
	lcd_sendHalf(0x02);
	CLR_E;
	_delay_us(100);

	lcd_write_cmd( LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7 );

	lcd_write_cmd( LCDC_ONOFF|LCDC_CURSOROFF );

	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON );

	lcd_write_cmd( LCDC_ENTRY|LCDC_ENTRYR );

	lcd_cls();
}
