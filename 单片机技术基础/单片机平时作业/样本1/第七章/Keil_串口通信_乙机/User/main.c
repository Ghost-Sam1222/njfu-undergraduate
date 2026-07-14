#include <reg52.h>
sbit GREEN = P1^0;  // 乙机绿灯
sbit RED = P1^1;    // 乙机红灯

void UART_Init()
{
    TMOD |= 0x20;
    TH1 = 0xFD;
    TL1 = 0xFD;
    TR1 = 1;
    SCON = 0x50;
}

void Send_Byte(unsigned char dat)
{
    SBUF = dat;
    while(!TI);
    TI = 0;
}

void main()
{
    unsigned char recv_data;
    UART_Init();
    GREEN = 1;      // 初始全灭
    RED = 1;
    
    while(1)
    {
        if(RI)      // 收到甲机数据
        {
            recv_data = SBUF;
            RI = 0;
            
            if(recv_data == 0xAA)   // 数据校验正确
            {
                GREEN = 0;          // 绿灯亮
                RED = 1;            // 红灯灭
                Send_Byte(0x55);    // 回发正确应答
            }
            else                    // 数据校验错误
            {
                GREEN = 1;          // 绿灯灭
                RED = 0;            // 红灯亮
                // 错误时不回发，甲机收不到应答自动灭灯
            }
        }
    }
}