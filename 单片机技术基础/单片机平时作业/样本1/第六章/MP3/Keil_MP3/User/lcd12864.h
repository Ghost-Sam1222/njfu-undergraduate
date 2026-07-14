#ifndef __LCD12864_H
#define __LCD12864_H

#include <reg51.h>
#include <intrins.h>

/* 数据类型重定义 */
#define uchar unsigned char
#define uint  unsigned int

/* 字体类型定义 */
#define LCD_8X16  0  /* 8x16 ASCII字符 */
#define LCD_16X16 1  /* 16x16 GB2312汉字 */

/* LCD12864(KS0108) 硬件引脚定义 */
#define LCD_DATA P2  /* 8位并行数据口 */
sbit LCD_RS  = P0^5; /* 寄存器选择：0=命令，1=数据 */
sbit LCD_RW  = P0^6; /* 读写选择：0=写，1=读 */
sbit LCD_EN  = P0^7; /* 读写使能：下降沿有效 */
sbit LCD_CS1 = P0^3; /* 右半屏 */
sbit LCD_CS2 = P0^2; /* 左半屏 */
sbit LCD_RST = P0^4; /* 复位信号：低电平复位 */

/* 函数声明 */
void init_lcd(void);
void lcd_delay(uint time);
void lcd_write_com(uchar cmdcode);
void lcd_write_data(uchar Rsspdata);
void lcd_Clr_Scr(void);
void lcd_werite_x_y(uchar x, uchar y);
void lcd_write_chinese(uchar x, uchar y, uchar hz);
void lcd_write_english(uchar x, uchar y, uchar en);
void lcd_show_string(uchar x, uchar y, const uchar *str, uchar font);

#endif
