#include <reg51.h>
sbit P2_0 = P2^0;
sbit P2_1 = P2^1;
sbit P2_2 = P2^2;

#define uchar unsigned char
#define uint  unsigned int

uchar code table[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
uchar num[3] = {6, 6, 6};
uchar pos = 0;

void delay_ms(uint x)
{
    uint i, j;
    for(i = x; i > 0; i--)
        for(j = 110; j > 0; j--);
}

void show()
{
    P2 = 0xFF;
    P0 = table[num[0]];
    P2_0 = 0;
    delay_ms(1);

    P2 = 0xFF;
    P0 = table[num[1]];
    P2_1 = 0;
    delay_ms(1);

    P2 = 0xFF;
    P0 = table[num[2]];
    P2_2 = 0;
    delay_ms(1);
}

void int0_func() interrupt 0
{
    pos++;
    if(pos >= 3) pos = 0;
}

void int1_func() interrupt 2
{
    if(num[0] == 1 && num[1] == 9 && num[2] == 3)
    {
        return;
    }
    
    num[pos]++;
    if(num[pos] >= 10) num[pos] = 0;
}

void main(void)
{
    IT0 = 1;
    IT1 = 1;
    EX0 = 1;
    EX1 = 1;
    EA  = 1;
    
    while(1)
    {
        show();
    }
}