/*
 * lcd.h
 *
 * Created: 6/8/2015 12:03:39 PM
 *  Author: zgoyetche
 */ 


#ifndef LCD_H_
#define LCD_H_

#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <math.h>
/***************************************************************************/
/*                       define the wanted LCD type:                       */
/***************************************************************************/
//#define LCD_1X8
//#define LCD_1X16
//#define LCD_1X20
//#define LCD_1X40
//#define LCD_2X8       // some 1x16 are wired as 2x8
//#define LCD_2X12
#define LCD_2X16
//#define LCD_2X20
//#define LCD_2X24
//#define LCD_2X40
//#define LCD_4X16
//#define LCD_4X20

/***************************************************************************/
/*                       define the LCD connections:                       */
/***************************************************************************/
struct bits
{
	uint8_t b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
}
__attribute__((__packed__));

#define SBIT_(port,pin) ((*(volatile struct bits*)&port).b##pin)
#define	SBIT(x,y)       SBIT_(x,y)

#define	LCD_D4		SBIT(PORTB, 5)
#define	LCD_DDR_D4	SBIT(DDRB, 5)

#define	LCD_D5		SBIT(PORTB, 4)
#define	LCD_DDR_D5	SBIT(DDRB, 4)

#define	LCD_D6		SBIT(PORTB, 3)
#define	LCD_DDR_D6	SBIT(DDRB, 3)

#define	LCD_D7		SBIT(PORTB, 2)
#define	LCD_DDR_D7	SBIT(DDRB, 2)

#define	LCD_RS		SBIT(PORTB, 0)
#define	LCD_DDR_RS	SBIT(DDRB, 0)

#define	LCD_E0		SBIT(PORTB, 1)
#define	LCD_DDR_E0	SBIT(DDRB, 1)

#ifdef LCD_1X8
#define LCD_COLUMN      8
#define LCD_LINE        1
#define LCD_LINE1       0x80
#endif

#ifdef LCD_1X16
#define LCD_COLUMN      16
#define LCD_LINE        1
#define LCD_LINE1       0x80
#endif

#ifdef LCD_1X20
#define LCD_COLUMN      20
#define LCD_LINE        1
#define LCD_LINE1       0x80
#endif

#ifdef LCD_1X40
#define LCD_COLUMN      40
#define LCD_LINE        1
#define LCD_LINE1       0x80
#endif

#ifdef LCD_2X8
#define LCD_COLUMN      8
#define LCD_LINE        2
#define LCD_LINE1       0x80
#define LCD_LINE2       (0x80 + 0x40)
#endif

#ifdef LCD_2X12
#define LCD_COLUMN      12
#define LCD_LINE        2
#define LCD_LINE1       0x80
#define LCD_LINE2       (0x80 + 0x40)
#endif

#ifdef LCD_2X16
#define LCD_COLUMN      16
#define LCD_LINE        2
#define LCD_LINE1       0x80
#define LCD_LINE2       (0x80 + 0x40)
#endif

#ifdef LCD_2X20
#define LCD_COLUMN      20
#define LCD_LINE        2
#define LCD_LINE1       0x80
#define LCD_LINE2       (0x80 + 0x40)
#endif

#ifdef LCD_2X24
#define LCD_COLUMN      24
#define LCD_LINE        2
#define LCD_LINE1       0x80
#define LCD_LINE2       (0x80 + 0x40)
#endif

#ifdef LCD_2X40
#define LCD_COLUMN      40
#define LCD_LINE        2
#define LCD_LINE1       0x80
#define LCD_LINE2       (0x80 + 0x40)
#endif

#ifdef LCD_4X16
#define LCD_COLUMN      16
#define LCD_LINE        4
#define LCD_LINE1       0x80
#define LCD_LINE2       (0x80 + 0x40)
#define LCD_LINE3       (0x80 + 0x10)
#define LCD_LINE4       (0x80 + 0x50)
#endif

#ifdef LCD_4X20
#define LCD_COLUMN      20
#define LCD_LINE        4
#define LCD_LINE1       0x80
#define LCD_LINE2       (0x80 + 0x40)
#define LCD_LINE3       (0x80 + 0x14)
#define LCD_LINE4       (0x80 + 0x54)
#endif

#define	LCD_TIME_ENA    1.0             // 1µs
#define LCD_TIME_DAT    50.0            // 50µs
#define LCD_TIME_CLR    2000.0          // 2ms


void lcd_putchar( uint8_t d );
void lcd_init( void );
void lcd_puts( void *s );
void lcd_blank( uint8_t len );          // blank n digits
void lcd_command( uint8_t d );
void lcdFloat(float number);



#if 0
AIL( void lcd_xy( uint8_t x, uint8_t y ))       // always inline function
{
	#ifdef LCD_LINE2
	if( y == 1 )
	x += LCD_LINE2 - LCD_LINE1;
	#ifdef LCD_LINE3
	if( y == 2 )
	x += LCD_LINE3 - LCD_LINE1;
	#ifdef LCD_LINE4
	if( y == 3 )
	x += LCD_LINE4 - LCD_LINE1;
	#endif
	#endif
	#endif
	lcd_command( x + LCD_LINE1 );
}
#else

#ifdef LCD_LINE4
#define lcd_xy(x, y)    lcd_command((x) + ((y==3) ? LCD_LINE4 : \
(y==2) ? LCD_LINE3 : \
(y==1) ? LCD_LINE2 : LCD_LINE1 ))
#else
#ifdef LCD_LINE3
#define lcd_xy(x, y)    lcd_command((x) + ((y==2) ? LCD_LINE3 : \
(y==1) ? LCD_LINE2 : LCD_LINE1 ))
#else
#ifdef LCD_LINE2
#define lcd_xy(x, y)    lcd_command((x) + ((y==1) ? LCD_LINE2 : LCD_LINE1 ))
#else
#define lcd_xy(x, y)    lcd_command((x) + LCD_LINE1 )
#endif
#endif
#endif
#endif
#endif /* LCD_H_ */
void lcd_nibble(uint8_t d);
void lcd_byte(uint8_t d);
void lcd_command(uint8_t d);
void lcd_putchar(uint8_t d);
void lcd_puts(void *s); 
void lcd_blank(uint8_t len);
void lcd_init(void);