#ifndef __MUSIC_H__
#define __MUSIC_H__

#include <reg52.h>

// 音符与索引对应表
#define V0	0
#define L1	1
#define L1_	2
#define L2	3
#define L2_	4
#define L3	5
#define L4	6
#define L4_	7
#define L5	8
#define L5_	9
#define L6	10
#define L6_	11
#define L7	12
#define M1	13
#define M1_	14
#define M2	15
#define M2_	16
#define M3	17
#define M4	18
#define M4_	19
#define M5	20
#define M5_	21
#define M6	22
#define M6_	23
#define M7	24
#define H1	25
#define H1_	26
#define H2	27
#define H2_	28
#define H3	29
#define H4	30
#define H4_	31
#define H5	32
#define H5_	33
#define H6	34
#define H6_	35
#define H7	36

// 播放速度，值为四分音符的时长(ms)
#define SPEED	700

// 对外声明全局数组
extern unsigned char code *MusicList[];      // 歌曲列表数组
extern unsigned char code SongNames[][10];   // 英文歌曲名数组

// 全局音乐变量
extern unsigned char CurrentSong;    // 当前选中铃声
extern unsigned char MusicSelect;    // 音乐播放指针
extern bit IsPlaying;                // 音乐播放标志

// 函数声明
void Music_Init(void);               // 音乐系统初始化（双定时器）
void Music_Play(unsigned char song); // 开始播放指定歌曲
void Music_Stop(void);               // 停止播放音乐

#endif