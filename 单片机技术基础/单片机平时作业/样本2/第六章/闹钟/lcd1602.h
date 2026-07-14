#ifndef __LCD1602_H__
#define __LCD1602_H__

#include <REGX51.H>

#define uchar unsigned char
#define uint  unsigned int

sbit rs = P2^0;
sbit rw = P2^1;
sbit e  = P2^2;

void delay_uint(uint i);
void LCD_Com(uchar com);
void LCD_Dat(uchar dat);
void LCD_Init(void);
void write_sfm(uchar hang, uchar add);
void write_string(uchar hang, uchar add, uchar *p);

#endif
