#include "E2PROM.h"                                       //包含E2PROM头文件

/*********************************************************/
// 单片机内部EEPROM不使能
/*********************************************************/
void ISP_Disable()
{
    ISP_CONTR = 0;                                        //命令寄存器赋值为0
    ISP_ADDRH = 0;                                        //地址寄存器高八位赋值为0
    ISP_ADDRL = 0;                                        //地址寄存器低八位赋值为0
} 

/*********************************************************/
// 从单片机内部EEPROM读一个字节
/*********************************************************/
unsigned char ReadE2PROM(unsigned int addr)
{
    ISP_DATA = 0x00;                                      //数据寄存器清零
    ISP_CONTR = 0x83;                                     //允许改变内部E2PROM,存取数据速度为5MHz
    ISP_CMD = 0x01;                                       //读命令
    ISP_ADDRH = (unsigned char)(addr >> 8);               //输入高8位地址
    ISP_ADDRL = (unsigned char)(addr & 0xff);             //输入低8位地址
    ISP_TRIG = 0x46;                                      //先向命令触发寄存器写入0x46
    ISP_TRIG = 0xb9;                                      //再向命令触发寄存器写入0xb9,完成触发
    _nop_();                                              //延时大约1us
    ISP_Disable();                                        //单片机内部EEPROM不使能
    
    return ISP_DATA;                                      //返回读的数据
}
/*********************************************************/
// 从单片机内部EEPROM写一个字节
/*********************************************************/
void WriteE2PROM(unsigned char dat, unsigned int addr)
{
    ISP_CONTR = 0x83;                                     //允许改变内部E2PROM,存取数据速度为5MHz
    ISP_CMD = 0x02;                                       //写命令
    ISP_ADDRH = (unsigned char)(addr >> 8);               //输入高8位地址
    ISP_ADDRL = (unsigned char)(addr & 0xff);             //输入低8位地址
    ISP_DATA = dat;                                       //输入要写的数据
    ISP_TRIG = 0x46;                                      //先向命令触发寄存器写入0x46
    ISP_TRIG = 0xb9;                                      //再向命令触发寄存器写入0xb9,完成触发 
    _nop_();                                              //延时大约1us
    ISP_Disable();                                        //单片机内部EEPROM不使能
}
/*********************************************************/
// 从单片机内部EEPROM扇区擦除
/*********************************************************/
void SectorErase(unsigned int addr)
{
    ISP_CONTR = 0x83;                                     //允许改变内部E2PROM,存取数据速度为5MHz
    ISP_CMD = 0x03;                                       //扇区擦除命令
    ISP_ADDRH = (unsigned char)(addr >> 8);               //输入高8位地址
    ISP_ADDRL = (unsigned char)(addr & 0xff);             //输入低8位地址
    ISP_TRIG = 0x46;                                      //先向命令触发寄存器写入0x46
    ISP_TRIG = 0xb9;                                      //再向命令触发寄存器写入0xb9,完成触发
    _nop_();                                              //延时大约1us
    ISP_Disable();                                        //单片机内部EEPROM不使能
}















