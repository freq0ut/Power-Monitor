/*
 * lcd.c
 *
 * Created: 6/8/2015 12:01:33 PM
 *  Author: zgoyetche
 */ 


#include "lcd.h"


//#include "main.h"


uint8_t lcd_pos = LCD_LINE1;


void lcd_nibble(uint8_t d)
{
	LCD_D7 = 0; if( d & 1<<7 ) LCD_D7 = 1;
	LCD_D6 = 0; if( d & 1<<6 ) LCD_D6 = 1;
	LCD_D5 = 0; if( d & 1<<5 ) LCD_D5 = 1;
	LCD_D4 = 0; if( d & 1<<4 ) LCD_D4 = 1;
	LCD_E0 = 1;
	_delay_us( LCD_TIME_ENA );
	LCD_E0 = 0;
}


void lcd_byte(uint8_t d)
{
	lcd_nibble( d );
	lcd_nibble( d<<4 );
	_delay_us( LCD_TIME_DAT );
}


void lcd_command(uint8_t d)
{
	LCD_RS = 0;
	lcd_byte( d );
	switch( d ){
		case 0 ... 3:                       // on longer commands
		_delay_us( LCD_TIME_CLR );
		d = LCD_LINE1;
		case 0x80 ... 0xFF:                 // set position
		lcd_pos = d;
	}
}


void lcd_putchar(uint8_t d)
{
	LCD_RS = 1;
	lcd_byte( d );
	switch( ++lcd_pos )
	{
		case LCD_LINE1 + LCD_COLUMN:
		#ifdef LCD_LINE2
			d = LCD_LINE2;
			break;
			case LCD_LINE2 + LCD_COLUMN:
			#ifdef LCD_LINE3
				d = LCD_LINE3;
				break;
				case LCD_LINE3 + LCD_COLUMN:
				#ifdef LCD_LINE4
					d = LCD_LINE4;
					break;
					case LCD_LINE4 + LCD_COLUMN:
				#endif
			#endif
		#endif
		d = LCD_LINE1;
		break;
		default:
		return;
	}
	lcd_command(d);
}


void lcd_puts(void *s)                        // display string from SRAM
{
	for( uint8_t *s1 = s; *s1; s1++ )             // until zero byte
		lcd_putchar( *s1 );
}


void lcd_blank(uint8_t len)                   // blank n digits
{
	while(len--)
	lcd_putchar(' ');
}


void lcd_init(void)
{
	LCD_DDR_D4 = 1;                               // enable output pins
	LCD_DDR_D5 = 1;
	LCD_DDR_D6 = 1;
	LCD_DDR_D7 = 1;
	LCD_DDR_RS = 1;
	LCD_DDR_E0 = 1;
	LCD_E0 = 0;
	LCD_RS = 0;                                   // send commands

	_delay_ms( 15 );
	lcd_nibble( 0x30 );
	_delay_ms( 4.1 );
	lcd_nibble( 0x30 );
	_delay_us( 100 );
	lcd_nibble( 0x30 );
	_delay_us( LCD_TIME_DAT );
	lcd_nibble( 0x20 );                           // 4 bit mode
	_delay_us( LCD_TIME_DAT );
	#if LCD_LINE == 1
	lcd_command( 0x20 );                          // 1 line
	#else
	lcd_command( 0x28 );                          // 2 lines 5*7
	#endif
	lcd_command( 0x08 );                          // display off
	lcd_command( 0x01 );                          // display clear
	lcd_command( 0x06 );                          // cursor increment
	lcd_command( 0x0C );                          // on, no cursor, no blink
}

void lcdFloat(float number)
{
	number = round(number * 100) / 100; /* round off to 2 decimal places */
	if(number >= 900)
	{
		lcd_putchar('0' + number / 100);                            /* hundreds place */
		lcd_putchar('0' + (number - 900)/10);                       /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 800)
	{
		lcd_putchar('0' + number / 100);                            /* hundreds place */
		lcd_putchar('0' + (number - 800)/10);                       /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 700)
	{
		lcd_putchar('0' + number / 100);                            /* hundreds place */
		lcd_putchar('0' + (number - 700)/10);                       /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 600)
	{
		lcd_putchar('0' + number / 100);                            /* hundreds place */
		lcd_putchar('0' + (number - 600)/10);                       /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 500)
	{
		lcd_putchar('0' + number / 100);                            /* hundreds place */
		lcd_putchar('0' + (number - 500)/10);                       /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 400)
	{
		lcd_putchar('0' + number / 100);                            /* hundreds place */
		lcd_putchar('0' + (number - 400)/10);                       /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 300)
	{
		lcd_putchar('0' + number / 100);                            /* hundreds place */
		lcd_putchar('0' + (number - 300)/10);                       /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 200)
	{
		lcd_putchar('0' + number / 100);                            /* hundreds place */
		lcd_putchar('0' + (number - 200)/10);                       /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 100)
	{
		lcd_putchar('0' + number / 100);                            /* hundreds place */
		lcd_putchar('0' + (number - 100)/10);                       /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number >= 10)
	{
		lcd_putchar('0' + number / 10);                             /* tens place */
		lcd_putchar('0' + number - 10 * floor(number / 10));        /* ones */
	}
	else if(number < 10 && number >= 0)
	{
		lcd_putchar('0' + number);									 /* ones */
	}
	else if(number < 0)
	{
		lcd_putchar('-');
		lcd_putchar('0' + number);									 /* ones */
	}
	else
	lcd_putchar('0');
	lcd_putchar('.');
	lcd_putchar('0' + (number * 10) - floor(number) * 10);          /* tenths */
	lcd_putchar('0' + (number * 100) - floor(number * 10) * 10);    /* hundredths place */
	lcd_puts(" "); //clear out left over decimals from larger previous number.
}