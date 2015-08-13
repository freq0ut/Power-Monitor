#include "glcd.h"
#include <math.h>

const extern uint8_t PROGMEM font[];

// the most basic function, set a single pixel
void setpixel(uint8_t *buff, uint8_t x, uint8_t y, uint8_t color) 
{
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return;
  // x is which column
  if (color) 
    buff[x+ (y/8)*128] |= _BV(7-(y%8));  
  else
    buff[x+ (y/8)*128] &= ~_BV(7-(y%8)); 
}

void drawbitmap(uint8_t *buff, uint8_t x, uint8_t y, const uint8_t bitmap, uint8_t w, uint8_t h, uint8_t color)	
{
  for (uint8_t j=0; j<h; j++) 
  {
    for (uint8_t i=0; i<w; i++ ) 
	{
      if (pgm_read_byte(bitmap + i + (j/8)*w) & _BV(j%8)) 
	  {
		setpixel(buff, x+i, y+j, color);
      }
    }
  }
}

void drawstring(uint8_t *buff, uint8_t x, uint8_t line, char *c) 
{
  while (c[0] != 0) 
  {
    drawchar(buff, x, line, c[0]);
    c++;
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) 
	{
      x = 0;    // ran out of this line
      line++;
    }
  }
}

void drawchar(uint8_t *buff, uint8_t x, uint8_t line, uint8_t c) 
{
  for (uint8_t i =0; i<5; i++ ) 
  {
    buff[x + (line*128) ] = pgm_read_byte(font+(c*5)+i);
    x++;
  }
}

// the most basic function, clear a single pixel
void clearpixel(uint8_t *buff, uint8_t x, uint8_t y) 
{
  // x is which column
  buff[x+ (y/8)*128] &= ~_BV(7-(y%8));
}

// bresenham's algorithm - thx wikpedia
void drawline(uint8_t *buff, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) 
{
  uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) 
  {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) 
  {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint8_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int8_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1) 
  {
    ystep = 1;
  } 
  else 
  {
    ystep = -1;
  }
  for (; x0<x1; x0++) 
  {
    if (steep) 
	{
      setpixel(buff, y0, x0, color);
    } 
	else 
	{
      setpixel(buff, x0, y0, color);
    }
    err -= dy;
    if (err < 0) 
	{
      y0 += ystep;
      err += dx;
    }
  }
}

// draw a circle
void drawcircle(uint8_t *buff, uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) 
{
  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  setpixel(buff, x0, y0+r, color);
  setpixel(buff, x0, y0-r, color);
  setpixel(buff, x0+r, y0, color);
  setpixel(buff, x0-r, y0, color);

  while (x<y) 
  {
    if (f >= 0) 
	{
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    setpixel(buff, x0 + x, y0 + y, color);
    setpixel(buff, x0 - x, y0 + y, color);
    setpixel(buff, x0 + x, y0 - y, color);
    setpixel(buff, x0 - x, y0 - y, color);
    
    setpixel(buff, x0 + y, y0 + x, color);
    setpixel(buff, x0 - y, y0 + x, color);
    setpixel(buff, x0 + y, y0 - x, color);
    setpixel(buff, x0 - y, y0 - x, color); 
  }
}

// draw a circle
void fillcircle(uint8_t *buff, uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  for (uint8_t i=y0-r; i<=y0+r; i++) 
  {
    setpixel(buff, x0, i, color);
  }

  while (x<y) 
  {
    if (f >= 0) 
	{
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    for (uint8_t i=y0-y; i<=y0+y; i++) 
	{
      setpixel(buff, x0+x, i, color);
      setpixel(buff, x0-x, i, color);
    } 
    for (uint8_t i=y0-x; i<=y0+x; i++) 
	{
      setpixel(buff, x0+y, i, color);
      setpixel(buff, x0-y, i, color);
    }    
  }
}

// clear everything
void clear_buffer(uint8_t *buff) 
{
  memset(buff, 0, 1024);
}

void lcd_float(uint8_t *buff, uint8_t x, uint8_t line, float number) 
{
	int d;
	
	number = round(number * 100) / 100; /* round off to 2 decimal places */
	if(number >= 900)
	{
		drawchar(buff, x, line, '0' + number / 100);
		drawchar(buff, x+6, line, '0' + (number - 900)/10);
		drawchar(buff, x+12, line, '0' + number - 10 * floor(number / 10));
		d = 18;
	}
	else if(number >= 800)
	{
		drawchar(buff, x, line, '0' + number / 100);
		drawchar(buff, x+6, line, '0' + (number - 800)/10);
		drawchar(buff, x+12, line, '0' + number - 10 * floor(number / 10));
		d = 18;
	}
	else if(number >= 700)
	{
		drawchar(buff, x, line, '0' + number / 100);
		drawchar(buff, x+6, line, '0' + (number - 700)/10);
		drawchar(buff, x+12, line, '0' + number - 10 * floor(number / 10));
		d = 18;
	}
	else if(number >= 600)
	{
		drawchar(buff, x, line, '0' + number / 100);
		drawchar(buff, x+6, line, '0' + (number - 600)/10);
		drawchar(buff, x+12, line, '0' + number - 10 * floor(number / 10));
		d = 18;
	}
	else if(number >= 500)
	{
		drawchar(buff, x, line, '0' + number / 100);
		drawchar(buff, x+6, line, '0' + (number - 500)/10);
		drawchar(buff, x+12, line, '0' + number - 10 * floor(number / 10));
		d = 18;
	}
	else if(number >= 400)
	{
		drawchar(buff, x, line, '0' + number / 100);
		drawchar(buff, x+6, line, '0' + (number - 400)/10);
		drawchar(buff, x+12, line, '0' + number - 10 * floor(number / 10));
		d = 18;
	}
	else if(number >= 300)
	{
		drawchar(buff, x, line, '0' + number / 100);
		drawchar(buff, x+6, line, '0' + (number - 300)/10);
		drawchar(buff, x+12, line, '0' + number - 10 * floor(number / 10));
		d = 18;
	}
	else if(number >= 200)
	{
		drawchar(buff, x, line, '0' + number / 100);
		drawchar(buff, x+6, line, '0' + (number - 200)/10);
		drawchar(buff, x+12, line, '0' + number - 10 * floor(number / 10));
		d = 18;
	}
	else if(number >= 100)
	{
		drawchar(buff, x, line, '0' + number / 100);
		drawchar(buff, x+6, line, '0' + (number - 100)/10);
		drawchar(buff, x+12, line, '0' + number - 10 * floor(number / 10));
		d = 18;
	}
	else if(number >= 10)
	{
		drawchar(buff, x, line, '0' + number / 10);
		drawchar(buff, x+6, line, '0' + number - 10*floor(number/10));
		d = 12;
	}
	else if(number < 10 && number >= 0)
	{
		drawchar(buff, x, line, '0' + number);
		d = 6;
	}
	else if(number < 0)
	{
		drawchar(buff, x, line, '-');
		drawchar(buff, x+6, line, '0' + number);
		d = 12;
	}
	else
	{
		drawchar(buff, x, line, '0');
		d = 6;
	}
	
	drawchar(buff, x+d, line, '.');
	drawchar(buff, x+d+6, line, '0' + (number * 10) - floor(number) * 10);
	drawchar(buff, x+d+12, line, '0' + (number * 100) - floor(number * 10) * 10);
	drawchar(buff, x+d+18, line, ' ');
	drawchar(buff, x+d+24, line, ' ');
}
