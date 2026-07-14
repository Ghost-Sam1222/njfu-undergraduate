#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <reg52.h>
#include "lcd1602.h"

#define KEY_NONE    0xFF
#define KEY_7       0
#define KEY_8       1
#define KEY_9       2
#define KEY_DIV     3
#define KEY_4       4
#define KEY_5       5
#define KEY_6       6
#define KEY_MUL     7
#define KEY_1       8
#define KEY_2       9
#define KEY_3       10
#define KEY_SUB     11
#define KEY_C       12      /* ON/C ÇåÁã */
#define KEY_0       13
#define KEY_EQUAL   14
#define KEY_ADD     15
#define KEY_LEFT    16      /* ×óÇÐ¼ü */
#define KEY_RIGHT   17      /* ÓÒÇÐ¼ü */
#define KEY_DEL     18      /* É¾³ý¼ü */
#define KEY_DOT     19      /* Ð¡Êýµã¼ü */
#define KEY_LPAREN  20      /* ×óÀ¨ºÅ */
#define KEY_RPAREN  21      /* ÓÒÀ¨ºÅ */
#define KEY_ANS     22      /* Ans¼ü */

extern uchar code key_num_table[];
extern uchar code key_lcd_table[];

uchar key_scan(void);
uchar key_get_value(uchar key_code);
uchar key_get_lcd_char(uchar key_code);
bit is_number_key(uchar key_code);
bit is_operator_key(uchar key_code);

#endif