//可以实现两个最多10位数之间的加减乘除运算
#include <reg52.h>
#include <string.h>
#include <stdio.h>

// ================= LCD 引脚（保持不变，已正常显示）=================
sbit LCD_EN  = P2^4;
sbit LCD_RW  = P2^5;
sbit LCD_RS  = P2^6;
#define LCD_DATA  P0

// ================= 4*4矩阵键盘【按你实际接线修改】=================
// 横向 行线：P1.0 P1.1 P1.2 P1.3
sbit KEY_R1 = P1^0;
sbit KEY_R2 = P1^1;
sbit KEY_R3 = P1^2;
sbit KEY_R4 = P1^3;
// 纵向 列线：P1.4 P1.5 P1.6 P1.7
sbit KEY_C1 = P1^4;
sbit KEY_C2 = P1^5;
sbit KEY_C3 = P1^6;
sbit KEY_C4 = P1^7;

// 功能宏定义
#define keydeng     14   // = 键
#define keyzuoyi    12   // < 删除键

// 按键对应显示字符（布局不变）
code unsigned char KeyDisPlaybuf[16] = 
{
    '1', '2', '3', '+',
    '4', '5', '6', '-',
    '7', '8', '9', '*',
    '<', '0', '=', '/'
};

// ================= 函数声明 =================
void DelayMs(unsigned int t);
void LCD_WriteCmd(unsigned char cmd);
void LCD_WriteData(unsigned char dat);
void LCD_Init(void);
void LCD_ShowStr(unsigned char x, unsigned char y, unsigned char *str);
unsigned char KeyScan(void);
unsigned char GetKey(void);
long Count(char *ch, char len);
void EasyCounter(void);

// ================= 通用延时 =================
void DelayMs(unsigned int t)
{
    unsigned int i, j;
    for(i = t; i > 0; i--)
        for(j = 110; j > 0; j--);
}

// ================= LCD 驱动 =================
void LCD_WriteCmd(unsigned char cmd)
{
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_DATA = cmd;
    LCD_EN = 1;
    DelayMs(1);
    LCD_EN = 0;
}

void LCD_WriteData(unsigned char dat)
{
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_DATA = dat;
    LCD_EN = 1;
    DelayMs(1);
    LCD_EN = 0;
}

void LCD_Init(void)
{
    DelayMs(10);
    LCD_WriteCmd(0x38);
    DelayMs(5);
    LCD_WriteCmd(0x0C);
    DelayMs(5);
    LCD_WriteCmd(0x06);
    DelayMs(5);
    LCD_WriteCmd(0x01);
    DelayMs(5);
}

void LCD_ShowStr(unsigned char x, unsigned char y, unsigned char *str)
{
    unsigned char addr;
    if(y == 0) addr = 0x80 + x;
    else addr = 0xC0 + x;
    LCD_WriteCmd(addr);
    while(*str)
    {
        LCD_WriteData(*str++);
    }
}

// ================= 矩阵键盘扫描（适配 P1 整口按键）=================
unsigned char KeyScan(void)
{
    unsigned char row, col, keyval = 0xFF;

    // 第1行 P1.0
    KEY_R1 = 0; KEY_R2 = 1; KEY_R3 = 1; KEY_R4 = 1;
    if(KEY_C1 == 0){row=0;col=0;keyval=row*4+col;}
    if(KEY_C2 == 0){row=0;col=1;keyval=row*4+col;}
    if(KEY_C3 == 0){row=0;col=2;keyval=row*4+col;}
    if(KEY_C4 == 0){row=0;col=3;keyval=row*4+col;}

    // 第2行 P1.1
    KEY_R1 = 1; KEY_R2 = 0; KEY_R3 = 1; KEY_R4 = 1;
    if(KEY_C1 == 0){row=1;col=0;keyval=row*4+col;}
    if(KEY_C2 == 0){row=1;col=1;keyval=row*4+col;}
    if(KEY_C3 == 0){row=1;col=2;keyval=row*4+col;}
    if(KEY_C4 == 0){row=1;col=3;keyval=row*4+col;}

    // 第3行 P1.2
    KEY_R1 = 1; KEY_R2 = 1; KEY_R3 = 0; KEY_R4 = 1;
    if(KEY_C1 == 0){row=2;col=0;keyval=row*4+col;}
    if(KEY_C2 == 0){row=2;col=1;keyval=row*4+col;}
    if(KEY_C3 == 0){row=2;col=2;keyval=row*4+col;}
    if(KEY_C4 == 0){row=2;col=3;keyval=row*4+col;}

    // 第4行 P1.3
    KEY_R1 = 1; KEY_R2 = 1; KEY_R3 = 1; KEY_R4 = 0;
    if(KEY_C1 == 0){row=3;col=0;keyval=row*4+col;}
    if(KEY_C2 == 0){row=3;col=1;keyval=row*4+col;}
    if(KEY_C3 == 0){row=3;col=2;keyval=row*4+col;}
    if(KEY_C4 == 0){row=3;col=3;keyval=row*4+col;}

    return keyval;
}

// 按键消抖 + 等待松手
unsigned char GetKey(void)
{
    unsigned char key = KeyScan();
    if(key != 0xFF)
    {
        DelayMs(20);
        key = KeyScan();
        if(key != 0xFF)
        {
            while(KeyScan() != 0xFF); // 等待松手
            return key;
        }
    }
    return 0xFF;
}

// ================= 四则运算函数 =================
long Count(char *ch, char len)
{
    unsigned char i = 0;
    char Operation;
    long Num1 = 0, Num2 = 0;

    while(i < len && ch[i] != '+' && ch[i] != '-' && ch[i] != '*' && ch[i] != '/')
    {
        Num1 *= 10;
        Num1 += ch[i] - '0';
        i++;
    }
    Operation = ch[i++];

    while(i < len && ch[i] != '=')
    {
        Num2 *= 10;
        Num2 += ch[i] - '0';
        i++;
    }

    switch(Operation)
    {
        case '+': return Num1 + Num2;
        case '-': return Num1 - Num2;
        case '*': return Num1 * Num2;
        case '/': 
            if(Num2 == 0) return 0;
            return Num1 / Num2;
        default: break;
    }
    return 0;
}

// ================= 计算器主逻辑 =================
void EasyCounter(void)
{
    unsigned char buff[30] = {0};
    unsigned char text[30] = {0};
    unsigned char key;
    unsigned char i = 0;

    LCD_ShowStr(0, 0, "                ");
    LCD_ShowStr(0, 1, "                ");

    do
    {
        key = GetKey();
        if(key != 0xFF)
        {
            // 退格删除
            if(key == keyzuoyi && i > 0)
            {
                i--;
                text[i] = '\0';
            }
            // 正常输入
            else if(key != keyzuoyi && key != keydeng)
            {
                text[i++] = KeyDisPlaybuf[key];
                text[i] = '\0';
            }
            // 刷新显示
            LCD_ShowStr(0, 0, "                ");
            LCD_ShowStr(16 - strlen(text), 0, text);
        }
    }while(key != keydeng);

    // 计算结果
    sprintf(buff, "%ld", Count(text, strlen(text)));
    LCD_ShowStr(0, 1, "Result:");
    LCD_ShowStr(7, 1, buff);
    DelayMs(2000);
}

// ================= 主函数 =================
void main(void)
{
    LCD_Init();
    while(1)
    {
        EasyCounter();
    }
}