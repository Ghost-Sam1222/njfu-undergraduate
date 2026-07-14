
#include <reg51.h>

typedef unsigned char uchar;
typedef unsigned int uint;

sbit BEEP = P1^5;

sbit KEY1 = P3^0;
sbit KEY2 = P3^1;
sbit KEY3 = P3^2;

sbit LCD_RS = P2^0;
sbit LCD_RW = P2^1;
sbit LCD_EN = P2^2;

#define LCD_DATA P0

uint current_reload_val = 0;
bit is_playing = 0;

void Delay_ms(uint ms)
{
    uint i, j;
    for(i = ms; i > 0; i--)
        for(j = 110; j > 0; j--);
}

void LCD_Write_Cmd(uchar cmd)
{
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DATA = cmd;
    LCD_EN = 1;
    Delay_ms(1);
    LCD_EN = 0;
}


void LCD_Write_Data(uchar dat)
{
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_DATA = dat;
    LCD_EN = 1;
    Delay_ms(1);
    LCD_EN = 0;
}

void LCD_Set_Cursor(uchar x, uchar y)
{
    if(y == 0)
        LCD_Write_Cmd(0x80 + x);
    else
        LCD_Write_Cmd(0xC0 + x);
}


void LCD_Write_String(uchar x, uchar y, uchar code *s)
{
    LCD_Set_Cursor(x, y);
    while(*s)
    {
        LCD_Write_Data(*s);
        s++;
    }
}

void LCD_Clear()
{
    LCD_Write_Cmd(0x01);
    Delay_ms(2);
}


void LCD_Init()
{
    LCD_Write_Cmd(0x38);   
    LCD_Write_Cmd(0x0C);   
    LCD_Write_Cmd(0x06);   
    LCD_Write_Cmd(0x01);  
    Delay_ms(2);
}


uchar Key_Scan()
{
    if(KEY1 == 0)
    {
        Delay_ms(10);
        if(KEY1 == 0)
        {
            while(KEY1 == 0);
            return 1;
        }
    }

    if(KEY2 == 0)
    {
        Delay_ms(10);
        if(KEY2 == 0)
        {
            while(KEY2 == 0);
            return 2;
        }
    }

    if(KEY3 == 0)
    {
        Delay_ms(10);
        if(KEY3 == 0)
        {
            while(KEY3 == 0);
            return 3;
        }
    }

    return 0;
}


uchar Delay_Check_Key(uint ms)
{
    uint i;
    uchar key;

    for(i = 0; i < ms / 10; i++)
    {
        Delay_ms(10);
        key = Key_Scan();
        if(key != 0)
            return key;
    }

    return 0;
}

uint code FREQ_TABLE[] = {
    0,
    63628, 63731, 63835, 63928, 64021, 64103, 64185,
    64260, 64331, 64400, 64463, 64528, 64580, 64633,
    64684, 64732, 64777, 64820, 64860, 64898, 64934
};

/* 歌曲1：小星星 */
uchar code STAR[] = {
    8,1, 8,1, 12,1, 12,1, 13,1, 13,1, 12,2,
    11,1, 11,1, 10,1, 10,1, 9,1, 9,1, 8,2,
    12,1, 12,1, 11,1, 11,1, 10,1, 10,1, 9,2,
    12,1, 12,1, 11,1, 11,1, 10,1, 10,1, 9,2,
    8,1, 8,1, 12,1, 12,1, 13,1, 13,1, 12,2,
    11,1, 11,1, 10,1, 10,1, 9,1, 9,1, 8,2,
    0,0
};

/* 歌曲2：两只老虎 */
uchar code TIGER[] = {
    8,1, 9,1, 10,1, 8,1,
    8,1, 9,1, 10,1, 8,1,
    10,1, 11,1, 12,2,
    10,1, 11,1, 12,2,
    12,1, 13,1, 12,1, 11,1, 10,1, 8,1,
    12,1, 13,1, 12,1, 11,1, 10,1, 8,1,
    9,1, 5,1, 8,2,
    9,1, 5,1, 8,2,
    0,0
};

/* 歌曲3：生日快乐 */
uchar code BIRTHDAY[] = {
    8,1, 8,1, 9,2, 8,2, 11,2, 10,4,
    8,1, 8,1, 9,2, 8,2, 12,2, 11,4,
    8,1, 8,1, 15,2, 13,2, 11,2, 10,2, 9,2,
    14,1, 14,1, 13,2, 11,2, 12,2, 11,4,
    0,0
};

uchar Play_Song(uchar code *song, uchar code *song_name, uchar code *lyric)
{
    uchar i = 0;
    uchar note, beat;
    uchar key;

    LCD_Clear();
    LCD_Write_String(0, 0, song_name);
    LCD_Write_String(0, 1, lyric);

    while(song[i] != 0 || song[i + 1] != 0)
    {
        note = song[i];
        beat = song[i + 1];
        i += 2;

        key = Key_Scan();
        if(key != 0)
            return key;

        if(note == 0)
        {
            TR0 = 0;
            is_playing = 0;
            BEEP = 0;

            key = Delay_Check_Key((uint)beat * 250);
            if(key != 0)
                return key;
        }
        else
        {
            current_reload_val = FREQ_TABLE[note];

            TH0 = current_reload_val / 256;
            TL0 = current_reload_val % 256;

            is_playing = 1;
            TR0 = 1;

            key = Delay_Check_Key((uint)beat * 250);

            TR0 = 0;
            is_playing = 0;
            BEEP = 0;

            if(key != 0)
                return key;

            Delay_ms(20);
        }
    }

    TR0 = 0;
    is_playing = 0;
    BEEP = 0;

    LCD_Clear();
    LCD_Write_String(0, 0, "Play Finished");
    LCD_Write_String(0, 1, "Press Key 1-3");

    return 0;
}

void Timer0_Init()
{
    TMOD &= 0xF0;
    TMOD |= 0x01;    

    ET0 = 1;         
    EA = 1;          
    TR0 = 0;         
}


void main()
{
    uchar key = 0;
    uchar next_key = 0;

    Timer0_Init();
    LCD_Init();

    LCD_Write_String(0, 0, " Music Player ");
    LCD_Write_String(0, 1, "Press Key 1-3");

    while(1)
    {
        if(key == 0)
            key = Key_Scan();

        if(key == 1)
        {
            next_key = Play_Song(STAR, "Song1: Star", "Little Star...");
            key = next_key;
        }
        else if(key == 2)
        {
            next_key = Play_Song(TIGER, "Song2: Tiger", "Two Tigers...");
            key = next_key;
        }
        else if(key == 3)
        {
            next_key = Play_Song(BIRTHDAY, "Song3: Birthday", "Happy Birthday");
            key = next_key;
        }
    }
}

void Timer0_ISR() interrupt 1
{
    if(is_playing)
    {
        TH0 = current_reload_val / 256;
        TL0 = current_reload_val % 256;
        BEEP = ~BEEP;
    }
    else
    {
        BEEP = 0;
    }
}