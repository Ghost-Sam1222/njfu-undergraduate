#include <reg52.h>
sbit LED = P1^0;      // 甲机指示灯
sbit KEY_OK = P1^1;   // 正确数据按键：发送约定值0xAA
sbit KEY_ERR = P1^2;  // 错误数据按键：发送错误值0xBB

// 软件延时，11.0592MHz下约1ms
void Delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 110; j++);
}

// 串口初始化：9600波特率，方式1，允许接收
void UART_Init()
{
    TMOD |= 0x20;   // 定时器1，方式2（8位自动重装）
    TH1 = 0xFD;     // 9600波特率初值
    TL1 = 0xFD;
    TR1 = 1;        // 启动定时器1
    SCON = 0x50;    // 串口方式1，REN=1（允许接收）
}

// 发送一个字节
void Send_Byte(unsigned char dat)
{
    SBUF = dat;
    while(!TI);     // 等待发送完成
    TI = 0;         // 清发送标志
}

void main()
{
    unsigned char recv;
    UART_Init();
    LED = 1;        // 初始灯灭
    
    while(1)
    {
        // 正确数据按键检测
        if(KEY_OK == 0)
        {
            Delay_ms(10);      // 软件消抖，滤除按键机械抖动
            if(KEY_OK == 0)
            {
                Send_Byte(0xAA);   // 发送正确约定数据
                Delay_ms(10);      // 等待乙机返回应答
                
                if(RI)             // 收到乙机应答
                {
                    recv = SBUF;
                    RI = 0;
                    if(recv == 0x55)
                        LED = 0;   // 应答正确，亮灯
                    else
                        LED = 1;   // 应答错误，灭灯
                }
                else
                {
                    LED = 1;       // 未收到应答，灭灯
                }
                
                while(KEY_OK == 0); // 等待按键松手，避免长按重复发送
            }
        }
        
        // 错误数据按键检测
        if(KEY_ERR == 0)
        {
            Delay_ms(10);      // 消抖
            if(KEY_ERR == 0)
            {
                Send_Byte(0xBB);   // 发送错误数据（可改为任意非0xAA的值）
                Delay_ms(10);      // 等待应答
                
                if(RI)
                {
                    recv = SBUF;
                    RI = 0;
                    if(recv == 0x55)
                        LED = 0;
                    else
                        LED = 1;
                }
                else
                {
                    LED = 1;       // 错误数据乙机不应答，甲机灭灯
                }
                
                while(KEY_ERR == 0); // 等待松手
            }
        }
    }
}