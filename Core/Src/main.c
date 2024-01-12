#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static struct StateEstimator state_estimator;
static struct Ao* state_estimator_ao = &state_estimator.super; 

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
  StateEstimator.new(&state_estimator);
  state_estimator.super.start(state_estimator_ao,
                              1,
                              100,
                              configMINIMAL_STACK_SIZE,
                              0U);
    



  vTaskStartScheduler();
  while(1){}
}

