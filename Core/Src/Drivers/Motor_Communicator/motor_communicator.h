#ifndef _MOTOR_COMMUNICATOR_H
#define _MOTOR_COMMUNICATOR_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

/*Public Interfacce*/
void Driver_MotorCommunicator_Init();
void HAL_UART_MspInit(UART_HandleTypeDef* huart);


#endif /*_MOTOR_COMMUNICATOR_H */