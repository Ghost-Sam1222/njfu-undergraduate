#include "headfile.h"
#include "ml_gpio.h"
#include "xunji.h"
#include "ml_motor.h"
#include "pid.h"
#include "buzzer_light.h"

//int left_encoder,right_encoder,left_pwm,right_pwm,left_angle_pwm,right_angle_pwm;
//int base_left_pwm,base_right_pwm,angle_pwm;

int now_statue,last_statue,change_flag1,beepx1;


void xunji_init()
{
  gpio_init(GPIOB, DL_GPIO_PIN_13, PB13, IN_UP);   // D1
	gpio_init(GPIOB, DL_GPIO_PIN_15, PB15, IN_UP);   // D2
	gpio_init(GPIOA, DL_GPIO_PIN_31, PA31, IN_UP);   // D3
	gpio_init(GPIOA, DL_GPIO_PIN_28, PA28, IN_UP);   // D4
	gpio_init(GPIOB, DL_GPIO_PIN_1,  PB1, IN_UP);   // D5
	gpio_init(GPIOB, DL_GPIO_PIN_4,  PB4, IN_UP);   // D6
	gpio_init(GPIOB, DL_GPIO_PIN_17, PB17, IN_UP);   // D7
	gpio_init(GPIOB, DL_GPIO_PIN_12, PB12, IN_UP);   // D8
}

unsigned char digtal(unsigned char channel)//1-8	  ��ȡXͨ������ֵ
{
	uint8_t value = 0;
	switch(channel) 
	{
		case 1:  
			if (gpio_get(GPIOB, DL_GPIO_PIN_13)) value = 0;
			else value = 1;  
			break;  
		case 2: 
			if(gpio_get(GPIOB, DL_GPIO_PIN_15)) value = 0;
			else value = 1;  
			break;  
		case 3: 
			if(gpio_get(GPIOA, DL_GPIO_PIN_31)) value = 0;
			else value = 1;  
			break;   
		case 4:  
			if(gpio_get(GPIOA, DL_GPIO_PIN_28)) value = 0;
			else value = 1;  
			break;   
		case 5:
			if(gpio_get(GPIOB, DL_GPIO_PIN_1)) value = 0;
			else value = 1;  
			break;
		case 6:  
			if(gpio_get(GPIOB, DL_GPIO_PIN_4)) value = 0;
			else value = 1;  
			break;  
		case 7: 
			if(gpio_get(GPIOB, DL_GPIO_PIN_17)) value = 0;
			else value = 1;  
			break;  
 		case 8: 
 			if(gpio_get(GPIOB, DL_GPIO_PIN_12)) value = 0;
 			else value = 1;  
 			break;   
	}
	return value; 
}

void track1(void)
{
       if(D1==0|D2==0|D3==0|D4==0|D5==0|D6==0|D7==0|D8==0)
	   { Set_right_pwm(0);	Set_left_pwm(0); beep(); }             //STOP          
	     else 
	    { 
          turn_pid(10,0);
			}                          //PID
	 
}

void track2(void)
{
    static uint16_t node_time = 0;
    static uint16_t turn_delay = 0;
    static uint8_t circle_cnt = 0;
    static uint8_t white_debounce_cnt = 0;
    static uint8_t action_flag = 0;

    #define NODE_COUNT_THRESHOLD  7
    #define TURN_RUN_TIME 70
    #define STRAIGHT_RUN_TIME 40
    #define TARGET_LOOP 4

    // 全白检测
    uint8_t current_white = (D1==1&&D2==1&&D3==1&&D4==1&&D5==1&&D6==1&&D7==1&&D8==1) ? 1 : 0;

    // 消抖
    if(current_white) {
        if(white_debounce_cnt < 5) white_debounce_cnt++;
    } else {
        white_debounce_cnt = 0;
    }
    uint8_t valid_white = (white_debounce_cnt >=5) ? 1 : 0;

    // 节点状态
    if(valid_white) {
        node_time++;
        if(node_time >= NODE_COUNT_THRESHOLD) {
            last_statue = now_statue;
            now_statue = 1;
        }
    } else {
        node_time = 0;
        last_statue = now_statue;
        now_statue = 0;
    }

    // ====================== 【关键修复】只有 2、3、4 才开启动作！======================
    if(now_statue == 1 && last_statue == 0) {
        change_flag1++;
        beep();
        turn_delay = 0;

        // 👇 只在这里加一个判断！！！
        if(change_flag1 == 2 || change_flag1 == 3 || change_flag1 == 4) {
            action_flag = 1;  // 只有2、3、4节点才动作
        }

        if(change_flag1 > 4) {
            change_flag1 = 0;
        }
    }

    // 5圈停车
    if(circle_cnt >= TARGET_LOOP) {
        Set_right_pwm(0);
        Set_left_pwm(0);
        return;
    }

    // 动作执行
    if(action_flag == 1) {
        if(change_flag1 == 2) {
            Set_right_pwm(3500);
            Set_left_pwm(2000);
            turn_delay++;
            if(turn_delay >= TURN_RUN_TIME) {
                turn_delay = 0;
                action_flag = 0;
            }
        }
        else if(change_flag1 == 3) {
            Set_right_pwm(3500);
            Set_left_pwm(1200);
            turn_delay++;
            if(turn_delay >= TURN_RUN_TIME) {
                turn_delay = 0;
                action_flag = 0;
            }
        }
        else if(change_flag1 == 4) {
            Set_right_pwm(4000);
            Set_left_pwm(4000);
            turn_delay++;
            if(turn_delay >= STRAIGHT_RUN_TIME) {
                turn_delay = 0;
                action_flag = 0;
                change_flag1 = 0;
                circle_cnt++;
            }
        }
    }
    // 正常循迹
    else {
        if(D1==1 && D2==1 && D3==1 && D4==0 && D5==0 && D6==1 && D7==1 && D8==1)
        {Set_right_pwm(3000); Set_left_pwm(3000);}
        else if(D1==1 && D2==1 && D3==0 && D4==0 && D5==1 && D6==1 && D7==1 && D8==1)
        {Set_right_pwm(1800); Set_left_pwm(4000);}
        else if(D1==1 && D2==0 && D3==0 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)
        {Set_right_pwm(1000); Set_left_pwm(5000);}
        else if(D1==0 && D2==0 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)
        {Set_right_pwm(700); Set_left_pwm(5200);}
        else if(D1==0 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)
        {Set_right_pwm(400); Set_left_pwm(5600);}
        else if(D1==1 && D2==1 && D3==1 && D4==0 && D5==1 && D6==1 && D7==1 && D8==1)
        {Set_right_pwm(2500); Set_left_pwm(3500);}
        else if(D1==1 && D2==1 && D3==0 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)
        {Set_right_pwm(2000); Set_left_pwm(4200);}
        else if(D1==1 && D2==0 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)
        {Set_right_pwm(1000); Set_left_pwm(4800);}
        else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==0 && D6==1 && D7==1 && D8==1)
        {Set_right_pwm(3500); Set_left_pwm(2500);}
        else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==0 && D7==1 && D8==1)
        {Set_right_pwm(4000); Set_left_pwm(2000);}
        else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==0 && D8==1)
        {Set_right_pwm(4800); Set_left_pwm(1200);}
        else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==0 && D6==0 && D7==1 && D8==1)
        {Set_right_pwm(4300); Set_left_pwm(1800);}
        else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==0 && D7==0 && D8==1)
        {Set_right_pwm(4500); Set_left_pwm(1400);}
        else if(D1==1 && D2==1 && D3==1 && D4==0 && D5==0 && D6==1 && D7==0 && D8==0)
        {Set_right_pwm(4800); Set_left_pwm(1200);}
        else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==0)
        {Set_right_pwm(5200); Set_left_pwm(800);}
    }
}

void track3(void)
{
	  if(D1==1 && D2==1 && D3==1 && D4==0 && D5==0 && D6==1 && D7==1 && D8==1)     //11100111
	{
//		sensor_bias = 0;
		Set_right_pwm(3000); Set_left_pwm(3000); 
	}
	
	 else if(D1==1 && D2==1 && D3==0 && D4==0 && D5==1 && D6==1 && D7==1 && D8==1) //11001111
	{
//		sensor_bias = -15;
//    Set_right_pwm(3500); Set_left_pwm(2200); 
		Set_right_pwm(4200); Set_left_pwm(1400); 
	}
   else if(D1==1 && D2==0 && D3==0 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)  //10011111
	{
//		sensor_bias = -25;
		Set_right_pwm(4700); Set_left_pwm(1000); 
	}
	 else if(D1==0 && D2==0 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)  //00111111
	{
//		sensor_bias = -35; 
			if(yaw_angle_int>-120&&yaw_angle_int<-90)
		{
			Set_right_pwm(700);  Set_left_pwm(5400);  
		}
		 else
		 { Set_right_pwm(5400);  Set_left_pwm(700); } 
		             //5000               800
	}
	
	 else if(D1==0 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)  //01111111
	{
			if(yaw_angle_int>-120&&yaw_angle_int<-90)
		{
			Set_right_pwm(200);  Set_left_pwm(6200);  
		}
		 else
		 { Set_right_pwm(6200);  Set_left_pwm(200); } 
		              //5500              500
	 }
	 // danglu
	 	else if(D1==1 && D2==1 && D3==1 && D4==0 && D5==1 && D6==1 && D7==1 && D8==1) //11101111
	{
//		sensor_bias = -5;
		Set_right_pwm(3500); Set_left_pwm(2500); 
	}
   else if(D1==1 && D2==1 && D3==0 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)   //11011111
	{
//		sensor_bias = -20;
		Set_right_pwm(4500); Set_left_pwm(1000); 
	}
	 else if(D1==1 && D2==0 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)   //10111111
	{
     	 if(yaw_angle_int>-120&&yaw_angle_int<-90)
		{
			Set_right_pwm(200);  Set_left_pwm(6200);  
		}
		 else
		 { Set_right_pwm(6200);  Set_left_pwm(200); } 
	}
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==0 && D6==1 && D7==1 && D8==1)  //11110111
	{    
//		sensor_bias = 10;
		 Set_right_pwm(1500); Set_left_pwm(4500);  
	}
   else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==0 && D7==1 && D8==1)   //11111011
	{
//		sensor_bias = 20;
		Set_right_pwm(800); Set_left_pwm(5200);  
	}
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==0 && D8==1)   //11111101
	{
//		sensor_bias = 30;
			if(yaw_angle_int>-60&&yaw_angle_int<-20)
		{
			Set_right_pwm(6200);  Set_left_pwm(200);  
		}
		 else
		 { Set_right_pwm(200);  Set_left_pwm(6200);} 
	}

	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==0 && D6==0 && D7==1 && D8==1)  //11110011
	{
//		sensor_bias = 15;
		Set_right_pwm(1200); Set_left_pwm(4800);  
	}
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==0 && D7==0 && D8==1)  //11111001
	{
//		sensor_bias = 25;
		Set_right_pwm(1000); Set_left_pwm(5000);  
	}
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==0 && D8==0)  //11111100
	{
		if(yaw_angle_int>-60&&yaw_angle_int<-20)
		{
			Set_right_pwm(5800);  Set_left_pwm(600);  
		}
//		sensor_bias = 35;
		 else
		 { Set_right_pwm(600);  Set_left_pwm(5800);} 
	}
	
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==0)  //11111110
	{
//		sensor_bias = 45;
			if(yaw_angle_int>-60&&yaw_angle_int<-20)
		{
			Set_right_pwm(6200);  Set_left_pwm(200);  
		}
		 else
		 { Set_right_pwm(200);  Set_left_pwm(6200);} 
		  
	}
	
	  else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)  //11111111
   {
		  last_statue=now_statue;
		  now_statue=0;
   }
	 
	   if(D1==0|D2==0|D3==0|D4==0|D5==0|D6==0|D7==0|D8==0)
	 {
		    last_statue=now_statue;
		    now_statue=1;
	 }
	        
	 		    if(now_statue!=last_statue) 
	       {
			     change_flag1++;
           beep();	
		     }
/****************************************************************************************
				                          ��һȦ
*****************************************************************************************/
			 if(change_flag1==0) {turn_pid(10,-42);}	 
			 if(change_flag1==2){check(-137); turn_pid(10,-137);}	 
					
					  if(change_flag1==4)
			    {
			    	Set_left_pwm(0);
   		      Set_right_pwm(0);
				    change_flag1++;
			    }
}

void track4(void)
{
	  if(D1==1 && D2==1 && D3==1 && D4==0 && D5==0 && D6==1 && D7==1 && D8==1)     //11100111
	{
//		sensor_bias = 0;
		Set_right_pwm(3000); Set_left_pwm(3000); 
	}
	
	 else if(D1==1 && D2==1 && D3==0 && D4==0 && D5==1 && D6==1 && D7==1 && D8==1) //11001111
	{
//		sensor_bias = -15;
//    Set_right_pwm(3500); Set_left_pwm(2200); 
		Set_right_pwm(4200); Set_left_pwm(1400); 
	}
   else if(D1==1 && D2==0 && D3==0 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)  //10011111
	{
//		sensor_bias = -25;
		Set_right_pwm(4700); Set_left_pwm(1000); 
	}
	 else if(D1==0 && D2==0 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)  //00111111
	{
//		sensor_bias = -35; 
			if(yaw_angle_int>-120&&yaw_angle_int<-90)
		{
			Set_right_pwm(700);  Set_left_pwm(5400);  
		}
		 else
		 { Set_right_pwm(5400);  Set_left_pwm(700); } 
		             //5000               800
	}
	
	 else if(D1==0 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)  //01111111
	{
			if(yaw_angle_int>-120&&yaw_angle_int<-90)
		{
			Set_right_pwm(200);  Set_left_pwm(6200);  
		}
		 else
		 { Set_right_pwm(6200);  Set_left_pwm(200); } 
		              //5500              500
	 }
	 // danglu
	 	else if(D1==1 && D2==1 && D3==1 && D4==0 && D5==1 && D6==1 && D7==1 && D8==1) //11101111
	{
//		sensor_bias = -5;
		Set_right_pwm(3500); Set_left_pwm(2500); 
	}
   else if(D1==1 && D2==1 && D3==0 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)   //11011111
	{
//		sensor_bias = -20;
		Set_right_pwm(4500); Set_left_pwm(1000); 
	}
	 else if(D1==1 && D2==0 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)   //10111111
	{
     	 if(yaw_angle_int>-120&&yaw_angle_int<-90)
		{
			Set_right_pwm(200);  Set_left_pwm(6200);  
		}
		 else
		 { Set_right_pwm(6200);  Set_left_pwm(200); } 
	}
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==0 && D6==1 && D7==1 && D8==1)  //11110111
	{    
//		sensor_bias = 10;
		 Set_right_pwm(1500); Set_left_pwm(4500);  
	}
   else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==0 && D7==1 && D8==1)   //11111011
	{
//		sensor_bias = 20;
		Set_right_pwm(800); Set_left_pwm(5200);  
	}
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==0 && D8==1)   //11111101
	{
//		sensor_bias = 30;
			if(yaw_angle_int>-60&&yaw_angle_int<-20)
		{
			Set_right_pwm(6200);  Set_left_pwm(200);  
		}
		 else
		 { Set_right_pwm(200);  Set_left_pwm(6200);} 
	}

	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==0 && D6==0 && D7==1 && D8==1)  //11110011
	{
//		sensor_bias = 15;
		Set_right_pwm(1200); Set_left_pwm(4800);  
	}
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==0 && D7==0 && D8==1)  //11111001
	{
//		sensor_bias = 25;
		Set_right_pwm(1000); Set_left_pwm(5000);  
	}
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==0 && D8==0)  //11111100
	{
		if(yaw_angle_int>-60&&yaw_angle_int<-20)
		{
			Set_right_pwm(4500);  Set_left_pwm(2000);  
		}
//		sensor_bias = 35;
		 else
		 { Set_right_pwm(600);  Set_left_pwm(5800);} 
	}
	
	 else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==0)  //11111110
	{
//		sensor_bias = 45;
			if(yaw_angle_int>-60&&yaw_angle_int<-20)
		{
			Set_right_pwm(4500);  Set_left_pwm(2000);  
		}
		 else
		 { Set_right_pwm(200);  Set_left_pwm(6200);} 
		  
	}
	
	  else if(D1==1 && D2==1 && D3==1 && D4==1 && D5==1 && D6==1 && D7==1 && D8==1)  //11111111
   {
		  last_statue=now_statue;
		  now_statue=0;
   }
	 
	   if(D1==0|D2==0|D3==0|D4==0|D5==0|D6==0|D7==0|D8==0)
	 {
		    last_statue=now_statue;
		    now_statue=1;
	 }
	        
	 		    if(now_statue!=last_statue) 
	       {
			     change_flag1++;
           beep();	
		     }
/****************************************************************************************
				                          ��һȦ
*****************************************************************************************/
			 if(change_flag1==0) {turn_pid(10,-42);}	 
			 if(change_flag1==2) {check(-138); turn_pid(10,-138);}	 
			 
/****************************************************************************************
				                          �ڶ�Ȧ
*****************************************************************************************/			 
			     if(change_flag1==4) {turn_pid(10,-46);}
			                                  //45 
			 	   if(change_flag1==6) //****** �׶�
					{
						check(-141);     
				    turn_pid(10,-141); //137 138
				  }
					
/****************************************************************************************
				                          ����Ȧ
*****************************************************************************************/	
		 if(change_flag1==8)  { turn_pid(10,-47);  }  //-45
		 if(change_flag1==10) { check(-142); turn_pid(10,-142);}
					
					
/****************************************************************************************
				                          ����Ȧ
*****************************************************************************************/						
         
           if(change_flag1==12)
			    {
          turn_pid(10,-50);  //-45
			    }
					 if(change_flag1==14)
					{
						check(-142); turn_pid(10,-142); 
					}
					
					  if(change_flag1==16)
			    {
			    	Set_left_pwm(0);
   		      Set_right_pwm(0);
				    change_flag1++;
			    }
}

