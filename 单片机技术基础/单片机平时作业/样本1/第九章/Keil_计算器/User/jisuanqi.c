/***********************************************************
 * 51单片机多功能计算器
 * 
 * 功能说明:
 *   1. 支持 + - * / 四则运算
 *   2. 支持小数、负数、括号嵌套运算
 *   3. Ans键存储上一次结果，支持链式计算
 *   4. 光标左右移动、任意位置插入、DEL删除
 *   5. 输入合法性校验 + 错误提示
 * 
 * 环境: Keil uVision2 + Proteus 6.9 | 芯片: AT89C52
 ***********************************************************/
#include <reg52.h>
#include "lcd1602.h"
#include "keyboard.h"
#include "calculator.h"

void handle_key(uchar key) {
    uchar op;
    
    if(key == KEY_NONE) return;
    
    if(is_number_key(key)) {
        calc_input_number(key_get_value(key));
    }
    else if(key == KEY_C) {
        calc_reset();
    }
    else if(is_operator_key(key)) {
        switch(key) {
            case KEY_ADD: op = OP_ADD; break;
            case KEY_SUB: op = OP_SUB; break;
            case KEY_MUL: op = OP_MUL; break;
            case KEY_DIV: op = OP_DIV; break;
            default: return;
        }
        calc_set_operator(op);
    }
    else if(key == KEY_EQUAL) {
        calc_calculate();
    }
    else if(key == KEY_LEFT)  { calc_cursor_left(); }
    else if(key == KEY_RIGHT) { calc_cursor_right(); }
    else if(key == KEY_DEL)   { calc_delete_char(); }
    else if(key == KEY_DOT)   { calc_input_dot(); }
    else if(key == KEY_LPAREN){ calc_input_lparen(); }
    else if(key == KEY_RPAREN){ calc_input_rparen(); }
    else if(key == KEY_ANS)   { calc_input_ans(); }
}

void main(void) {
    uchar key;
    lcd_init();
    calc_init();
    while(1) {
        key = key_scan();
        handle_key(key);
    }
}
