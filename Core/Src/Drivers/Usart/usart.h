#ifndef _USART_H
#define _USART_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include "motor_communicator.h"
#include "active_object.h"
#include "computer_communicator.h"

/*Public Interfacce*/
void Driver_USART_Init();
void HAL_UART_MspInit(UART_HandleTypeDef* huart);

#endif /*_USART_H */