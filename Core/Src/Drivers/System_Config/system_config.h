#ifndef _SYSTEM_CONFIG_H
#define _SYSTEM_CONFIG_H

#include "stm32f4xx_hal.h"

/*Public interface*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void Error_Handler(void);


#endif /* _SYSTEM_CONFIG_H */