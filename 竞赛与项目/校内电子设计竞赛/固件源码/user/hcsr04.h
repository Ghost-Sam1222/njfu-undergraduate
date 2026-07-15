#ifndef _HCSR04_H_
#define _HCSR04_H_

#include "ti_msp_dl_config.h"  // TI?????(??SysConfig?ti_msp_dl_config.h)
#include "ml_delay.h"
#include "ml_gpio.h"
// ==================== ????:?????? ====================
#define TRIG_PORT    GPIOB
#define TRIG_PIN     DL_GPIO_PIN_9
#define ECHO_PORT    GPIOA
#define ECHO_PIN     DL_GPIO_PIN_10

void HCSR04_Init(void);
float HCSR04_GetDistance(void);

#endif