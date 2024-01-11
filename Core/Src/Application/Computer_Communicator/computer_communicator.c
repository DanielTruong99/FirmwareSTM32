#include "computer_communicator.h"

/*Private Interfacce*/
static UART_HandleTypeDef huart3;

static void UART_Init(UART_HandleTypeDef* huart);


/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart3);
}

/**
  * @brief This function will be called if number of 
  * recived bytes equal data Size declared in HAL_UART_Receive_IT.
  */
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//   // sprintf(tx_data, "Hello World Hi\n");
//   // HAL_UART_Transmit_IT(&huart3, tx_data, (unsigned)strlen(tx_data));
//   // HAL_UART_Receive_IT(&huart3, rx_data, 3);
// }

/**
  * @brief This function initialize USART1.
  */
void App_ComputerCommunicator_Init()
{
  UART_Init(&huart3);
  // HAL_UART_Receive_IT(&huart3, rx_data, 3);
}

static void UART_Init(UART_HandleTypeDef* huart)
{
  huart->Instance = USART3;
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