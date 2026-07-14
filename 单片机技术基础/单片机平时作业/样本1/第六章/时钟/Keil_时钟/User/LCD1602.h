#ifndef __LCD1602_H__                                         //定义LCD1602头文件
#define __LCD1602_H__

#include <reg52.h>                                            //包含reg52.h头文件

#define uchar unsigned char	                                  //以后unsigned char就可以用uchar代替
#define uint  unsigned int	                                  //以后unsigned int 就可以用uint 代替

sbit RS = P2^7;                                               //定义LCD1602数据命令选择端口
sbit RW = P2^6;                                               //定义LCD1602读写选择端口
sbit EN = P2^5;                                               //定义LCD1602使能端口

extern void DelayMs(unsigned int ms);                         //延时X*ms函数
void WriteLcdCmd(unsigned char cmd);                          //向LCD1602写命令函数
void WriteLcdData(unsigned char dat);                         //向LCD1602写数据函数
void SetLcdCursor(unsigned char line, unsigned char column);  //设置LCD1602坐标函数
void ShowLcdStr(unsigned char *str);                          //LCD1602显示字符串函数
void ShowLcdNum(unsigned char num);                           //液晶输出数字
void LcdInit();                                               //LCD1602初始化

#endif 