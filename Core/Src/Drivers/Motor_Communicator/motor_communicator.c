#include "motor_communicator.h"

/*Private Interfacce*/
static UART_HandleTypeDef huart2;
// static char rx_data[9];
// static char tx_data[20];
static void UART_Init(UART_HandleTypeDef* huart);


/**
  * @brief This function handles USART1 global interrupt.
  */
void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
}

/**
  * @brief This function will be called if number of 
  * recived bytes equal data Size declared in HAL_UART_Receive_IT.
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // sprintf(tx_data, "Hello World Hi\n");
  // HAL_UART_Transmit_IT(&huart2, tx_data, (unsigned)strlen(tx_data));
  // HAL_UART_Receive_IT(&huart2, rx_data, 3);
}

/**
  * @brief This function initialize USART1.
  */
void Driver_MotorCommunicator_Init()
{
  UART_Init(&huart2);
  // HAL_UART_Receive_IT(&huart2, rx_data, 3);
}


void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART2)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 9, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
  else if(huart->Instance==USART3)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  }
}

static void UART_Init(UART_HandleTypeDef* huart)
{
  huart->Instance = USART2;
  huart->Init.BaudRate = 115200;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(huart) != HAL_OK)
  {
    // Error_Handler();
  }
}