#ifndef __CALCULATOR_H
#define __CALCULATOR_H

#include <reg52.h>
#include "lcd1602.h"
#include "keyboard.h"

#define OP_NONE     0
#define OP_ADD      1
#define OP_SUB      2
#define OP_MUL      3
#define OP_DIV      4
#define MAX_INPUT   16

typedef struct {
    uchar input_buf[MAX_INPUT];
    uchar input_len;
    uchar cursor_pos;
    uchar j;
} Calculator;

extern Calculator calc;
extern float last_result;

void calc_init(void);
void calc_reset(void);
bit  calc_validate_input(uchar ch);
void calc_input_char(uchar ch);
void calc_input_number(uchar num);
void calc_input_dot(void);
void calc_input_lparen(void);
void calc_input_rparen(void);
void calc_input_ans(void);
void calc_set_operator(uchar op);
void calc_calculate(void);
void calc_cursor_left(void);
void calc_cursor_right(void);
void calc_delete_char(void);
void calc_back_to_input(void);
void calc_display_result(float result);
void calc_display_error(void);
void calc_refresh_display(void);

#endif