# ml_libs/ — 硬件驱动抽象层

本目录是对 MSPM0G3507 外设的**逐层封装库**。每个模块把 TI DriverLib 的底层寄存器操作封装成简洁的函数接口。

## 设计思想

TI 官方 DriverLib 函数参数多、调用冗长。这组库用**结构体 + 枚举**简化接口，例如：

```c
// TI 原生写法（几十行）
DL_GPIO_initDigitalOutput(GPIO_GRP_0_DOUT31_0, ...);
DL_GPIO_setPins(GPIO_PORT_A, GPIO_PIN_0);

// ml_libs 封装后
gpio_init(PA0, OUT);
gpio_set(PA0, 1);
```

## 文件说明

| 文件 | 封装的外设 | 核心函数 |
|------|-----------|----------|
| `ml_gpio.c/.h` | GPIO 输入/输出 | `gpio_init()`, `gpio_set()`, `gpio_get()`, `gpio_toggle()` |
| `ml_pwm.c/.h` | PWM 输出 | `pwm_init()`, `pwm_update()` （占空比 0~10000） |
| `ml_uart.c/.h` | UART 串口（0~3） | 中断接收 + `printf` 重定向到 UART0 |
| `ml_tim.c/.h` | 通用定时器中断 | `tim_interrupt_ms_init()` 配置毫秒级定时中断 |
| `ml_exti.c/.h` | 外部中断 | `exti_init()` 上升沿/下降沿触发 |
| `ml_i2c.c/.h` | 软件 I2C（位操作） | `i2c_start()`, `i2c_write()`, `i2c_read()` ... 跑在 PB6/PB7 |
| `ml_delay.c/.h` | SysTick 延时 | `delay_ms()`, `delay_us()` （80MHz 主频） |
| `ml_system.c/.h` | 系统时钟 | 配置 80MHz（SYSPLL + HFXT） |
| `ml_motor.c/.h` | 电机驱动 | `Set_left_pwm()`, `Set_right_pwm()` （正反转 + 刹车） |
| `ml_mpu6050.c/.h` | MPU6050 六轴传感器 | ⚠️ **写了但实际未使用**——实际用串口 IMU |
| `ml_oled.c/.h` | OLED 128×64 SSD1306 | `oled_show_string()`, `oled_show_num()`, `oled_show_float()` |
| `ml_oled_font.h` | OLED 8×16 ASCII 字库 | 121KB 字体数据 |
| `headfile.h` | **总头文件**：include 所有上述文件 | 用户代码只需 `#include "headfile.h"` |

## 学习价值

这套封装是嵌入式开发中常见的 **HAL（硬件抽象层）** 思想——把底层硬件细节藏起来，让上层业务代码（如巡线逻辑）不需要关心寄存器。如果你以后做 STM32、GD32 等 MCU 开发，这个模式完全通用。

## 注意事项

- `ml_mpu6050.c` 代码完整但**项目实际未使用**（改用了串口 IMU），保留供学习 I2C 驱动写法
- 软件 I2C 比硬件 I2C 慢，但优点是**不占用硬件 I2C 外设**，引脚可灵活配置
