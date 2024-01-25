#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static struct StateEstimator state_estimator;
static struct Ao* state_estimator_ao = &state_estimator.super; 

static struct MotorCommunicator motor_communicator;
static struct Ao* motor_communicator_ao = &motor_communicator.super; 

static struct ComputerCommunicator computer_communicator;
static struct Ao* computer_communicator_ao = &computer_communicator.super; 

int main(void)
{
  // System initialization

  HAL_Init();
  SystemClock_Config();
  
  // Periferal initialization
  MX_GPIO_Init();
  Driver_USART_Init();
  Driver_Encoder_Init();

  // Task initialization----------------------------------------------------------------
  // State Estimator Task
  StateEstimator.new(&state_estimator);
  state_estimator.super.start(state_estimator_ao,
                              2,
                              20,
                              3000,
                              0U);

  // Motor Communicator Task
  MotorCommunicator.new(&motor_communicator);
  motor_communicator.super.start(motor_communicator_ao,
                              3,
                              15,
                              1000,
                              0U);        

  // Computer Communicator Task
  ComputerCommunicator.new(&computer_communicator);
  computer_communicator.super.start(computer_communicator_ao,
                              1,
                              15,
                              2000,
                              0U);                   
    



  vTaskStartScheduler();
  while(1){}
}

