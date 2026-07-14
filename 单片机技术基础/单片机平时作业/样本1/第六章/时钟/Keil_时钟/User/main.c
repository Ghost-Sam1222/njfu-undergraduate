#include <reg52.h>                                        //包含头文件
#include "LCD1602.h"                                      //包含LCD1602头文件
#include "E2PROM.h"                                       //包含E2PROM头文件
#include "DS1302.h"                                       //包含DS1302头文件
#include "KeyScanf.h"                                     //包含KeyScanf头文件
#include "Buzzer.h"										  //包含Buzzer头文件
#include "Music.h"
#include "Delay.h"

#define uchar unsigned char	                              //以后unsigned char就可以用uchar代替
#define uint  unsigned int	                              //以后unsigned int 就可以用uint 代替

uchar Clock_Hour;					                      //闹钟的小时
uchar Clock_Minute;				                          //闹钟的分钟
uchar Clock_Swt;					                      //闹钟的开关
uchar Buzzer_Flag = 0;			                          //蜂鸣器工作标志


uchar TimeBuff[7] = {26,6,7,1,20,00,00};				  //时间数组，默认2026年6月7日，星期天，20:00:00
// TimeBuff[0] 代表年份，范围00-99
// TimeBuff[1] 代表月份，范围1-12
// TimeBuff[2] 代表日期，范围1-31
// TimeBuff[3] 代表星期，范围1-7，1是星期天，2是星期一... ...
// TimeBuff[4] 代表小时，范围00-23
// TimeBuff[5] 代表分钟，范围00-59
// TimeBuff[6] 代表秒钟，范围00-59

/********************************************************/
// 延时X*ms函数
/********************************************************/
void DelayMs(unsigned int ms)
{
    unsigned int i, j;                                    //定义两个无符号整形变量i,j
    
    for(i=0; i<ms; i++)                                   
        for(j=0; j<112; j++);
}
/*********************************************************/
// 液晶显示内容的初始化
/*********************************************************/
void ShowLcdInit()
{
    SetLcdCursor(0, 0);                                   //设置坐标第一行，第一列
    ShowLcdStr("20  -  -        ");                       //显示"20  -  -        "
    SetLcdCursor(1, 0);                                   //设置坐标第二行，第一列
    ShowLcdStr("  :  :          ");                       //显示"  :  :          "

}
/*********************************************************/
// 液晶显示星期
/*********************************************************/
void ShowLcdWeek(uchar week)
{
    switch(week)
    {
        case 1: ShowLcdStr("Sun");  break;                //week为1，显示Sun
        case 2: ShowLcdStr("Mon");  break;                //week为2，显示Mon
        case 3: ShowLcdStr("Tue");  break;                //week为3，显示Tue
        case 4: ShowLcdStr("Wed");  break;                //week为4，显示Wed
        case 5: ShowLcdStr("Thu");  break;                //week为5，显示Thu
        case 6: ShowLcdStr("Fri");  break;                //week为6，显示Fri
        case 7: ShowLcdStr("Sat");  break;                //week为7，显示Sat
        default:                    break;                //结束swithc判断
    }
}

/*********************************************************/
// 刷新时间显示
/*********************************************************/
void FlashTime()
{
    SetLcdCursor(0, 2);                                   //设置坐标第1行，第3列
    ShowLcdNum(TimeBuff[0]);                              //显示年份
    
    SetLcdCursor(0, 5);                                   //设置坐标第1行，第6列
    ShowLcdNum(TimeBuff[1]);                              //显示月份
    
    SetLcdCursor(0, 8);                                   //设置坐标第1行，第9列
    ShowLcdNum(TimeBuff[2]);                              //显示日期
    
    SetLcdCursor(1, 0);                                   //设置坐标第2行，第1列
    ShowLcdNum(TimeBuff[4]);                              //显示小时
    
    SetLcdCursor(1, 3);                                   //设置坐标第2行，第4列
    ShowLcdNum(TimeBuff[5]);                              //显示分钟
    
    SetLcdCursor(1, 6);                                   //设置坐标第2行，第7列
    ShowLcdNum(TimeBuff[6]);                              //显示秒钟
    
    SetLcdCursor(0, 12);                                  //设置坐标第1行，第13列
    ShowLcdWeek(TimeBuff[3]);                             //显示星期	
}

/*********************************************************/
// 闹钟判断
/*********************************************************/
void ClockJudge()
{
	if(Clock_Swt == 1)			        // 判断闹钟开关是否开启
	{
		if((Clock_Hour==TimeBuff[4]) && (Clock_Minute==TimeBuff[5]))
		{
			if(TimeBuff[6] == 0)		// 闹钟时间到
			{
				Music_Play(CurrentSong); // 播放选中的铃声
			}
		}
	}
	
	if(TimeBuff[6] == 59)				// 响铃1分钟后自动停止
	{
		Music_Stop();
	}
	
	if(KeyUp == 0)			            // 按加键停止闹钟
	{
		Music_Stop();		
		DelayMs(10);
		while(KeyUp == 0);
		DelayMs(10);
	}
}

/*********************************************************/
 //主函数
/*********************************************************/
void main()
{
  	
	LcdInit();											  //执行液晶初始化	
	DS1302_Init();										  //时钟芯片的初始化
	Music_Init();  										  //初始化音乐播放系统（双定时器）
	ShowLcdInit();										  //液晶显示内容的初始化
   
	if(DS1302_Read_Byte(0x81) >= 128)		              //判断时钟芯片是否正在运行
	{
		DS1302_Write_Time();						      //如果没有，则初始化一个时间
	}	
    
	Clock_Hour = ReadE2PROM(0x2000);		              //读取0x2000这个地址的内容，赋值给闹钟的小时变量
	if(Clock_Hour > 23)									  //如果读取到的闹钟小时数值不正常，则重新赋值
	{
		Clock_Hour = 12;
	}
    Clock_Minute = ReadE2PROM(0x2001);	                  //读取0x2001这个地址的内容，赋值给闹钟的分钟变量
	if(Clock_Minute > 59)								  //如果读取到的闹钟分钟数值不正常，则重新赋值
	{ 
		Clock_Minute = 30;
	}
	Clock_Swt = ReadE2PROM(0x2002);		                  //读取0x2002这个地址的内容，赋值给闹钟的开关变量
	if(Clock_Swt > 1)									  //如果读取到的闹钟开关数值不正常，则重新赋值
	{
		Clock_Swt = 0;
	}

	CurrentSong = ReadE2PROM(0x2003); 					  // 读取保存的铃声设置
	if(CurrentSong > 3)                 				  // 异常值处理
	{
    	CurrentSong = 0;
	}
    
    while(1)
    {
        DS1302_Read_Time();				                  //获取当前时钟芯片的时间，存在数组time_buf中
		FlashTime();							          //刷新时间显示
		ClockJudge();							          //闹钟工作的判断
		
		KeyScanf1();							          //按键扫描(时间的设置)
		KeyScanf2();							          //按键扫描(闹钟的设置)
		
		DelayMs(100);							          //延时0.1秒
    }
}


























