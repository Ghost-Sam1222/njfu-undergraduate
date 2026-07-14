#ifndef __DS1302_H__                                      //定义DS1302_H头文件
#define __DS1302_H__

#include <reg52.h>                                        //包含头文件
#include <intrins.h>

#define uchar unsigned char	                              //以后unsigned char就可以用uchar代替
#define uint  unsigned int	                              //以后unsigned int 就可以用uint 代替

sbit RST      = P1^3;				                      //时钟芯片DS1302的RST管脚
sbit SDA      = P1^2;				                      //时钟芯片DS1302的SDA管脚
sbit SCK      = P1^1;				                      //时钟芯片DS1302的SCK管脚

extern uchar TimeBuff[7];		                          //时间数组
void DS1302_Init(void);                                   //初始化DS1302
void DS1302_Write_Byte(uchar addr, uchar dat);            //向DS1302写入一字节数据
uchar DS1302_Read_Byte(uchar addr);                       //从DS1302读出一字节数据
void DS1302_Write_Time();                                 //向DS1302写入时间数据
void DS1302_Read_Time();                                  //从DS1302读出时间数据

#endif
