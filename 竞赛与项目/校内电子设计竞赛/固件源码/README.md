# 固件源码

## 总览

本目录包含 MSPM0G3507 的 **Keil MDK** 工程代码。按功能分为三层：

```
固件源码/
├── user/       ← 应用层：main入口、巡线、PID、传感器业务逻辑
├── ml_libs/    ← 驱动层：GPIO/PWM/UART/I2C/TIM 等硬件抽象库
└── code/       ← 算法层：PID 模块（早期版本，已被 user/_pid.c 替代）
```

## 代码架构

```
main.c (初始化 + 主循环)
  ├── 10ms 定时中断 (TIMG8)
  │   ├── 读编码器
  │   └── 根据 mode/set 调用 track1() ~ track4()
  ├── 串口中断 (UART1)
  │   └── 接收 IMU 角度数据
  ├── 外部中断
  │   └── 编码器脉冲计数
  └── 主循环
      ├── 更新 IMU 角度显示
      ├── 超声波测距（每 25 次）
      └── 更新 OLED 显示
```

## 编译方法

1. 安装 [Keil MDK](https://www.keil.com/download/product/)（ARM Compiler 6）
2. 安装 [TI MSPM0 SDK](https://www.ti.com/tool/MSPM0-SDK) v1.20+
3. 打开 `user/project.uvprojx`
4. 编译，烧录

## 值得注意的设计细节

- **两套 PID 共存**：`code/pid.c` 是早期版本（速度 PID + 低通滤波），`user/_pid.c` 是最终使用的版本（增量式 + 位置式 + 串级控制）。建议学习时对比两版，理解 PID 的演进。
- **MPU6050 驱动写了但没用**：`ml_mpu6050.c` 代码完整，但实际项目用的是串口 IMU（`user/IMU.c`）。这是竞赛中常见的「方案变更」遗迹，值得思考为什么换了方案。
- **K230 通信协议**：UART0 115200bps，帧格式 `[0x75][addr][dataL][dataH][0x04]`，见 `user/serical.c`。
- **「seriacl」是 typo**：`user/seriacl.h` 实际对应 `user/serical.c`，文件名拼写错误但不影响编译。
