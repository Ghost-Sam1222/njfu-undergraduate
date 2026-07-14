#ifndef __LCD12864_DATA_H
#define __LCD12864_DATA_H


typedef unsigned char  uint8_t;
typedef unsigned int   uint16_t;
typedef unsigned long  uint32_t;
typedef signed int     int16_t;  
typedef signed long    int32_t;  

/* 以下字符集定义只可注释一个 */
/* #define LCD_CHARSET_UTF8 */
#define LCD_CHARSET_GB2312  /* Keil2默认使用GB2312编码 */

/* 字模基本单元 */
typedef struct 
{
#ifdef LCD_CHARSET_UTF8
	char Index[5];
#endif
#ifdef LCD_CHARSET_GB2312
	char Index[3];
#endif
	uint8_t Data[32];
} ChineseCell_t;

/* ASCII字模数据声明 */
extern const uint8_t code LCD_F8x16[][16];
extern const uint8_t code LCD_F6x8[][6];

/* 汉字字模数据声明 */
extern const ChineseCell_t code LCD_CF16x16[];

/* 图像数据声明 */
extern const uint8_t code Diode[];

#endif