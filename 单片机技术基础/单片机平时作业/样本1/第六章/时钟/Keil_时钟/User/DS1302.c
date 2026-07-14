#include "DS1302.h"                                       //包含DS1302头文件

/*********************************************************/
// 初始化DS1302
/*********************************************************/
void DS1302_Init(void)
{
    RST = 0;                                              //RST脚置低
    SCK = 0;                                              //SCK脚置低
    SDA = 0;                                              //SDA脚置低	
}
/*********************************************************/
// 向DS1302写入一字节数据
/*********************************************************/
void DS1302_Write_Byte(uchar addr, uchar dat)
{
    uchar i;                                              //定义无符号变量i，for循环用  
    
    RST = 1;                                              //RST引脚拉为高电平 
    
    /* 写入目标地址：addr*/
    for(i=0; i<8; i++)                                    //循环8次 
    {
        if(addr & 0x01)                                   //判断addr最后一位是否为1 
            SDA = 1;                                      //若为1，SDA引脚输出为1 
        else                                              //否则
            SDA = 0;                                      //SDA引脚输出为0 
        SCK = 1;                                          //SCK拉为高电平 
        _nop_();                                          //延时1us 
        SCK = 0;                                          //SCK拉为低电平 
        _nop_();                                          //延时1us 
        
        addr = addr >> 1;                                 //addr向右移动一位 
    }
    
    /* 写入数据：dat*/
    for(i=0; i<8; i++)                                    //循环8次 
    {
        if(dat & 0x01)                                    //判断dat最后一位是否为1  
            SDA = 1;                                      //若为1，SDA引脚输出为1 
        else                                              //否则 
            SDA = 0;                                      //SDA引脚输出为0 
        SCK = 1;                                          //SCK拉为高电平 
        _nop_();                                          //延时1us 
        SCK = 0;                                          //SCK拉为低电平 
        _nop_();                                          //延时1us 
        
        dat = dat >> 1;                                   //addr向右移动一位 
    }
    
    RST = 0;                                              //RST引脚拉为低电平 
}
/*********************************************************/
// 从DS1302读出一字节数据
/*********************************************************/
uchar DS1302_Read_Byte(uchar addr)
{
    uchar i;                                              //定义无符号变量i，for循环用 
    uchar temp;                                           //temp用来保存读到的数据 
    
    RST = 1;                                              //RST引脚拉为高电平 
    
    /* 写入目标地址：addr*/
    for(i=0; i<8; i++)                                    //循环8次  
    {
        if(addr & 0x01)                                   //判断addr最后一位是否为1  
            SDA = 1;                                      //若为1，SDA引脚输出为1  
        else                                              //否则  
            SDA = 0;                                      //SDA引脚输出为0  
        SCK = 1;                                          //SCK拉为高电平 
        _nop_();                                          //延时1us 
        SCK = 0;                                          //SCK拉为低电平 
        _nop_();                                          //延时1us 
        
        addr = addr >> 1;                                 //addr向右移动一位 
    }
    
    /* 读出该地址的数据 */
    for(i=0; i<8; i++)                                    //循环8次 
    {
        temp = temp >> 1;                                 //temp向右移动一位 
        
        if(SDA == 1)                                      //判断SDA是否为1 
            temp |= 0x80;                                 //若为1，temp按位或上0x80 
        else                                              //否则
            temp &= 0x7f;                                 //temp按位与上0x7f 
        SCK = 1;                                          //SCK拉为高电平 
        _nop_();                                          //延时1us 
        SCK = 0;                                          //SCK拉为低电平 
        _nop_();                                          //延时1us 
    }
    
    RST = 0;                                              //RST引脚拉为低电平 
    
    return temp;                                          //返回读到的数据 
}
/*********************************************************/
// 向DS1302写入时间数据
/*********************************************************/
void DS1302_Write_Time()
{
    uchar i;                                              //定义无符号字符型变量i，for循环使用
    uchar temp1;                                          //temp1用来保存转换成的二进制的十位
    uchar temp2;                                          //temp2用来保存转换成的二进制的个位
    
    for(i=0; i<7; i++)                                    //循环7次
    {
        temp1 = (TimeBuff[i]/10) << 4;                    //分离出十位并左移4位
        temp2 = TimeBuff[i] % 10;                         //分离出个位
        TimeBuff[i] = temp1 + temp2;                      //十位与个位整合
    }
    
    DS1302_Write_Byte(0x8E, 0x00);                        //关闭写保护
    DS1302_Write_Byte(0x80, 0x80);                        //暂停时钟
    DS1302_Write_Byte(0x8C, TimeBuff[0]);                 //年
    DS1302_Write_Byte(0x88, TimeBuff[1]);                 //月
    DS1302_Write_Byte(0x86, TimeBuff[2]);                 //日
    DS1302_Write_Byte(0x8A, TimeBuff[3]);                 //星期
    DS1302_Write_Byte(0x84, TimeBuff[4]);                 //时
    DS1302_Write_Byte(0x82, TimeBuff[5]);                 //分
    DS1302_Write_Byte(0x80, TimeBuff[6]);                 //秒
    DS1302_Write_Byte(0x80, TimeBuff[6]&0x7F);            //运行时钟
    DS1302_Write_Byte(0x8E, 0x80);                        //打开写保护
}
/*********************************************************/
// 从DS1302读出时间数据
/*********************************************************/
void DS1302_Read_Time()
{
    uchar i;                                              //定义无符号字符型变量i，for循环使用
    
    TimeBuff[0] = DS1302_Read_Byte(0x8D);				  //年 
	TimeBuff[1] = DS1302_Read_Byte(0x89);			      //月 
	TimeBuff[2] = DS1302_Read_Byte(0x87);				  //日 
	TimeBuff[3] = DS1302_Read_Byte(0x8B);				  //星期
	TimeBuff[4] = DS1302_Read_Byte(0x85);				  //时 
	TimeBuff[5] = DS1302_Read_Byte(0x83);				  //分 
	TimeBuff[6] = (DS1302_Read_Byte(0x81)) & 0x7F;		  //秒 
    
    for(i=0;i<7;i++)		                              //BCD转十进制
	{           
		TimeBuff[i] = (TimeBuff[i]/16)*10 + TimeBuff[i]%16;
	}
}