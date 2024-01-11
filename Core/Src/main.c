#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static StateEstimator state_estimator; 

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
  StateEstimator_ctor(&state_estimator);
  state_estimator._super.start(
    &state_estimator._super,
                          3,
                        200,
                         5
  );


  vTaskStartScheduler();
  while(1){}
}

