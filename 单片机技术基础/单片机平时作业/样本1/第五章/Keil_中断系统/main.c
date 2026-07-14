#include <reg51.h>

/************************ 接线 ************************/
sbit KEY_LOCK  = P3^2;  // 选择/锁定键 → 接P3.2(INT0)
sbit KEY_ADD   = P3^3;  // +键 → 接P3.3(INT1)
sbit KEY_LEFT  = P3^5;  // 左切 → 接P3.5
sbit KEY_RIGHT = P3^6;  // 右切 → 接P3.6
sbit KEY_SUB   = P3^7;  // -键 → 接P3.7

/************************ 全局变量 ************************/
unsigned char code SEG_TABLE[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
unsigned char disp_buf[3] = {6, 6, 6};  // 上电666

unsigned char sel_bit = 0;      // 0=百 1=十 2=个
bit is_selected = 0;            // 0=锁定常亮 1=选中闪烁
unsigned char flash_cnt = 0;
bit flash_flag = 0;

bit lock_key_flag = 0;
bit add_key_flag = 0;

/************************ 基础函数 ************************/
void delay_ms(unsigned int ms)
{
    unsigned int i,j;
    for(i=ms; i>0; i--)
        for(j=110; j>0; j--);
}

void check_target(void)
{
    if(disp_buf[0]==1 && disp_buf[1]==9 && disp_buf[2]==3)
    {
        is_selected = 0;
        flash_flag = 0;
        flash_cnt = 0;
    }
}

/************************ 数码管扫描 ************************/
void display_scan(void)
{
    if(is_selected == 1)
    {
        flash_cnt++;
        if(flash_cnt >= 50)
        {
            flash_cnt = 0;
            flash_flag = !flash_flag;
        }
    }

    P2 = 0xFF;
    if(sel_bit==0 && is_selected && flash_flag) P0 = 0x00;
    else P0 = SEG_TABLE[disp_buf[0]];
    P2 = ~0x01;
    delay_ms(1);

    P2 = 0xFF;
    if(sel_bit==1 && is_selected && flash_flag) P0 = 0x00;
    else P0 = SEG_TABLE[disp_buf[1]];
    P2 = ~0x02;
    delay_ms(1);

    P2 = 0xFF;
    if(sel_bit==2 && is_selected && flash_flag) P0 = 0x00;
    else P0 = SEG_TABLE[disp_buf[2]];
    P2 = ~0x04;
    delay_ms(1);
}

/************************ 中断0：选定数码管 ************************/
void int0_isr(void) interrupt 0 using 1
{
    lock_key_flag = 1;  // 按选择键触发中断0
}

/************************ 中断1：调整数据 ************************/
void int1_isr(void) interrupt 2 using 1
{
    add_key_flag = 1;  // 按+键触发中断1
}

/************************ 主函数 ************************/
void main(void)
{
    unsigned char debounce = 0;
    
    P0 = 0x00;
    P2 = 0xFF;

    // 中断配置 
    IT0 = 1;  // INT0下降沿触发
    IT1 = 1;  // INT1下降沿触发
    EX0 = 1;  // 中断0负责选定
    EX1 = 1;  // 中断1负责调整
    EA = 1;

    while(1)
    {
        // ---------- 中断0处理：选择/锁定 ----------
        if(lock_key_flag)
        {
            debounce++;
            if(debounce >= 20)
            {
                debounce = 0;
                if(KEY_LOCK == 0)
                {
                    is_selected = !is_selected;
                    flash_flag = 0;
                    flash_cnt = 0;
                    while(KEY_LOCK == 0) display_scan();  // 等待松开
                }
                lock_key_flag = 0;
            }
        }

        // ---------- 中断1处理：数值+1 ----------
        if(add_key_flag && is_selected)
        {
            debounce++;
            if(debounce >= 20)
            {
                debounce = 0;
                if(KEY_ADD == 0)
                {
                    disp_buf[sel_bit]++;
                    if(disp_buf[sel_bit] > 9) disp_buf[sel_bit] = 0;
                    check_target();
                    while(KEY_ADD == 0) display_scan();  // 等待松开
                }
                add_key_flag = 0;
            }
        }

        // ---------- 非阻塞处理：左右切、-键 ----------
        if(is_selected)
        {
            if(KEY_LEFT == 0)
            {
                delay_ms(10);
                if(KEY_LEFT == 0 && sel_bit>0) sel_bit--;
                while(KEY_LEFT == 0) display_scan();
            }
            if(KEY_RIGHT == 0)
            {
                delay_ms(10);
                if(KEY_RIGHT == 0 && sel_bit<2) sel_bit++;
                while(KEY_RIGHT == 0) display_scan();
            }
            if(KEY_SUB == 0)
            {
                delay_ms(10);
                if(KEY_SUB == 0)
                {
                    if(disp_buf[sel_bit]==0) disp_buf[sel_bit]=9;
                    else disp_buf[sel_bit]--;
                    check_target();
                }
                while(KEY_SUB == 0) display_scan();
            }
        }

        display_scan();
    }
}
