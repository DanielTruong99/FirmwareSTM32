#include "main.h"
#include "FreeRTOS.h"
#include "task.h"


int main(void)
{
  // System initialization
  HAL_Init();
  SystemClock_Config();
  
  // Periferal initialization
  MX_GPIO_Init();
  Driver_MotorCommunicator_Init();
  Driver_StateEstimator_Init();
  App_ComputerCommunicator_Init();

  // Task initialization----------------------------------------------------------------
  // State Estimator Task



  vTaskStartScheduler();
  while(1){}
}

