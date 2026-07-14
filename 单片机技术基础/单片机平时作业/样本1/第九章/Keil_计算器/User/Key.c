#include <REG52.H>
#include "Delay.h"

sbit P1_0 = P1^0;
sbit P1_1 = P1^1;
sbit P1_2 = P1^2;
sbit P1_3 = P1^3;
sbit P1_4 = P1^4;

/**
  * @brief  获取独立按键键码
  * @param  无
  * @retval 按下按键的键码，范围：0~4，无按键按下时返回值为0
  */

unsigned char Key()
{
	unsigned char KeyNumber=0;
	
	if(P1_0==0){Delay(20);while(P1_0==0);Delay(20);KeyNumber=1;}
	if(P1_1==0){Delay(20);while(P1_1==0);Delay(20);KeyNumber=2;}
	if(P1_2==0){Delay(20);while(P1_2==0);Delay(20);KeyNumber=3;}
	if(P1_3==0){Delay(20);while(P1_3==0);Delay(20);KeyNumber=4;}
	if(P1_4==0){Delay(20);while(P1_4==0);Delay(20);KeyNumber=5;}
	return KeyNumber;
}
