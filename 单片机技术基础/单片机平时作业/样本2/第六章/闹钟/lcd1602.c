#include "lcd1602.h"

void delay_uint(uint i)
{
    while(i--);
}

void LCD_Com(uchar com)
{
    e = 0;
    rs = 0;
    rw = 0;
    P0 = com;
    delay_uint(50);
    e = 1;
    delay_uint(50);
    e = 0;
    delay_uint(50);
}

void LCD_Dat(uchar dat)
{
    e = 0;
    rs = 1;
    rw = 0;
    P0 = dat;
    delay_uint(50);
    e = 1;
    delay_uint(50);
    e = 0;
    delay_uint(50);
}

void write_sfm(uchar hang, uchar add)
{
    if(hang == 1)
        LCD_Com(0x80 + add);
    else
        LCD_Com(0x80 + 0x40 + add);
}

void write_string(uchar hang, uchar add, uchar *p)
{
    if(hang == 1)
        LCD_Com(0x80 + add);
    else
        LCD_Com(0x80 + 0x40 + add);

    while(*p != '\0')
    {
        LCD_Dat(*p);
        p++;
    }
}

void LCD_Init(void)
{
    LCD_Com(0x38);    
    LCD_Com(0x0C);    
    LCD_Com(0x06);   
    LCD_Com(0x01);    
    delay_uint(1000);
}
