#include <REGX51.H>
#include "lcd1602.h"

sbit key_mode = P3^0;
sbit key_next = P3^1;
sbit key_add  = P3^2;
sbit key_dec  = P3^3;
sbit beep     = P2^3;

uchar mode = 0;
uchar change = 0;

uchar miao = 0;
uchar fen  = 10;
uchar shi  = 8;

uchar alarm_fen = 12;
uchar alarm_shi = 8;

uchar time_str[]  = "08:10:00";
uchar alarm_str[] = "08:12";

uint ms_count = 0;
uint ring_count = 0;

bit one_second_flag = 0;
bit alarm_ring = 0;

void Delay_ms(uint t)
{
    uint i, j;
    for(i = t; i > 0; i--)
        for(j = 110; j > 0; j--);
}

void Timer0_Init(void)
{
    TMOD &= 0xF0;
    TMOD |= 0x01;

    TH0 = 0xFC;
    TL0 = 0x18;

    ET0 = 1;
    EA  = 1;
    TR0 = 1;
}

void Update_Time_String(void)
{
    time_str[0] = shi / 10 + '0';
    time_str[1] = shi % 10 + '0';

    time_str[3] = fen / 10 + '0';
    time_str[4] = fen % 10 + '0';

    time_str[6] = miao / 10 + '0';
    time_str[7] = miao % 10 + '0';
}

void Update_Alarm_String(void)
{
    alarm_str[0] = alarm_shi / 10 + '0';
    alarm_str[1] = alarm_shi % 10 + '0';

    alarm_str[3] = alarm_fen / 10 + '0';
    alarm_str[4] = alarm_fen % 10 + '0';
}

void Display_All(void)
{
    Update_Time_String();
    Update_Alarm_String();

    write_string(1, 0, "TIME:");
    write_string(1, 5, time_str);

    write_string(2, 0, "CLOCK:");
    write_string(2, 6, alarm_str);

    if(alarm_ring)
        write_string(2, 12, "RING");
    else if(mode == 0)
        write_string(2, 12, " M0 ");
    else if(mode == 1)
        write_string(2, 12, " M1 ");
    else
        write_string(2, 12, " M2 ");
}

void Show_Cursor(void)
{
    if(mode == 0)
    {
        LCD_Com(0x0C);
    }
    else if(mode == 1)
    {
        LCD_Com(0x0E);

        if(change == 0)
            write_sfm(1, 6);
        else if(change == 1)
            write_sfm(1, 9);
        else
            write_sfm(1, 12);
    }
    else if(mode == 2)
    {
        LCD_Com(0x0E);

        if(change == 0)
            write_sfm(2, 7);
        else
            write_sfm(2, 10);
    }
}

void Time_Add_One_Second(void)
{
    if(miao < 59)
    {
        miao++;
    }
    else
    {
        miao = 0;

        if(fen < 59)
        {
            fen++;
        }
        else
        {
            fen = 0;

            if(shi < 23)
                shi++;
            else
                shi = 0;
        }
    }
}

void Check_Alarm(void)
{
    if((shi == alarm_shi) && (fen == alarm_fen) && (miao == 0))
    {
        alarm_ring = 1;
        ring_count = 0;
    }
}

void Stop_Alarm(void)
{
    alarm_ring = 0;
    ring_count = 0;
    beep = 0;
}

void Key_Process(void)
{
    if(key_mode == 0)
    {
        Delay_ms(20);
        if(key_mode == 0)
        {
            Stop_Alarm();

            change = 0;

            if(mode < 2)
                mode++;
            else
                mode = 0;

            Display_All();
            Show_Cursor();

            while(key_mode == 0);
        }
    }

    if(key_next == 0)
    {
        Delay_ms(20);
        if(key_next == 0)
        {
            Stop_Alarm();

            if(mode == 1)
            {
                if(change < 2)
                    change++;
                else
                    change = 0;
            }
            else if(mode == 2)
            {
                if(change < 1)
                    change++;
                else
                    change = 0;
            }

            Show_Cursor();

            while(key_next == 0);
        }
    }

    if(key_add == 0)
    {
        Delay_ms(20);
        if(key_add == 0)
        {
            Stop_Alarm();

            if(mode == 1)
            {
                if(change == 0)
                {
                    if(shi < 23) shi++;
                    else shi = 0;
                }
                else if(change == 1)
                {
                    if(fen < 59) fen++;
                    else fen = 0;
                }
                else
                {
                    if(miao < 59) miao++;
                    else miao = 0;
                }
            }
            else if(mode == 2)
            {
                if(change == 0)
                {
                    if(alarm_shi < 23) alarm_shi++;
                    else alarm_shi = 0;
                }
                else
                {
                    if(alarm_fen < 59) alarm_fen++;
                    else alarm_fen = 0;
                }
            }

            Display_All();
            Show_Cursor();

            while(key_add == 0);
        }
    }

    if(key_dec == 0)
    {
        Delay_ms(20);
        if(key_dec == 0)
        {
            Stop_Alarm();

            if(mode == 1)
            {
                if(change == 0)
                {
                    if(shi > 0) shi--;
                    else shi = 23;
                }
                else if(change == 1)
                {
                    if(fen > 0) fen--;
                    else fen = 59;
                }
                else
                {
                    if(miao > 0) miao--;
                    else miao = 59;
                }
            }
            else if(mode == 2)
            {
                if(change == 0)
                {
                    if(alarm_shi > 0) alarm_shi--;
                    else alarm_shi = 23;
                }
                else
                {
                    if(alarm_fen > 0) alarm_fen--;
                    else alarm_fen = 59;
                }
            }

            Display_All();
            Show_Cursor();

            while(key_dec == 0);
        }
    }
}

void main(void)
{
    beep = 0;

    LCD_Init();
    Timer0_Init();

    Display_All();
    Show_Cursor();

    while(1)
    {
        Key_Process();

        if(one_second_flag)
        {
            one_second_flag = 0;

            if(mode == 0)
            {
                Time_Add_One_Second();
                Check_Alarm();
                Display_All();
            }

            if(alarm_ring)
            {
                ring_count++;

                if(ring_count >= 20)
                {
                    Stop_Alarm();
                    Display_All();
                }
            }
        }
    }
}

void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x18;

    ms_count++;

    if(ms_count >= 1000)
    {
        ms_count = 0;
        one_second_flag = 1;
    }

    if(alarm_ring)
    {
        beep = !beep;
    }
    else
    {
        beep = 0;
    }
}
