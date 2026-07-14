#ifndef __LCD1602_H
#define __LCD1602_H

#include <reg52.h>

#define uint unsigned int
#define uchar unsigned char

/* LCD1602 引脚定义 */
sbit lcden = P1^1;  /* 使能引脚E */
sbit rs    = P1^0;  /* 寄存器选择 */
sbit rw    = P1^2;  /* 读写选择 */
sbit busy  = P0^7;  /* 忙信号 */

/* 函数声明 */
void delay(uchar z);
void lcd_write_com(uchar com);
void lcd_write_date(uchar date);
void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uchar row, uchar col);
void lcd_display_char(uchar row, uchar col, uchar ch);
void lcd_display_string(uchar row, uchar col, uchar *str);

#endif
