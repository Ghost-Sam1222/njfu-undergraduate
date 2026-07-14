#include <reg51.h>

#define uchar unsigned char

sbit LED_GREEN = P1^0;
sbit LED_RED   = P1^1;

uchar correct_data = 0xA5;

void Uart_Init(void)
{
    TMOD &= 0x0F;
    TMOD |= 0x20;

    TH1 = 0xFD;
    TL1 = 0xFD;

    SCON = 0x50;
    TR1 = 1;

    TI = 0;
    RI = 0;
}

void Uart_Send(uchar dat)
{
    SBUF = dat;
    while(TI == 0);
    TI = 0;
}

void main(void)
{
    uchar recv_data;

    LED_GREEN = 1;
    LED_RED = 1;

    Uart_Init();

    while(1)
    {
        if(RI == 1)
        {
            RI = 0;
            recv_data = SBUF;

            if(recv_data == correct_data)
            {
                LED_GREEN = 0;
                LED_RED = 1;
                Uart_Send(0x01);
            }
            else
            {
                LED_GREEN = 1;
                LED_RED = 0;
                Uart_Send(0x00);
            }
        }
    }
}