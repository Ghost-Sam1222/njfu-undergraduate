#ifndef __E2PROM_H__                                        //E2PROM头文件定义
#define __E2PROM_H__

#include <reg52.h>                                          //包含reg52.h头文件
#include <intrins.h>                                        //包含intrins.h头文件

sfr ISP_DATA = 0xE2;                                        //数据寄存器
sfr ISP_ADDRH = 0xE3;                                       //地址寄存器高八位
sfr ISP_ADDRL = 0xE4;                                       //地址寄存器低八位
sfr ISP_CMD = 0xE5;                                         //命令寄存器
sfr ISP_TRIG = 0xE6;                                        //命令触发寄存器
sfr ISP_CONTR = 0xE7;                                       //命令寄存器

void ISP_Disable();                                         //单片机内部E2PROM不使能
void WriteE2PROM(unsigned char dat, unsigned int addr);     //向单片机E2PROM内写数据
unsigned char ReadE2PROM(unsigned int addr);                //向单片机E2PROM内读数据
void SectorErase(unsigned int addr);                        //擦除整片扇区

#endif    