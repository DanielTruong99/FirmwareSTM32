#include "state_estimator.h"

/*Private Interfacce*/
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim7;
static int16_t pendlm_counter = 0;
static int16_t motor_counter = 0;
static StateEstimator *state_estimator;
static TimeEvent time_event;


static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM7_Init(void);

void TimeEvent_ctor(TimeEvent * const self, Event event)
{
  self->_super = event;
  self->_pendlm_counter = 0;
  self->_motor_counter = 0;
}

void StateEstimator_ctor(StateEstimator *const self)
{
  Event const timeout_evt = {._signal = TIMEOUT_2KHz_SIG};
  self->initial = StateEstimator_initial;
  self->wait = StateEstimator_wait;
  Ao_ctor(&self->_super, (StateHandler)self->initial);\
  TimeEvent_ctor(&time_event, timeout_evt);
  state_estimator = self;
  self->_motor_topic._resol = (float)(2.0F * PI) / (4.0F * 500.0F);
  self->_pendlm_topic._resol = (float)(2.0F * PI) / (4.0F * 1000.0F);
}

/**
  * @brief  Initial state of the State Estimator
  * @param  self pointer to the StateEstimator
  * @param  event event object cotaining type of event and data 
  * @retval StateHandler status at the end of the initial state
  */
Status StateEstimator_wait(StateEstimator *const self, Event *const event)
{
  Status status;
  
  switch (event->_signal)
  {
    case ENTRY_SIG:
    {
      status = HANDLED_STATUS;
      break;
    }
    
    case EXIT_SIG:
    {
      status = HANDLED_STATUS;
      break;
    }

    case TIMEOUT_2KHz_SIG:
    {
      float raw_motor_angle;
      float raw_pendlm_angle;
      
      self->_motor_topic._counter = ((TimeEvent *)event)->_motor_counter;
      self->_pendlm_topic._counter = ((TimeEvent *)event)->_pendlm_counter;
      
      raw_motor_angle = self->_motor_topic._counter * self->_motor_topic._resol;
      raw_pendlm_angle = self->_pendlm_topic._counter * self->_pendlm_topic._resol;

      self->_states_topic._motor._angle = raw_motor_angle;
      self->_states_topic._pendlm._angle = raw_pendlm_angle;


      status = HANDLED_STATUS;
      break;
    } 

    default:
    {
      status = IGNORED_STATUS;
      break;
    }   
  }

  return status;
}

/**
  * @brief  Initial state of the State Estimator
  * @param  self pointer to the StateEstimator
  * @param  event event object cotaining type of event and data 
  * @retval StateHandler status at the end of the initial state
  */
Status StateEstimator_initial(StateEstimator *const self, Event *const event)
{
  Status status;
  self->_super._super->setHandler(self->_super._super, (StateHandler)self->wait);
  status = TRAN_STATUS;
  return status;
}

/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM7)
  {
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  
    // time_event._motor_counter = motor_counter;
    // time_event._pendlm_counter = pendlm_counter;
    // state_estimator->_super.postFromISR(&state_estimator->_super, &time_event, &xHigherPriorityTaskWoken);
  }
}

/**
  * @brief  Input Capture callback in non-blocking mode
  * @param  htim TIM IC handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM4)
  {
    // Read motor encoder counter value 
    motor_counter = (int16_t)((uint32_t)__HAL_TIM_GET_COUNTER(htim));
  }
  else if(htim->Instance == TIM3)
  {
    // Read pendullum encoder counter value 
    pendlm_counter = (int16_t)((uint32_t)__HAL_TIM_GET_COUNTER(htim));
  }
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim3);
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim4);
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim7);
}

/**
  * @brief This function initialize USART1.
  */
void Driver_StateEstimator_Init()
{
  MX_TIM3_Init(); // Motor encoder
  MX_TIM4_Init(); // Pendullum encoder
  MX_TIM7_Init(); // Basic timer for generating periodic task running at 2kHz

  HAL_TIM_Base_Start_IT(&htim7);
  HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start_IT(&htim4, TIM_CHANNEL_ALL);
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{
  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    // Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    // Error_Handler();
  }
}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{
  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    // Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    // Error_Handler();
  }
}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 84;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 500;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    // Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    // Error_Handler();
  }
}

/**
* @brief TIM_Encoder MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_encoder: TIM_Encoder handle pointer
* @retval None
*/
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_encoder)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim_encoder->Instance==TIM3)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**TIM3 GPIO Configuration
    PA6     ------> TIM3_CH1
    PA7     ------> TIM3_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }
  else if(htim_encoder->Instance==TIM4)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /**TIM4 GPIO Configuration
    PD12     ------> TIM4_CH1
    PD13     ------> TIM4_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* TIM4 interrupt Init */
    HAL_NVIC_SetPriority(TIM4_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  }

}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM7)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM7_CLK_ENABLE();

    /* TIM7 interrupt Init */
    HAL_NVIC_SetPriority(TIM7_IRQn, 8, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
  }
}