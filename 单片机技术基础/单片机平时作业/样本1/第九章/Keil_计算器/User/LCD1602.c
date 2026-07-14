#include "lcd1602.h"

void delay(uchar z)
{
    uchar y;
    for(z; z > 0; z--)
        for(y = 0; y < 110; y++);
}

void lcd_write_com(uchar com)
{
    rs = 0;
    P0 = com;
    delay(5);
    lcden = 1;
    delay(5);
    lcden = 0;
}

void lcd_write_date(uchar date)
{
    rs = 1;
    P0 = date;
    delay(5);
    lcden = 1;
    delay(5);
    lcden = 0;
}

void lcd_init(void)
{
    lcden = 1;
    rw = 0;
    
    lcd_write_com(0x38);
    delay(5);
    lcd_write_com(0x38);
    delay(5);
    lcd_write_com(0x0C);
    delay(1);
    lcd_write_com(0x06);
    delay(1);
    lcd_write_com(0x80);
    delay(1);
    lcd_write_com(0x01);
    delay(1);
}

void lcd_clear(void)
{
    lcd_write_com(0x01);
    delay(1);
}

void lcd_set_cursor(uchar row, uchar col)
{
    if(row == 0)
        lcd_write_com(0x80 + col);
    else
        lcd_write_com(0x80 + 0x40 + col);
}

void lcd_display_char(uchar row, uchar col, uchar ch)
{
    lcd_set_cursor(row, col);
    lcd_write_date(ch);
}

void lcd_display_string(uchar row, uchar col, uchar *str)
{
    lcd_set_cursor(row, col);
    while(*str)
    {
        lcd_write_date(*str);
        str++;
    }
}
