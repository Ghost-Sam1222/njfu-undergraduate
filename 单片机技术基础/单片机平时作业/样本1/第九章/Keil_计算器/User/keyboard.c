#include "keyboard.h"

bit key_lock = 0;

uchar code key_num_table[] = {
    7, 8, 9, 0,
    4, 5, 6, 0,
    1, 2, 3, 0,
    0, 0, 0, 0
};

uchar code key_lcd_table[] = {
    '7'-0x30, '8'-0x30, '9'-0x30, '/'-0x30,
    '4'-0x30, '5'-0x30, '6'-0x30, '*'-0x30,
    '1'-0x30, '2'-0x30, '3'-0x30, '-'-0x30,
    0,         '0'-0x30, '='-0x30, '+'-0x30
};

uchar key_scan(void)
{
    uchar temp;
    uchar key = KEY_NONE;
    
    /* P3.0 ×óÇÐ¼ü */
    if((P3 & 0x01) == 0) {
        delay(25);
        if((P3 & 0x01) == 0) { while((P3 & 0x01) == 0); return KEY_LEFT; }
    }
    /* P3.1 ÓÒÇÐ¼ü */
    if((P3 & 0x02) == 0) {
        delay(25);
        if((P3 & 0x02) == 0) { while((P3 & 0x02) == 0); return KEY_RIGHT; }
    }
    /* P3.2 É¾³ý¼ü */
    if((P3 & 0x04) == 0) {
        delay(25);
        if((P3 & 0x04) == 0) { while((P3 & 0x04) == 0); return KEY_DEL; }
    }
    /* P3.3 Ð¡Êýµã¼ü */
    if((P3 & 0x08) == 0) {
        delay(25);
        if((P3 & 0x08) == 0) { while((P3 & 0x08) == 0); return KEY_DOT; }
    }
    /* P3.4 ×óÀ¨ºÅ */
    if((P3 & 0x10) == 0) {
        delay(25);
        if((P3 & 0x10) == 0) { while((P3 & 0x10) == 0); return KEY_LPAREN; }
    }
    /* P3.5 ÓÒÀ¨ºÅ */
    if((P3 & 0x20) == 0) {
        delay(25);
        if((P3 & 0x20) == 0) { while((P3 & 0x20) == 0); return KEY_RPAREN; }
    }
    /* P3.6 Ans¼ü */
    if((P3 & 0x40) == 0) {
        delay(25);
        if((P3 & 0x40) == 0) { while((P3 & 0x40) == 0); return KEY_ANS; }
    }
    
    if(key_lock) {
        P2 = 0xF0;
        if(P2 == 0xF0) { delay(30); key_lock = 0; }
        return KEY_NONE;
    }
    
    /* 4x4¾ØÕó¼üÅÌÉ¨Ãè */
    P2 = 0xFE;
    if(P2 != 0xFE) {
        delay(25);
        if(P2 != 0xFE) {
            temp = P2 & 0xF0;
            switch(temp) {
                case 0xE0: key = KEY_7; break;
                case 0xD0: key = KEY_8; break;
                case 0xB0: key = KEY_9; break;
                case 0x70: key = KEY_DIV; break;
            }
        }
    }
    if(key == KEY_NONE) {
        P2 = 0xFD;
        if(P2 != 0xFD) {
            delay(25);
            if(P2 != 0xFD) {
                temp = P2 & 0xF0;
                switch(temp) {
                    case 0xE0: key = KEY_4; break;
                    case 0xD0: key = KEY_5; break;
                    case 0xB0: key = KEY_6; break;
                    case 0x70: key = KEY_MUL; break;
                }
            }
        }
    }
    if(key == KEY_NONE) {
        P2 = 0xFB;
        if(P2 != 0xFB) {
            delay(25);
            if(P2 != 0xFB) {
                temp = P2 & 0xF0;
                switch(temp) {
                    case 0xE0: key = KEY_1; break;
                    case 0xD0: key = KEY_2; break;
                    case 0xB0: key = KEY_3; break;
                    case 0x70: key = KEY_SUB; break;
                }
            }
        }
    }
    if(key == KEY_NONE) {
        P2 = 0xF7;
        if(P2 != 0xF7) {
            delay(25);
            if(P2 != 0xF7) {
                temp = P2 & 0xF0;
                switch(temp) {
                    case 0xE0: key = KEY_C; break;
                    case 0xD0: key = KEY_0; break;
                    case 0xB0: key = KEY_EQUAL; break;
                    case 0x70: key = KEY_ADD; break;
                }
            }
        }
    }
    
    if(key != KEY_NONE) key_lock = 1;
    return key;
}

uchar key_get_value(uchar key_code) {
    return key_num_table[key_code];
}

uchar key_get_lcd_char(uchar key_code) {
    return 0x30 + key_lcd_table[key_code];
}

bit is_number_key(uchar key_code) {
    return (key_code == KEY_0 || key_code == KEY_1 || key_code == KEY_2 ||
            key_code == KEY_3 || key_code == KEY_4 || key_code == KEY_5 ||
            key_code == KEY_6 || key_code == KEY_7 || key_code == KEY_8 ||
            key_code == KEY_9);
}

bit is_operator_key(uchar key_code) {
    return (key_code == KEY_ADD || key_code == KEY_SUB ||
            key_code == KEY_MUL || key_code == KEY_DIV);
}