#ifndef LCD_H_
#define LCD_H_

#define LCD_Y 2		
#define LCD_X 16	

#define USE_RW 0

#define LCD_D7PORT  B
#define LCD_D7 5
#define LCD_D6PORT  B
#define LCD_D6 4
#define LCD_D5PORT  B
#define LCD_D5 3
#define LCD_D4PORT  B
#define LCD_D4 2

#define LCD_RSPORT B
#define LCD_RS 0

#define LCD_RWPORT B
#define LCD_RW 1

#define LCD_EPORT B
#define LCD_E 1

#define USE_LCD_LOCATE	1			

#define USE_LCD_CHAR 	1			

#define USE_LCD_STR_P 	1			
#define USE_LCD_STR_E 	0			

#define USE_LCD_INT 	1			
#define USE_LCD_HEX 	1			

#define USE_LCD_DEFCHAR		0		
#define USE_LCD_DEFCHAR_P 	0		
#define USE_LCD_DEFCHAR_E 	0		

#define USE_LCD_CURSOR_ON 		0	
#define USE_LCD_CURSOR_BLINK 	0	
#define USE_LCD_CURSOR_HOME 	0	

#if ( (LCD_Y == 4) && (LCD_X == 16) )
#define LCD_LINE1 0x00		
#define LCD_LINE2 0x28		
#define LCD_LINE3 0x14  
#define LCD_LINE4 0x54  	
#else
#define LCD_LINE1 0x00		
#define LCD_LINE2 0x40		
#define LCD_LINE3 0x10  	
#define LCD_LINE4 0x50  	
#endif

#define PORT(x) SPORT(x)
#define SPORT(x) (PORT##x)

#define PIN(x) SPIN(x)
#define SPIN(x) (PIN##x)

#define DDR(x) SDDR(x)
#define SDDR(x) (DDR##x)

#define LCDC_CLS					0x01
#define LCDC_HOME					0x02
#define LCDC_ENTRY					0x04
	#define LCDC_ENTRYR					0x02
	#define LCDC_ENTRYL					0
	#define LCDC_MOVE					0x01
#define LCDC_ONOFF					0x08
	#define LCDC_DISPLAYON				0x04
	#define LCDC_CURSORON				0x02
	#define LCDC_CURSOROFF				0
	#define LCDC_BLINKON				0x01
#define LCDC_SHIFT					0x10
	#define LCDC_SHIFTDISP				0x08
	#define LCDC_SHIFTR					0x04
	#define LCDC_SHIFTL					0
#define LCDC_FUNC					0x20
	#define LCDC_FUNC8B					0x10
	#define LCDC_FUNC4B					0
	#define LCDC_FUNC2L					0x08
	#define LCDC_FUNC1L					0
	#define LCDC_FUNC5x10				0x04
	#define LCDC_FUNC5x7				0
#define LCDC_SET_CGRAM				0x40
#define LCDC_SET_DDRAM				0x80

void lcd_init(void);								
void lcd_cls(void);									
void lcd_str(char * str);							

void lcd_locate(uint8_t y, uint8_t x);				

void lcd_char(char c);								
void lcd_str_P(char * str);							
void lcd_str_E(char * str);							
void lcd_int(int val);								
void lcd_hex(int val);								
void lcd_defchar(uint8_t nr, uint8_t *def_znak);	
void lcd_defchar_P(uint8_t nr, uint8_t *def_znak);	
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak);	

void lcd_home(void);								
void lcd_cursor_on(void);							
void lcd_cursor_off(void);							
void lcd_blink_on(void);							
void lcd_blink_off(void);							

#endif
