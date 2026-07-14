#ifndef __KeyScanf_H__
#define __KeyScanf_H__

#include "E2PROM.h"
#include "LCD1602.h"
#include "DS1302.h"

sbit KeyLeft   = P3^0;				// 左切按键
sbit KeyRight  = P3^1;				// 右切按键
sbit KeySet    = P3^2;				// 设置时间按键
sbit KeyClock  = P3^3;				// 设置闹钟按键
sbit KeyDown   = P3^4;				// 减按键
sbit KeyUp     = P3^5;				// 加按键				                      

extern uchar TimeBuff[7];
extern void DelayMs(unsigned int ms);
extern void ShowLcdWeek(uchar week);
extern void ShowLcdInit();
extern uchar Clock_Hour;
extern uchar Clock_Minute;
extern uchar Clock_Swt;
extern uchar CurrentSong;
extern unsigned char code SongNames[][10];

void KeyScanf1();
void KeyScanf2();

#endif
