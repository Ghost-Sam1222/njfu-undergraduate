#include "KeyScanf.h"
#include "Buzzer.h"
#include "Music.h"

/*********************************************************/
// 获取指定年份和月份的天数
/*********************************************************/
static uchar GetMonthDays(uchar year, uchar month)
{
    if(month == 1 || month == 3 || month == 5 || month == 7 || 
       month == 8 || month == 10 || month == 12)
        return 31;
    else if(month == 4 || month == 6 || month == 9 || month == 11)
        return 30;
    else if(month == 2)
    {
        uint fullYear = 2000 + year;
        if((fullYear % 4 == 0 && fullYear % 100 != 0) || (fullYear % 400 == 0))
            return 29;
        else
            return 28;
    }
    else
        return 31;
}

/*********************************************************/
// 时间设置项定义
/*********************************************************/
#define TIME_ITEM_COUNT 7
uchar code TimeCursorTable[TIME_ITEM_COUNT][4] = {
    {0, 2, 0, 2},   // 年份 (列2开始，2个字符宽)
    {0, 5, 1, 2},   // 月份 (列5开始)
    {0, 8, 2, 2},   // 日期 (列8开始)
    {0, 12, 3, 3},  // 星期 (列12开始，3个字符)
    {1, 0, 4, 2},   // 小时 (列0开始)
    {1, 3, 5, 2},   // 分钟 (列3开始)
    {1, 6, 6, 2}    // 秒钟 (列6开始)
};

/*********************************************************/
// 通用清屏函数：从指定位置开始清空N个字符
/*********************************************************/
void ClearLcdArea(uchar line, uchar col, uchar len)
{
    uchar i;
    SetLcdCursor(line, col);
    for(i = 0; i < len; i++)
        WriteLcdData(' ');
    SetLcdCursor(line, col);
}

/*********************************************************/
// 刷新单个时间项显示（先清空，再显示）
/*********************************************************/
void RefreshTimeItem(uchar item)
{
    uchar line, col, idx, width;
    
    line = TimeCursorTable[item][0];
    col  = TimeCursorTable[item][1];
    idx  = TimeCursorTable[item][2];
    width = TimeCursorTable[item][3];
    
    ClearLcdArea(line, col, width);  // 先清空
    
    if(idx == 3)
    {
        ShowLcdWeek(TimeBuff[3]);
    }
    else
    {
        ShowLcdNum(TimeBuff[idx]);
    }
}

/*********************************************************/
// 按键扫描(设置时间)
/*********************************************************/
void KeyScanf1()
{
    uchar currentItem, idx;
    
    currentItem = 0;
    
    if(KeySet == 0)
    {
        WriteLcdCmd(0x0f);
        SetLcdCursor(TimeCursorTable[0][0], TimeCursorTable[0][1] + 1);  // 光标在第二个数字上
        DelayMs(10);
        while(!KeySet);
        DelayMs(10);
        
        while(1)
        {
            /* 左切按键 */
            if(KeyLeft == 0)
            {
                currentItem = (currentItem == 0) ? TIME_ITEM_COUNT - 1 : currentItem - 1;
                SetLcdCursor(TimeCursorTable[currentItem][0], TimeCursorTable[currentItem][1] + 1);
                DelayMs(200);
                while(!KeyLeft);
            }
            
            /* 右切按键 */
            if(KeyRight == 0)
            {
                currentItem = (currentItem == TIME_ITEM_COUNT - 1) ? 0 : currentItem + 1;
                SetLcdCursor(TimeCursorTable[currentItem][0], TimeCursorTable[currentItem][1] + 1);
                DelayMs(200);
                while(!KeyRight);
            }
            
            /* 选择键：保存退出 */
            if(KeySet == 0)
            {
                break;
            }
            
            idx = TimeCursorTable[currentItem][2];
            
            /* 减按键 */
            if(KeyDown == 0)
            {
                switch(currentItem)
                {
                    case 0:
                        TimeBuff[0] = (TimeBuff[0] == 0) ? 99 : TimeBuff[0] - 1;
                        {
                            uchar maxDay = GetMonthDays(TimeBuff[0], TimeBuff[1]);
                            if(TimeBuff[2] > maxDay) TimeBuff[2] = maxDay;
                        }
                        break;
                    case 1:
                        TimeBuff[1] = (TimeBuff[1] == 1) ? 12 : TimeBuff[1] - 1;
                        {
                            uchar maxDay = GetMonthDays(TimeBuff[0], TimeBuff[1]);
                            if(TimeBuff[2] > maxDay) TimeBuff[2] = maxDay;
                        }
                        break;
                    case 2:
                        {
                            uchar maxDay = GetMonthDays(TimeBuff[0], TimeBuff[1]);
                            TimeBuff[2] = (TimeBuff[2] == 1) ? maxDay : TimeBuff[2] - 1;
                        }
                        break;
                    case 3:
                        TimeBuff[3] = (TimeBuff[3] == 1) ? 7 : TimeBuff[3] - 1;
                        break;
                    case 4:
                        TimeBuff[4] = (TimeBuff[4] == 0) ? 23 : TimeBuff[4] - 1;
                        break;
                    case 5:
                        TimeBuff[5] = (TimeBuff[5] == 0) ? 59 : TimeBuff[5] - 1;
                        break;
                    case 6:
                        TimeBuff[6] = (TimeBuff[6] == 0) ? 59 : TimeBuff[6] - 1;
                        break;
                }
                RefreshTimeItem(currentItem);
                SetLcdCursor(TimeCursorTable[currentItem][0], TimeCursorTable[currentItem][1] + 1);
                DelayMs(200);
                while(!KeyDown);
            }
            
            /* 加按键 */
            if(KeyUp == 0)
            {
                switch(currentItem)
                {
                    case 0:
                        TimeBuff[0] = (TimeBuff[0] == 99) ? 0 : TimeBuff[0] + 1;
                        {
                            uchar maxDay = GetMonthDays(TimeBuff[0], TimeBuff[1]);
                            if(TimeBuff[2] > maxDay) TimeBuff[2] = maxDay;
                        }
                        break;
                    case 1:
                        TimeBuff[1] = (TimeBuff[1] == 12) ? 1 : TimeBuff[1] + 1;
                        {
                            uchar maxDay = GetMonthDays(TimeBuff[0], TimeBuff[1]);
                            if(TimeBuff[2] > maxDay) TimeBuff[2] = maxDay;
                        }
                        break;
                    case 2:
                        {
                            uchar maxDay = GetMonthDays(TimeBuff[0], TimeBuff[1]);
                            TimeBuff[2] = (TimeBuff[2] == maxDay) ? 1 : TimeBuff[2] + 1;
                        }
                        break;
                    case 3:
                        TimeBuff[3] = (TimeBuff[3] == 7) ? 1 : TimeBuff[3] + 1;
                        break;
                    case 4:
                        TimeBuff[4] = (TimeBuff[4] == 23) ? 0 : TimeBuff[4] + 1;
                        break;
                    case 5:
                        TimeBuff[5] = (TimeBuff[5] == 59) ? 0 : TimeBuff[5] + 1;
                        break;
                    case 6:
                        TimeBuff[6] = (TimeBuff[6] == 59) ? 0 : TimeBuff[6] + 1;
                        break;
                }
                RefreshTimeItem(currentItem);
                SetLcdCursor(TimeCursorTable[currentItem][0], TimeCursorTable[currentItem][1] + 1);
                DelayMs(200);
                while(!KeyUp);
            }
        }
        
        WriteLcdCmd(0x0C);
        DS1302_Write_Time();
        DelayMs(10);
        while(!KeySet);
        DelayMs(10);
    }
}

/*********************************************************/
// 闹钟设置项定义
/*********************************************************/
#define CLOCK_ITEM_COUNT 4
uchar code ClockCursorTable[CLOCK_ITEM_COUNT][3] = {
    {0, 6, 2},   // 小时 (行0,列6,宽度2)
    {0, 9, 2},   // 分钟 (行0,列9,宽度2)
    {0, 13, 3},  // 开关 (行0,列13,宽度3)
    {1, 5, 8}    // 铃声 (行1,列5,宽度8)
};

/*********************************************************/
// 闹钟界面
/*********************************************************/
void RefreshClockScreen()
{
    SetLcdCursor(0, 0);
    ShowLcdStr("CLK  ");
    ShowLcdNum(Clock_Hour);
    ShowLcdStr(":");
    ShowLcdNum(Clock_Minute);
    ShowLcdStr("  ");
    if(Clock_Swt)
        ShowLcdStr(" ON");
    else
        ShowLcdStr("OFF");
    
    SetLcdCursor(1, 0);
    ShowLcdStr("Bell  ");
    ShowLcdStr("        ");
    SetLcdCursor(1, 5);
    ShowLcdStr(SongNames[CurrentSong]);
}

/*********************************************************/
// 按键扫描(设置闹钟)
/*********************************************************/
void KeyScanf2()
{
    uchar currentItem;
    
    currentItem = 0;
    
    if(KeyClock == 0)
    {
        RefreshClockScreen();
        WriteLcdCmd(0x0f);
        SetLcdCursor(ClockCursorTable[0][0], ClockCursorTable[0][1]);
        DelayMs(10);
        while(!KeyClock);
        DelayMs(10);
        
        while(1)
        {
            /* 左切按键 */
            if(KeyLeft == 0)
            {
                currentItem = (currentItem == 0) ? CLOCK_ITEM_COUNT - 1 : currentItem - 1;
                SetLcdCursor(ClockCursorTable[currentItem][0], ClockCursorTable[currentItem][1]);
                DelayMs(200);
                while(!KeyLeft);
            }
            
            /* 右切按键 */
            if(KeyRight == 0)
            {
                currentItem = (currentItem == CLOCK_ITEM_COUNT - 1) ? 0 : currentItem + 1;
                SetLcdCursor(ClockCursorTable[currentItem][0], ClockCursorTable[currentItem][1]);
                DelayMs(200);
                while(!KeyRight);
            }
            
            /* 选择键：保存退出 */
            if(KeyClock == 0)
            {
                break;
            }
            
            /* 减按键 */
            if(KeyDown == 0)
            {
                switch(currentItem)
                {
                    case 0:  // 小时
                        Clock_Hour = (Clock_Hour == 0) ? 23 : Clock_Hour - 1;
                        ClearLcdArea(0, 5, 2);
                        ShowLcdNum(Clock_Hour);
                        break;
                    case 1:  // 分钟
                        Clock_Minute = (Clock_Minute == 0) ? 59 : Clock_Minute - 1;
                        ClearLcdArea(0, 8, 2);
                        ShowLcdNum(Clock_Minute);
                        break;
                    case 2:  // 开关 - 【修复】先清空3个字符！
                        Clock_Swt = !Clock_Swt;
                        ClearLcdArea(0, 13, 3);
                        ShowLcdStr(Clock_Swt ? " ON" : "OFF");
                        break;
                    case 3:  // 铃声
                        CurrentSong = (CurrentSong == 0) ? 3 : CurrentSong - 1;
                        ClearLcdArea(1, 5, 8);
                        ShowLcdStr(SongNames[CurrentSong]);
                        break;
                }
                SetLcdCursor(ClockCursorTable[currentItem][0], ClockCursorTable[currentItem][1]);
                DelayMs(200);
                while(!KeyDown);
            }
            
            /* 加按键 */
            if(KeyUp == 0)
            {
                switch(currentItem)
                {
                    case 0:  // 小时
                        Clock_Hour = (Clock_Hour == 23) ? 0 : Clock_Hour + 1;
                        ClearLcdArea(0, 5, 2);
                        ShowLcdNum(Clock_Hour);
                        break;
                    case 1:  // 分钟
                        Clock_Minute = (Clock_Minute == 59) ? 0 : Clock_Minute + 1;
                        ClearLcdArea(0, 8, 2);
                        ShowLcdNum(Clock_Minute);
                        break;
                    case 2:  // 开关 
                        Clock_Swt = !Clock_Swt;
                        ClearLcdArea(0, 12, 3);
                        ShowLcdStr(Clock_Swt ? " ON" : "OFF");
                        break;
                    case 3:  // 铃声
                        CurrentSong = (CurrentSong == 3) ? 0 : CurrentSong + 1;
                        ClearLcdArea(1, 5, 8);
                        ShowLcdStr(SongNames[CurrentSong]);
                        break;
                }
                SetLcdCursor(ClockCursorTable[currentItem][0], ClockCursorTable[currentItem][1]);
                DelayMs(200);
                while(!KeyUp);
            }
        }
        
        WriteLcdCmd(0x0C);
        ShowLcdInit();
        DelayMs(10);
        while(!KeyClock);
        DelayMs(10);
        
        SectorErase(0x2000);
        WriteE2PROM(Clock_Hour, 0x2000);
        WriteE2PROM(Clock_Minute, 0x2001);
        WriteE2PROM(Clock_Swt, 0x2002);
        WriteE2PROM(CurrentSong, 0x2003);
    }
}