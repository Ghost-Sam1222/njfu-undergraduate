#include <REG51.H>
#include <string.h>
#include "Delay.h"
#include "Key.h"
#include "Timer0.h"
#include "LCD12864.h"
#include "Music.h"
#include "Qingtian.h"
#include "Huahai.h"
#include "AiCuo.h" 
#include "YuAi.h" 
#include "FlowerDance.h"

/* 蜂鸣器引脚定义 */
sbit BEEP = P1^5;

/* 系统状态定义 */
#define STATE_SELECT 0    // 歌名选择状态
#define STATE_PLAYING 1   // 播放状态
#define STATE_PAUSED 2    // 暂停状态
#define STATE_FINISHED 3  // 播放结束状态

/* 歌曲结构体定义 */
typedef struct {
    unsigned char code *Music;
    unsigned char code *LyricBase;
    unsigned char code *LyricSwitch;
    unsigned char *Name;
    unsigned char LyricLen;
} Song;

Song songs[] = {
    {Music_QingTian, (unsigned char code *)Lyric_QingTian, LyricSwitch_QingTian, "晴天", LYRICLEN_QINGTIAN},
    {Music_HuaHai,   (unsigned char code *)Lyric_HuaHai,   LyricSwitch_HuaHai,   "花海", LYRICLEN_HUAHAI},
    {Music_AiCuo,    (unsigned char code *)Lyric_AiCuo,    LyricSwitch_AiCuo,    "爱错", LYRICLEN_AICUO},  
    {Music_YuAi,     (unsigned char code *)Lyric_YuAi,     LyricSwitch_YuAi,     "雨爱", LYRICLEN_YuAi},
    {Music_FlowerDance, (unsigned char code *)Lyric_FlowerDance, LyricSwitch_FlowerDance, "花之舞", LYRICLEN_FlowerDance}  
};

#define SONG_COUNT 5	//歌曲数目
#define TEMPO 200  		// 音乐播放速度

/* 全局变量 */
unsigned char currentSong = 0;		// 当前选中的歌曲索引
unsigned char state = STATE_SELECT; // 当前系统状态
unsigned int musicIndex = 0;		// 音乐数组索引
unsigned char currentLyric = 0;		// 当前显示的歌词索引
unsigned char currentNote = V0;		// 当前播放的音符
unsigned int beatCount = 0;			// 节拍计数器
unsigned int noteDuration = 0;  	// 当前音符剩余时长
unsigned char needRefresh = 1;		// LCD刷新标志
unsigned char notePlaying = 0;  	// 音符播放标志

/* 函数声明 */
void Timer1Init(void);
void ShowSelectScreen(void);
void ShowPlayScreen(void);
void ShowFinishScreen(void);
void PlayNextNote(void);
void StopMusic(void);

/* 定时器1初始化 - 1ms中断@12MHz，节拍计时 */
void Timer1Init(void)
{
    TMOD &= 0x0F;
    TMOD |= 0x10;
    TH1 = 0xFC;
    TL1 = 0x18;
    TF1 = 0;
    TR1 = 0;
    ET1 = 1;
    EA = 1;
    PT1 = 0;  
    PT0 = 1;  
}

/* 定时器0中断服务函数 */
void Timer0_Routine() interrupt 1
{
    unsigned int reload;
    reload = FreqTable[currentNote];
    
    TH0 = reload / 256;
    TL0 = reload % 256;
    
    if(reload != 0)  // 只有非休止符才翻转
    {
        BEEP = ~BEEP;
    }
    else
    {
        BEEP = 0;  // 休止符保持低电平
    }
}

/* 定时器1中断服务函数 - 节拍计时 */
void Timer1_Routine() interrupt 3
{
    TH1 = 0xFC;
    TL1 = 0x18;
    
    if(state == STATE_PLAYING && notePlaying)
    {
        if(beatCount > 0)
        {
            beatCount--;
        }
    }
}

/* 工具函数：擦除指定行 */
void EraseLine(unsigned char page)
{
    unsigned char i;
    lcd_werite_x_y(page, 0);
    for(i=0; i<8; i++) lcd_show_string(page, i*16, "  ", LCD_16X16);
}

/* 显示歌名选择界面 - 滚动显示版（支持任意多首歌） */
void ShowSelectScreen(void)
{
    unsigned char i;
    unsigned char offset = 0;  // 滚动偏移量
    
    // 计算滚动偏移：当前选中超过第3首（索引3）时，向上滚动
    if(currentSong >= 3)
    {
        offset = currentSong - 3;
    }
    
    // 只显示4首歌（LCD最多显示4行16x16汉字）
    for(i=0; i<4; i++)
    {
        unsigned char songIndex = offset + i;
        
        // 超出歌曲总数就停止显示
        if(songIndex >= SONG_COUNT)
        {
            break;
        }
        
        // 先擦除当前行
        EraseLine(i*2);
        
        if(songIndex == currentSong)
        {
            // 当前选中：> 歌名
            lcd_show_string(i*2, 16, "> ", LCD_16X16);
            lcd_show_string(i*2, 32, songs[songIndex].Name, LCD_16X16);
        }
        else
        {
            // 未选中：只显示歌名
            lcd_show_string(i*2, 32, songs[songIndex].Name, LCD_16X16);
        }
    }
    
    // 擦除多余的空行
    for(i=SONG_COUNT - offset; i<4; i++)
    {
        EraseLine(i*2);
    }
    
    needRefresh = 0;
}

/* 显示播放界面 - 三行歌词+歌名 */
void ShowPlayScreen(void)
{
    unsigned char nameLen;
    
    // 第一行：显示歌名
    EraseLine(0);
    nameLen = strlen(songs[currentSong].Name);
    lcd_show_string(0, (128 - nameLen*16)/2, songs[currentSong].Name, LCD_16X16);
    
    // 第二行：当前歌词
    EraseLine(2);
    lcd_show_string(2, 0, (unsigned char code *)(songs[currentSong].LyricBase + currentLyric * 16), LCD_16X16);
    
    // 第三行：下一句歌词
    EraseLine(4);
    if(currentLyric < songs[currentSong].LyricLen - 1)
    {
        lcd_show_string(4, 0, (unsigned char code *)(songs[currentSong].LyricBase + (currentLyric+1) * 16), LCD_16X16);
    }
    
    // 第四行：下下句歌词
    EraseLine(6);
    if(currentLyric < songs[currentSong].LyricLen - 2)
    {
        lcd_show_string(6, 0, (unsigned char code *)(songs[currentSong].LyricBase + (currentLyric+2) * 16), LCD_16X16);
    }
    
    needRefresh = 0;
}

/* 显示播放结束界面 */
void ShowFinishScreen(void)
{
    unsigned char nameLen;
    
    EraseLine(0);
    nameLen = strlen(songs[currentSong].Name);
    lcd_show_string(0, (128 - nameLen*16)/2, songs[currentSong].Name, LCD_16X16);
    
    EraseLine(2);
    lcd_show_string(2, 16, "歌曲播放完毕", LCD_16X16);
    
    EraseLine(4);
    lcd_show_string(4, 0, "按K1/K3切换歌曲", LCD_16X16);
    
    EraseLine(6);
    lcd_show_string(6, 16, "按K2重播", LCD_16X16);
    
    needRefresh = 0;
}

/* 播放下一个音符 */
void PlayNextNote(void)
{
    unsigned char note, duration;
    
    // 读取当前音符和时长
    note = songs[currentSong].Music[musicIndex];
    duration = songs[currentSong].Music[musicIndex+1];
    
    // 检查是否播放结束
    if(note == 0xFF)
    {
        StopMusic();
        state = STATE_FINISHED;
        needRefresh = 1;
        return;
    }
    
    // 设置当前音符
    currentNote = note;
    TR0 = 1;  // 开启定时器0
    
    // 检查是否需要切换歌词
    if(currentLyric < songs[currentSong].LyricLen - 1)
    {
        if(musicIndex == songs[currentSong].LyricSwitch[currentLyric + 1])
        {
            currentLyric++;
            needRefresh = 1;
        }
    }
    
    // 设置音符时长，启动计时
    noteDuration = duration * TEMPO;
    beatCount = noteDuration;
    notePlaying = 1;
    
    // 移动到下一个音符
    musicIndex += 2;
}

/* 停止音乐播放 */
void StopMusic(void)
{
    TR0 = 0;
    TR1 = 0;
    BEEP = 0;
    currentNote = V0;
    musicIndex = 0;
    currentLyric = 0;
    beatCount = 0;
    notePlaying = 0;
    noteDuration = 0;
}

void main(void)
{
    unsigned char key;
    
    // 初始化所有模块
    init_lcd();
    Timer0Init();
    Timer1Init();
    
    BEEP = 0;
    ShowSelectScreen();
    
    while(1)
    {
        /* 按键处理 - 最高优先级，每次循环都检测 */
        key = Key();
        if(key != 0)
        {
            switch(state)
            {
                case STATE_SELECT:
                    if(key == 1) // 上一首
                    {
                        currentSong = (currentSong - 1 + SONG_COUNT) % SONG_COUNT;
                        needRefresh = 1;
                    }
                    else if(key == 3) // 下一首
                    {
                        currentSong = (currentSong + 1) % SONG_COUNT;
                        needRefresh = 1;
                    }
                    else if(key == 2) // 开始播放
                    {
                        state = STATE_PLAYING;
                        musicIndex = 0;
                        currentLyric = 0;
                        TR1 = 1;
                        notePlaying = 0;  // 先不播放，等主循环处理
                        needRefresh = 1;
                    }
                    break;
                    
                case STATE_PLAYING:
                case STATE_PAUSED:
                    if(key == 1) // 上一首
                    {
                        StopMusic();
                        currentSong = (currentSong - 1 + SONG_COUNT) % SONG_COUNT;
                        state = STATE_SELECT;
                        needRefresh = 1;
                    }
                    else if(key == 3) // 下一首
                    {
                        StopMusic();
                        currentSong = (currentSong + 1) % SONG_COUNT;
                        state = STATE_SELECT;
                        needRefresh = 1;
                    }
                    else if(key == 2) // 播放/暂停切换
                    {
                        if(state == STATE_PLAYING)
                        {
                            state = STATE_PAUSED;
                            TR0 = 0;  // 暂停时关闭蜂鸣器
                            TR1 = 0;
                            BEEP = 0;
                        }
                        else
                        {
                            state = STATE_PLAYING;
                            TR0 = 1;
                            TR1 = 1;
                        }
                        needRefresh = 1;
                    }
                    break;
                    
                case STATE_FINISHED:
                    if(key == 1) // 上一首
                    {
                        currentSong = (currentSong - 1 + SONG_COUNT) % SONG_COUNT;
                        state = STATE_SELECT;
                        needRefresh = 1;
                    }
                    else if(key == 3) // 下一首
                    {
                        currentSong = (currentSong + 1) % SONG_COUNT;
                        state = STATE_SELECT;
                        needRefresh = 1;
                    }
                    else if(key == 2) // 重新播放
                    {
                        state = STATE_PLAYING;
                        musicIndex = 0;
                        currentLyric = 0;
                        TR1 = 1;
                        notePlaying = 0;
                        needRefresh = 1;
                    }
                    break;
            }
        }
        
        /* LCD刷新 - 只在需要时刷新 */
        if(needRefresh)
        {
            switch(state)
            {
                case STATE_SELECT: ShowSelectScreen(); break;
                case STATE_PLAYING:
                case STATE_PAUSED: ShowPlayScreen(); break;
                case STATE_FINISHED: ShowFinishScreen(); break;
            }
        }
        
        /* 播放逻辑 */
        if(state == STATE_PLAYING)
        {
            if(!notePlaying)
            {
                // 当前没有音符在播放，播放下一个
                PlayNextNote();
            }
            else if(beatCount == 0)
            {
                // 当前音符播放完毕
                notePlaying = 0;
                TR0 = 0;  // 关闭蜂鸣器
                BEEP = 0;
                
                // 音符之间短暂停顿（5ms）
                Delay(5);
            }
        }
    }
}
