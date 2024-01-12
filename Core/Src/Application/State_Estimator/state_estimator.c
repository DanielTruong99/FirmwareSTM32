#include "state_estimator.h"

/*Private Interfacce*/
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim7;
struct Ao * ao_estimator;


Status initial(struct StateEstimator *const self, Event const * const event)
{
  Status status;
  status = TRAN_STATUS;
  self->super.handler = (StateHandler)self->wait;
  return status;
}

Status wait(struct StateEstimator *const self, Event const * const event)
{
  Status status;
  
  switch (event->signal)
  {
    case ENTRY_SIG:
    {
      HAL_TIM_Base_Start_IT(&htim7);
      HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL);
      HAL_TIM_Encoder_Start_IT(&htim4, TIM_CHANNEL_ALL);

      status = HANDLED_STATUS;
      break;
    }

    case TIMEOUT_2KHz_SIG:
    {
      

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

static void new(struct StateEstimator * const self)
{
  self->initial=&initial; self->wait=&wait;
  Ao_new(&self->super, (StateHandler)&initial);
  ao_estimator = &self->super;
}
const struct StateEstimatorClass StateEstimator={.new=&new};

