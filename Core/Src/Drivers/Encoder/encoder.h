#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "state_estimator.h"

/*Public Interfacce*/


void Driver_Encoder_Init();
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_encoder);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);

#endif /* _ENCODER_H */