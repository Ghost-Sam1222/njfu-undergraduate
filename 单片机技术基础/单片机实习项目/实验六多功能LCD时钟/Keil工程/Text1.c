#include <REG52.H>

typedef unsigned char uchar;
typedef unsigned int uint;

sbit LCD_RS = P2^0;
sbit LCD_E  = P2^1;

sbit BUZZER = P1^0;

sbit KEY_MODE = P3^0;
sbit KEY_ADD  = P3^1;
sbit KEY_MINUS= P3^2;
sbit KEY_OK   = P3^3;

sbit DS1302_IO   = P3^4;
sbit DS1302_CE   = P3^5;
sbit DS1302_SCLK = P3^6;

#define KEY_NONE 0
#define KEY_MODE_ID 1
#define KEY_ADD_ID 2
#define KEY_MINUS_ID 3
#define KEY_OK_ID 4

#define ALARM_COUNT 3

typedef struct {
    uchar hour;
    uchar min;
    uchar enable;
} Alarm;

uchar hour_now, min_now, sec_now;
uchar edit_mode = 0;
uchar last_ring_hour = 0xff;
uchar last_ring_min = 0xff;
uchar ring_step = 0;
uchar ring_tick = 0;
uchar alarm_view = 0;
uchar alarm_view_tick = 0;
bit ringing = 0;

Alarm alarms[ALARM_COUNT] = {
    {7, 0, 1},
    {12, 0, 0},
    {18, 0, 0}
};

uchar code ring_pattern[] = {
    4, 2, 4, 2, 4, 2, 8, 4,
    4, 2, 4, 2, 8, 6
};

void delay_us(uint t)
{
    while(t--);
}

void delay_ms(uint ms)
{
    uint i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 120; j++);
}

uchar dec_to_bcd(uchar dat)
{
    return (dat / 10) * 16 + dat % 10;
}

uchar bcd_to_dec(uchar dat)
{
    return (dat >> 4) * 10 + (dat & 0x0f);
}

void lcd_write_cmd(uchar cmd)
{
    LCD_RS = 0;
    P0 = cmd;
    delay_ms(1);
    LCD_E = 1;
    delay_ms(1);
    LCD_E = 0;
}

void lcd_write_dat(uchar dat)
{
    LCD_RS = 1;
    P0 = dat;
    delay_ms(1);
    LCD_E = 1;
    delay_ms(1);
    LCD_E = 0;
}

void lcd_pos(uchar row, uchar col)
{
    if(row == 0)
        lcd_write_cmd(0x80 + col);
    else
        lcd_write_cmd(0xc0 + col);
}

void lcd_string(uchar row, uchar col, char *s)
{
    lcd_pos(row, col);
    while(*s)
        lcd_write_dat(*s++);
}

void lcd_two_digit(uchar dat)
{
    lcd_write_dat(dat / 10 + '0');
    lcd_write_dat(dat % 10 + '0');
}

void lcd_init(void)
{
    LCD_E = 0;
    delay_ms(20);
    lcd_write_cmd(0x38);
    lcd_write_cmd(0x0c);
    lcd_write_cmd(0x06);
    lcd_write_cmd(0x01);
}

void ds1302_write_byte(uchar dat)
{
    uchar i;
    for(i = 0; i < 8; i++) {
        DS1302_IO = dat & 0x01;
        delay_us(2);
        DS1302_SCLK = 1;
        delay_us(2);
        DS1302_SCLK = 0;
        dat >>= 1;
    }
}

uchar ds1302_read_byte(void)
{
    uchar i, dat = 0;
    DS1302_IO = 1;
    for(i = 0; i < 8; i++) {
        if(DS1302_IO)
            dat |= (1 << i);
        DS1302_SCLK = 1;
        delay_us(2);
        DS1302_SCLK = 0;
        delay_us(2);
    }
    return dat;
}

void ds1302_write(uchar addr, uchar dat)
{
    DS1302_CE = 0;
    DS1302_SCLK = 0;
    DS1302_CE = 1;
    ds1302_write_byte(addr);
    ds1302_write_byte(dat);
    DS1302_CE = 0;
}

uchar ds1302_read(uchar addr)
{
    uchar dat;
    DS1302_CE = 0;
    DS1302_SCLK = 0;
    DS1302_CE = 1;
    ds1302_write_byte(addr);
    dat = ds1302_read_byte();
    DS1302_CE = 0;
    return dat;
}

void rtc_write_time(void)
{
    ds1302_write(0x8e, 0x00);
    ds1302_write(0x80, dec_to_bcd(sec_now));
    ds1302_write(0x82, dec_to_bcd(min_now));
    ds1302_write(0x84, dec_to_bcd(hour_now));
    ds1302_write(0x8e, 0x80);
}

void rtc_read_time(void)
{
    sec_now = bcd_to_dec(ds1302_read(0x81) & 0x7f);
    min_now = bcd_to_dec(ds1302_read(0x83) & 0x7f);
    hour_now = bcd_to_dec(ds1302_read(0x85) & 0x3f);
}

void rtc_init(void)
{
    uchar sec_reg;
    DS1302_CE = 0;
    DS1302_SCLK = 0;
    sec_reg = ds1302_read(0x81);
    if(sec_reg & 0x80) {
        hour_now = 12;
        min_now = 0;
        sec_now = 0;
        rtc_write_time();
    }
}

uchar key_scan(void)
{
    KEY_MODE = 1;
    KEY_ADD = 1;
    KEY_MINUS = 1;
    KEY_OK = 1;

    if(KEY_MODE == 0) {
        delay_ms(15);
        if(KEY_MODE == 0) {
            while(KEY_MODE == 0);
            return KEY_MODE_ID;
        }
    }
    if(KEY_ADD == 0) {
        delay_ms(15);
        if(KEY_ADD == 0) {
            while(KEY_ADD == 0);
            return KEY_ADD_ID;
        }
    }
    if(KEY_MINUS == 0) {
        delay_ms(15);
        if(KEY_MINUS == 0) {
            while(KEY_MINUS == 0);
            return KEY_MINUS_ID;
        }
    }
    if(KEY_OK == 0) {
        delay_ms(15);
        if(KEY_OK == 0) {
            while(KEY_OK == 0);
            return KEY_OK_ID;
        }
    }
    return KEY_NONE;
}

void add_value(bit up)
{
    uchar alarm_index;
    uchar alarm_field;
    if(edit_mode == 1) {
        hour_now = up ? (hour_now + 1) % 24 : (hour_now + 23) % 24;
    } else if(edit_mode == 2) {
        min_now = up ? (min_now + 1) % 60 : (min_now + 59) % 60;
    } else if(edit_mode == 3) {
        sec_now = up ? (sec_now + 1) % 60 : (sec_now + 59) % 60;
    } else if(edit_mode >= 4) {
        alarm_index = (edit_mode - 4) / 3;
        alarm_field = (edit_mode - 4) % 3;
        if(alarm_field == 0)
            alarms[alarm_index].hour = up ? (alarms[alarm_index].hour + 1) % 24 : (alarms[alarm_index].hour + 23) % 24;
        else if(alarm_field == 1)
            alarms[alarm_index].min = up ? (alarms[alarm_index].min + 1) % 60 : (alarms[alarm_index].min + 59) % 60;
        else
            alarms[alarm_index].enable = !alarms[alarm_index].enable;
    }
}

void key_process(void)
{
    uchar key = key_scan();
    if(key == KEY_NONE)
        return;

    if(key == KEY_OK_ID) {
        ringing = 0;
        BUZZER = 0;
        if(edit_mode != 0) {
            edit_mode = 0;
            rtc_write_time();
        }
    } else if(key == KEY_MODE_ID) {
        if(edit_mode == 0)
            edit_mode = 1;
        else {
            if(edit_mode == 3)
                rtc_write_time();
            edit_mode++;
            if(edit_mode > 3 + ALARM_COUNT * 3)
                edit_mode = 1;
        }
    } else if(key == KEY_ADD_ID) {
        if(edit_mode != 0)
            add_value(1);
    } else if(key == KEY_MINUS_ID) {
        if(edit_mode != 0)
            add_value(0);
    }
}

void alarm_sound_task(void)
{
    if(ringing) {
        BUZZER = 1;
    } else {
        BUZZER = 0;
        ring_step = 0;
        ring_tick = 0;
    }
}

void check_alarm(void)
{
    uchar i;
    if(ringing)
        return;
    if(last_ring_hour == hour_now && last_ring_min == min_now)
        return;

    for(i = 0; i < ALARM_COUNT; i++) {
        if(alarms[i].enable && alarms[i].hour == hour_now && alarms[i].min == min_now) {
            ringing = 1;
            last_ring_hour = hour_now;
            last_ring_min = min_now;
            break;
        }
    }
}

void show_time_line(void)
{
    lcd_string(0, 0, "Time ");
    lcd_two_digit(hour_now);
    lcd_write_dat(':');
    lcd_two_digit(min_now);
    lcd_write_dat(':');
    lcd_two_digit(sec_now);
    lcd_write_dat(' ');
    if(ringing)
        lcd_string(0, 14, "AL");
    else if(edit_mode)
        lcd_string(0, 14, "SE");
    else
        lcd_string(0, 14, "  ");
}

void show_alarm_line(void)
{
    uchar alarm_index = alarm_view;
    uchar alarm_field = 0;
    if(edit_mode >= 4) {
        alarm_index = (edit_mode - 4) / 3;
        alarm_field = (edit_mode - 4) % 3;
    }

    lcd_pos(1, 0);
    if(edit_mode >= 1 && edit_mode <= 3) {
        lcd_string(1, 0, "Set ");
        if(edit_mode == 1) lcd_string(1, 4, "Hour        ");
        if(edit_mode == 2) lcd_string(1, 4, "Minute      ");
        if(edit_mode == 3) lcd_string(1, 4, "Second      ");
        return;
    }

    lcd_write_dat('A');
    lcd_write_dat(alarm_index + '1');
    lcd_write_dat(' ');
    lcd_two_digit(alarms[alarm_index].hour);
    lcd_write_dat(':');
    lcd_two_digit(alarms[alarm_index].min);
    lcd_write_dat(' ');
    if(alarms[alarm_index].enable)
        lcd_string(1, 9, "ON ");
    else
        lcd_string(1, 9, "OFF");

    if(edit_mode >= 4) {
        lcd_write_dat(' ');
        if(alarm_field == 0) lcd_write_dat('H');
        if(alarm_field == 1) lcd_write_dat('M');
        if(alarm_field == 2) lcd_write_dat('E');
        lcd_write_dat(' ');
    } else {
        lcd_string(1, 12, "    ");
    }
}

void update_alarm_view(void)
{
    if(edit_mode == 0) {
        alarm_view_tick++;
        if(alarm_view_tick >= 25) {
            alarm_view_tick = 0;
            alarm_view++;
            if(alarm_view >= ALARM_COUNT)
                alarm_view = 0;
        }
    } else if(edit_mode >= 4) {
        alarm_view = (edit_mode - 4) / 3;
        alarm_view_tick = 0;
    } else {
        alarm_view_tick = 0;
    }
}

void display_refresh(void)
{
    update_alarm_view();
    show_time_line();
    show_alarm_line();
}

void main(void)
{
    lcd_init();
    rtc_init();
    BUZZER = 0;

    while(1) {
        if(edit_mode == 0 || edit_mode >= 4)
            rtc_read_time();
        key_process();
        check_alarm();
        alarm_sound_task();
        display_refresh();
        delay_ms(60);
    }
}
