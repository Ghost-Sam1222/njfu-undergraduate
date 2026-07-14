#include <reg51.h>
#include <stdio.h>

#define uchar unsigned char
#define uint  unsigned int

sbit KEY_START = P3^3;  
sbit KEY_ADD_10= P2^3;  
sbit KEY_ADD_1 = P2^4;  
sbit BEEP = P1^5;       


sbit LCD_RS = P2^0;
sbit LCD_RW = P2^1;
sbit LCD_EN = P2^2;
#define LCD_DATA P0


uint preset_min = 0;    
uchar preset_sec = 0;  
uint current_min = 0;   
uchar current_sec = 0;  
bit is_running = 0;     
bit time_is_up = 0;     

uint tick_50ms = 0;    
char lcd_buf[17];       


void Delay_ms(uint z) {
    uint x, y;
    for(x=z; x>0; x--) for(y=110; y>0; y--);
}

void LCD_Write_Cmd(uchar cmd) {
    LCD_RS = 0; LCD_RW = 0; LCD_DATA = cmd;
    LCD_EN = 1; Delay_ms(1); LCD_EN = 0;
}

void LCD_Write_Data(uchar dat) {
    LCD_RS = 1; LCD_RW = 0; LCD_DATA = dat;
    LCD_EN = 1; Delay_ms(1); LCD_EN = 0;
}

void LCD_Write_String(uchar x, uchar y, char *s) {
    if (y == 0) LCD_Write_Cmd(0x80 + x);
    else        LCD_Write_Cmd(0xC0 + x);
    while (*s) { LCD_Write_Data(*s); s++; }
}

void LCD_Init() {
    LCD_Write_Cmd(0x38); 
    LCD_Write_Cmd(0x0C); 
    LCD_Write_Cmd(0x06); 
    LCD_Write_Cmd(0x01); 
}


void Timer1_Init() {
    TMOD &= 0x0F;
    TMOD |= 0x10;        
    TH1 = (65536 - 50000) / 256; 
    TL1 = (65536 - 50000) % 256;
    ET1 = 1;             
    EA  = 1;             
    TR1 = 1;             
}

void Timer1_ISR() interrupt 3 {
    TH1 = (65536 - 50000) / 256; 
    TL1 = (65536 - 50000) % 256;

    
    if (is_running && !time_is_up) {
        tick_50ms++;
        if (tick_50ms >= 20) { 
            tick_50ms = 0;
            
            
            if (current_sec > 0) {
                current_sec--;
            } else if (current_min > 0) {
                current_sec = 59;
                current_min--;
            } else {
               
                is_running = 0;
                time_is_up = 1;
            }
        }
    }
}


void main() {
    LCD_Init();
    Timer1_Init();
    BEEP = 1; 
    
    LCD_Write_String(0, 0, "  Countdown     ");
    
    while(1) {
        
        if (time_is_up) {
            LCD_Write_String(0, 0, "** TIME IS UP! **");
            LCD_Write_String(0, 1, "Press START Key ");
            
            BEEP = 0; 
        } else {
            BEEP = 1; 
            
            if (is_running) {
                LCD_Write_String(0, 0, "Mode: RUNNING >>");
            } else {
                LCD_Write_String(0, 0, "Mode: PAUSED || ");
            }
            
            sprintf(lcd_buf, "Time: %02d:%02d     ", current_min, current_sec);
            LCD_Write_String(0, 1, lcd_buf);
        }

      
        if (KEY_START == 0) {
            Delay_ms(20);
            if (KEY_START == 0) {
                if (time_is_up) {
                    
                    time_is_up = 0;
                    current_min = preset_min;
                    current_sec = preset_sec;
                    BEEP = 1; 
                } else {
                    
                    is_running = !is_running;
                }
                while(!KEY_START); 
            }
        }

        
        if (!is_running && !time_is_up) {
            if (KEY_ADD_10 == 0) {
                Delay_ms(20);
                if (KEY_ADD_10 == 0) {
                    preset_min += 10;
                    if (preset_min > 99) preset_min = 0; 
                    current_min = preset_min;
                    current_sec = preset_sec;
                    while(!KEY_ADD_10);
                }
            }

            
            if (KEY_ADD_1 == 0) {
                Delay_ms(20);
                if (KEY_ADD_1 == 0) {
                    preset_min++;
                    if (preset_min > 99) preset_min = 0;
                    current_min = preset_min;
                    current_sec = preset_sec;
                    while(!KEY_ADD_1);
                }
            }
        }
    }
}