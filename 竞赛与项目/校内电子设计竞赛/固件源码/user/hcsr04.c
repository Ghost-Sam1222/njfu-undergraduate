#include "hcsr04.h"
#include "ml_delay.h"
/**
 * @brief  ??????(??:?? cm)
 * @retval ??????
 */
/*void delay_us(uint32_t us)
{
    uint32_t i;
    for(i=0; i<us*4; i++)
      {
        __NOP();
    }
}
*/

void HCSR04_Init(void)
{
    // ??GPIO??

    //==================== TRIG = PB9 ?? ====================
    DL_GPIO_initDigitalOutput(DL_GPIO_PIN_9);
    DL_GPIO_clearPins(TRIG_PORT, TRIG_PIN);
    DL_GPIO_enableOutput(TRIG_PORT, TRIG_PIN);

    //==================== ECHO = PA10 ?? ====================
    DL_GPIO_initDigitalInput(ECHO_PIN);
    
}
float HCSR04_GetDistance(void)
{
    uint32_t time = 0;
    uint32_t timeout = 0;
	const uint32_t max_wait = 6000;
    
      DL_GPIO_setPins(TRIG_PORT, TRIG_PIN);
    delay_us(15);
    DL_GPIO_clearPins(TRIG_PORT, TRIG_PIN);

    // 2. ??ECHO??
	timeout = 0;
    while(DL_GPIO_readPins(ECHO_PORT, ECHO_PIN) == 0 && (timeout < max_wait)){
		timeout++;
	}
	if(timeout >= max_wait){
		return 0.0f;
	}
	time = 0;
	timeout = 0;

    // 3. ??ECHO?????
      time = 0;
    while(DL_GPIO_readPins(ECHO_PORT, ECHO_PIN) != 0 && (timeout < max_wait))
    {
        time++;
        delay_us(1);
		timeout++;
    }

    return time * 0.017f;
}