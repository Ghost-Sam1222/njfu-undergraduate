#include <reg51.h>

#define uchar unsigned char
#define uint  unsigned int

sbit LED = P1^0;
sbit KEY_OK = P2^0;
sbit KEY_ERR = P2^1;

void delay_ms(uint ms)
{
    uint i, j;
    for(i = ms; i > 0; i--)
        for(j = 110; j > 0; j--);
}

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

uchar Uart_Receive(void)
{
    uint timeout = 60000;

    while((RI == 0) && timeout)
    {
        timeout--;
    }

    if(RI == 1)
    {
        RI = 0;
        return SBUF;
    }

    return 0x00;
}

void main(void)
{
    uchar ack;

    LED = 0;
    Uart_Init();

    while(1)
    {
        if(KEY_OK == 0)
        {
            delay_ms(20);
            if(KEY_OK == 0)
            {
                Uart_Send(0xA5);
                ack = Uart_Receive();

                if(ack == 0x01)
                    LED = 1;
                else
                    LED = 0;

                while(KEY_OK == 0);
            }
        }

        if(KEY_ERR == 0)
        {
            delay_ms(20);
            if(KEY_ERR == 0)
            {
                Uart_Send(0x33);
                ack = Uart_Receive();

                if(ack == 0x01)
                    LED = 1;
                else
                    LED = 0;

                while(KEY_ERR == 0);
            }
        }
    }
}