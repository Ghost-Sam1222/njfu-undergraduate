#include "calculator.h"

Calculator calc;
float last_result = 0;    /* Ans存储 */

void calc_init(void) {
    uchar i;
    calc.input_len = 0;
    calc.cursor_pos = 0;
    calc.j = 0;
    for(i = 0; i < MAX_INPUT; i++)
        calc.input_buf[i] = 0;
    lcd_write_com(0x0F);
}

void calc_reset(void) {
    lcd_write_com(0x01);
    calc_init();
}

void calc_refresh_display(void) {
    uchar i;
    lcd_write_com(0x80);
    for(i = 0; i < calc.input_len; i++)
        lcd_write_date(calc.input_buf[i]);
    lcd_write_com(0x80 + calc.cursor_pos);
}

/* ===== 合法性校验  ===== */
bit calc_validate_input(uchar ch) {
    uchar prev_char;
    uchar i, dot_count;
    
    /* 第一个字符 */
    if(calc.input_len == 0 || calc.cursor_pos == 0) {
        if(ch >= '0' && ch <= '9') return 1;
        if(ch == '-') return 1;    /* 只允许一个开头负号 */
        if(ch == '(') return 1;
        if(ch == 'A') return 1;
        return 0;
    }
    
    prev_char = calc.input_buf[calc.cursor_pos - 1];
    
    /* 禁止连续两个运算符 */
    if(prev_char == '+' || prev_char == '-' || 
       prev_char == '*' || prev_char == '/') {
        if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || 
           ch == '.' || ch == ')')
            return 0;  
        if((ch >= '0' && ch <= '9') || ch == '(' || ch == 'A')
            return 1;
        return 0;
    }
    
    if(prev_char >= '0' && prev_char <= '9') {
        if(ch == 'A' || ch == '(') return 0;
    }
    
    if(prev_char == '(') {
        if(ch == '+' || ch == '*' || ch == '/' || ch == '.' || ch == ')')
            return 0;
        if(ch == '-') return 1;  /* 括号后允许负号：(-3) */
        return 1;
    }
    
    if(prev_char == ')') {
        if((ch >= '0' && ch <= '9') || ch == '.' || ch == 'A' || ch == '(')
            return 0;
        return 1;
    }
    
    if(prev_char == 'A') {
        if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ')')
            return 1;
        return 0;
    }
    
    /* 小数点检查 */
    if(ch == '.') {
        dot_count = 0;
        for(i = calc.cursor_pos - 1; i < 255; i--) {
            if(calc.input_buf[i] == '+' || calc.input_buf[i] == '-' ||
               calc.input_buf[i] == '*' || calc.input_buf[i] == '/' ||
               calc.input_buf[i] == '(' || calc.input_buf[i] == ')')
                break;
            if(calc.input_buf[i] == '.') { dot_count++; break; }
        }
        if(dot_count > 0) return 0;
    }
    
    return 1;
}

void calc_input_char(uchar ch) {
    uchar i;
    if(calc.j == 1) calc_reset();
    if(!calc_validate_input(ch)) return;
    
    for(i = calc.input_len; i > calc.cursor_pos; i--)
        calc.input_buf[i] = calc.input_buf[i - 1];
    
    calc.input_buf[calc.cursor_pos] = ch;
    calc.input_len++;
    calc.cursor_pos++;
    calc_refresh_display();
}

void calc_input_number(uchar num) {
    calc_input_char(0x30 + num);
    if(calc.j != 0) calc.j = 0;
}

void calc_input_dot(void) { calc_input_char('.'); }
void calc_input_lparen(void) { calc_input_char('('); }
void calc_input_rparen(void) { calc_input_char(')'); }
void calc_input_ans(void) { calc_input_char('A'); }

void calc_set_operator(uchar op) {
    uchar op_char;
    switch(op) {
        case OP_ADD: op_char = '+'; break;
        case OP_SUB: op_char = '-'; break;
        case OP_MUL: op_char = '*'; break;
        case OP_DIV: op_char = '/'; break;
        default: return;
    }
    calc_input_char(op_char);
}

void calc_cursor_left(void) {
    if(calc.j == 1) { calc_back_to_input(); return; }
    if(calc.cursor_pos > 0) calc.cursor_pos--;
    else calc.cursor_pos = calc.input_len;
    lcd_write_com(0x80 + calc.cursor_pos);
}

void calc_cursor_right(void) {
    if(calc.j == 1) { calc_back_to_input(); return; }
    if(calc.cursor_pos < calc.input_len) calc.cursor_pos++;
    else calc.cursor_pos = 0;
    lcd_write_com(0x80 + calc.cursor_pos);
}

void calc_delete_char(void) {
    uchar i;
    if(calc.j == 1) { calc_back_to_input(); return; }
    if(calc.cursor_pos == 0) return;
    
    for(i = calc.cursor_pos - 1; i < calc.input_len - 1; i++)
        calc.input_buf[i] = calc.input_buf[i + 1];
    
    calc.input_buf[calc.input_len - 1] = ' ';
    calc.input_len--;
    calc.cursor_pos--;
    
    lcd_write_com(0x01);
    lcd_write_com(0x80);
    for(i = 0; i < calc.input_len; i++)
        lcd_write_date(calc.input_buf[i]);
    lcd_write_com(0x80 + calc.cursor_pos);
}

void calc_back_to_input(void) {
    lcd_write_com(0x01);
    calc.j = 0;
    calc_refresh_display();
    lcd_write_com(0x0F);
}

uchar get_priority(uchar op) {
    if(op == '+' || op == '-') return 1;
    if(op == '*' || op == '/') return 2;
    return 0;
}


void calc_calculate(void) {
    float nums[8];
    uchar ops[8];
    uchar ntop, otop, i, ch, paren_count;
    float current_num, decimal, a, b;
    uchar has_dot, op, num_pending;
    float final_result;
    
    calc.j = 1;
    lcd_write_com(0x0C);
    
    /* 括号匹配检查 */
    paren_count = 0;
    for(i = 0; i < calc.input_len; i++) {
        if(calc.input_buf[i] == '(') paren_count++;
        if(calc.input_buf[i] == ')') paren_count--;
        if(paren_count < 0) { calc_display_error(); return; }
    }
    if(paren_count != 0) { calc_display_error(); return; }
    
    ntop = 0; otop = 0;
    current_num = 0; has_dot = 0; decimal = 0.1;
    num_pending = 0;  /* 标记：是否有待入栈的数字 */
    
    for(i = 0; i < calc.input_len; i++) {
        ch = calc.input_buf[i];
        
        
        if(ch == 'A') {
            nums[ntop++] = last_result;
            num_pending = 0;  
            continue;
        }
        
        if(ch >= '0' && ch <= '9') {
            if(has_dot == 0)
                current_num = current_num * 10 + (float)(ch - '0');
            else {
                current_num += (float)(ch - '0') * decimal;
                decimal *= 0.1;
            }
            num_pending = 1;
        }
        else if(ch == '.') {
            has_dot = 1; decimal = 0.1;
            num_pending = 1;
        }
        else if(ch == '(') {
            ops[otop++] = ch;
        }
        else if(ch == ')') {
            /* 先把当前数字入栈 */
            if(num_pending) {
                nums[ntop++] = current_num;
                current_num = 0; has_dot = 0; num_pending = 0;
            }
            /* 计算到左括号 */
            while(otop > 0 && ops[otop-1] != '(') {
                b = nums[--ntop];
                a = nums[--ntop];
                op = ops[--otop];
                if(op == '/' && b < 0.0001 && b > -0.0001)
                    { calc_display_error(); return; }
                if(op == '+') nums[ntop++] = a + b;
                else if(op == '-') nums[ntop++] = a - b;
                else if(op == '*') nums[ntop++] = a * b;
                else if(op == '/') nums[ntop++] = a / b;
            }
            if(otop > 0) otop--;  /* 弹出左括号 */
        }
        else if(ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            /* 负号：开头或括号后 */
            if(ch == '-' && (i == 0 || calc.input_buf[i-1] == '(')) {
                nums[ntop++] = 0;
                ops[otop++] = '-';
                continue;
            }
            
            /* 只有确实有数字时才入栈 */
            if(num_pending) {
                nums[ntop++] = current_num;
                current_num = 0; has_dot = 0; num_pending = 0;
            }
            
            /* 优先级高的先算 */
            while(otop > 0 && ops[otop-1] != '(' &&
                  get_priority(ops[otop-1]) >= get_priority(ch)) {
                b = nums[--ntop];
                a = nums[--ntop];
                op = ops[--otop];
                if(op == '/' && b < 0.0001 && b > -0.0001)
                    { calc_display_error(); return; }
                if(op == '+') nums[ntop++] = a + b;
                else if(op == '-') nums[ntop++] = a - b;
                else if(op == '*') nums[ntop++] = a * b;
                else if(op == '/') nums[ntop++] = a / b;
            }
            ops[otop++] = ch;
        }
    }
    
    /* 最后一个数字入栈 */
    if(num_pending) {
        nums[ntop++] = current_num;
    }
    
    /* 计算剩余 */
    while(otop > 0) {
        b = nums[--ntop];
        a = nums[--ntop];
        op = ops[--otop];
        if(op == '/' && b < 0.0001 && b > -0.0001)
            { calc_display_error(); return; }
        if(op == '+') nums[ntop++] = a + b;
        else if(op == '-') nums[ntop++] = a - b;
        else if(op == '*') nums[ntop++] = a * b;
        else if(op == '/') nums[ntop++] = a / b;
    }
    
    final_result = nums[0];
    last_result = final_result;  /* 保存Ans */
    calc_display_result(final_result);
}

/* ===== 浮点数精度四舍五入 ===== */
void calc_display_result(float result) {
    long int_part;
    float frac_part;
    long temp;
    uchar i;
    uchar negative = 0;
    
    if(result < 0) { negative = 1; result = -result; }
    
    /* 四舍五入到4位小数 */
    result = result + 0.00005;
    
    int_part = (long)result;
    frac_part = result - (float)int_part;
    
    lcd_write_com(0x80 + 0x4F);
    lcd_write_com(0x04);
    
    temp = (long)(frac_part * 10000);
    for(i = 0; i < 4; i++) {
        lcd_write_date(0x30 + temp % 10);
        temp = temp / 10;
    }
    lcd_write_date('.');
    
    temp = int_part;
    if(temp == 0) {
        lcd_write_date(0x30);
    } else {
        while(temp != 0) {
            lcd_write_date(0x30 + temp % 10);
            temp = temp / 10;
        }
    }
    
    if(negative) lcd_write_date(0x2D);
    lcd_write_date(0x3D);
    lcd_write_com(0x06);
}

void calc_display_error(void) {
    lcd_write_com(0x80 + 0x40);
    lcd_write_date('E'); lcd_write_date('R'); lcd_write_date('R');
    lcd_write_date('O'); lcd_write_date('R'); lcd_write_date('!');
}