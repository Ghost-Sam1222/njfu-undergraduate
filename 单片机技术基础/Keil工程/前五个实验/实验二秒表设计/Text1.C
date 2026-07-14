#include <reg51.h>

/*
 * Countdown from 60 to 00.
 * Wiring is the same as the original count-up program:
 * P3.0--DATA, P3.1--CLK.
 *
 * Speed note:
 * COUNT_DELAY_TICKS controls the pause after each number.
 * 100 is about 1 second, 20 is about 0.2 second.
 */
#define COUNT_DELAY_TICKS 20

unsigned char code table[] = {
    0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0,
    0xFE, 0xF6, 0xEE, 0x3E, 0x9C, 0x7A, 0x9E, 0x8E
};

unsigned char Count;       /* Current number shown on the display. */

void delay10ms(void)
{
    unsigned char i, j;

    /* Short software delay from the original experiment code. */
    for(i = 250; i > 0; i--)
        for(j = 248; j > 0; j--);
}

void delayCountSpeed(void)
{
    unsigned char k;

    /* Repeat delay10ms to set the countdown speed. */
    for(k = 0; k < COUNT_DELAY_TICKS; k++)
        delay10ms();
}

void putch(unsigned char n)
{
    /* Send one segment-code byte to the display module. */
    SBUF = n;
    while(TI == 0);
    TI = 0;
}

void display(unsigned char n)
{
    /* Send tens digit first, then ones digit. */
    putch(table[n / 10]);
    putch(table[n % 10]);
}

void main(void)
{
    Count = 60;

    while(1)
    {
        display(Count);
        delayCountSpeed();

        /* After 00, restart from 60. */
        if(Count == 0)
            Count = 60;
        else
            Count--;
    }
}
