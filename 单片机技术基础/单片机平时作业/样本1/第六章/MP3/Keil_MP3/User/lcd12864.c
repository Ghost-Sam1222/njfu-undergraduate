#include "lcd12864.h"
#include "LCD12864_Data.h"

/*---------------------------------------
  延时函数
---------------------------------------*/
void lcd_delay(uint time)
{
    uint i;
    for(i=0; i<time; i++);
}

/*---------------------------------------
  12864初始化函数
---------------------------------------*/
void init_lcd(void)
{           
    LCD_RST = 0;       /* 液晶复位 */
    lcd_delay(250);    /* 复位延时 */
    LCD_RST = 1;       /* 复位结束 */
    
    LCD_CS1 = 1;       /* 同时选通左右半屏 */
    LCD_CS2 = 1;
    lcd_delay(250);
    
    lcd_write_com(0x3F); /* 开启显示 */
}

/*---------------------------------------
  写命令函数
---------------------------------------*/
void lcd_write_com(uchar cmdcode)
{                                          
    LCD_RS = 0;        /* 选择命令寄存器 */
    LCD_RW = 0;        /* 写操作 */
    LCD_DATA = cmdcode;/* 输出命令 */
    _nop_();           /* 时序延时 */
    LCD_EN = 1;        /* 产生使能脉冲 */
    _nop_();
    LCD_EN = 0;
}

/*---------------------------------------
  写数据函数（标准阴码）
---------------------------------------*/
void lcd_write_data(uchar Rsspdata)
{
    LCD_RS = 1;        /* 选择数据寄存器 */
    LCD_RW = 0;        /* 写操作 */
    LCD_DATA = Rsspdata;/* 输出数据 */
    _nop_();           /* 时序延时 */
    LCD_EN = 1;        /* 产生使能脉冲 */
    _nop_();
    LCD_EN = 0;
}

/*---------------------------------------
  清屏函数
---------------------------------------*/
void lcd_Clr_Scr(void)
{
    uchar j, k;
    LCD_CS1 = 1;       /* 同时选通左右半屏 */
    LCD_CS2 = 1;
    
    for(k=0; k<8; k++) /* 遍历8行（每页） */
    {
        lcd_write_com(0xB8 + k); /* 设置页地址 */
        lcd_write_com(0x40);     /* 设置列地址为0 */
        for(j=0; j<64; j++)      /* 每行64列 */
        {
            lcd_write_data(0xFF); /* 写入1熄灭所有像素（配合阴码） */
        }
    }
}

/*---------------------------------------
  设置坐标函数 
---------------------------------------*/
void lcd_werite_x_y(uchar x, uchar y)
{
    y = y & 0x7F; /* 限制列地址范围0-127 */
    x = x & 0x07; /* 限制页地址范围0-7 */
    
    if(y < 64)    /* 左半屏(0-63列) */
    {
        LCD_CS1 = 0;
        LCD_CS2 = 1;
        lcd_write_com(0x40 + y);
    }
    else          /* 右半屏(64-127列) */
    {
        LCD_CS1 = 1;
        LCD_CS2 = 0;
        lcd_write_com(0x40 + (y & 0x3F)); /* 转换为右半屏列地址0-63 */
    }
    
    lcd_write_com(0xB8 + x); /* 设置页地址 */
}

/*---------------------------------------
  指定位置显示16x16汉字
  正确访问结构体数组的Data字段
---------------------------------------*/
void lcd_write_chinese(uchar x, uchar y, uchar hz)
{
    uchar i;
    
    lcd_werite_x_y(x, y);
    for(i=0; i<16; i++)
        lcd_write_data(LCD_CF16x16[hz].Data[i]); /* 显示上半部分(16字节) */
    
    lcd_werite_x_y(x+1, y);
    for(i=0; i<16; i++)
        lcd_write_data(LCD_CF16x16[hz].Data[16 + i]); /* 显示下半部分(16字节) */
}        

/*---------------------------------------
  指定位置显示8x16字符 
---------------------------------------*/
void lcd_write_english(uchar x, uchar y, uchar en)
{
    uchar i;
    uchar index;
        
    index = en - 32;
    
    /* 防止索引越界 */
    if(index >= 95)
    {
        index = 0; /* 显示空格 */
    }
    
    lcd_werite_x_y(x, y);
    for(i=0; i<8; i++)
        lcd_write_data(LCD_F8x16[index][i]); /* 显示上半部分(8字节) */
    
    lcd_werite_x_y(x+1, y);
    for(i=0; i<8; i++)
        lcd_write_data(LCD_F8x16[index][8 + i]); /* 显示下半部分(8字节) */
}	 

/*---------------------------------------
  自动汉字查找函数
---------------------------------------*/
uchar lcd_get_chinese_index(uchar gb1, uchar gb2)
{
    uchar i;
    uchar count;
    
    /* 自动计算字库中汉字的个数 */
    count = sizeof(LCD_CF16x16) / sizeof(ChineseCell_t);
    
    /* 遍历所有汉字，比较编码 */
    for(i = 0; i < count - 1; i++)
    {
        if((LCD_CF16x16[i].Index[0] == gb1) && 
           (LCD_CF16x16[i].Index[1] == gb2))
        {
            return i;
        }
    }
    
    /* 找不到返回第一个（空白） */
    return count ;
}

/*---------------------------------------
  显示字符串，支持混合中英文
---------------------------------------*/
void lcd_show_string(uchar x, uchar y, const uchar *str, uchar font)
{
    uchar i = 0;
    uchar char_width;
    uchar char_height;
    uchar hz_index;
    
    /* 根据字体设置字符宽高 */
    if(font == LCD_8X16)
    {
        char_width = 8;
        char_height = 16;
    }
    else if(font == LCD_16X16)
    {
        char_width = 16;
        char_height = 16;
    }
    else
    {
        return; /* 不支持的字体 */
    }
    
    while(str[i] != '\0')
    {
        /* 判断是否为GB2312汉字（第一个字节大于0xA0） */
        if(str[i] > 0xA0)
        {
            /* 显示汉字，占2个字节 */
            hz_index = lcd_get_chinese_index(str[i], str[i+1]);
            lcd_write_chinese(x, y, hz_index);
            i += 2;
            y += char_width;
        }
        else
        {
            /* 显示ASCII字符，占1个字节 */
            lcd_write_english(x, y, str[i]);
            i += 1;
            y += (char_width / 2);
        }
        
        /* 自动换行 */
        if(y > 128 - (char_width / 2))
        {
            y = 0;
            x += (char_height / 8);
        }
        
        /* 超出屏幕范围停止显示 */
        if(x > 6) /* 16x16字体最多显示4行（0-6页） */
        {
            break;
        }
    }
}
