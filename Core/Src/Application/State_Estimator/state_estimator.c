#include "state_estimator.h"

/*Private Interfacce*/
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim7;
extern struct ComputerCommunicator * computer_communicator;
struct Ao * ao_estimator;
struct StateEstimator * state_estimator;


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
      static Encoder encoder_topic = {0};
      static State state_topic = {.motor ={0.0F}, .pendlm = {0.0F}, .cart = {0.0F};
      BaseType_t is_success;
      is_success = xQueuePeek(self->encoder_sub, &encoder_topic, 0);

      if (is_success)
      {
        /*Input: encoder_topic; Ouput: state_topic*/
        self->data_processor->processNewData(self->data_processor, &encoder_topic, &state_topic);
      }

      /*Public into state topic*/
      self->public(self->state_pub, &state_topic);

      /*Post STATE_UPDATED_SIG event into computer_communicator queue*/
      static const Event state_evt = {STATE_UPDATED_SIG};
      ao_estimator->post(&computer_communicator->super, &state_evt);
      
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

void public(QueueHandle_t xQueue, const void * pvItemToQueue)
{
  xQueueOverwrite(xQueue, pvItemToQueue);
}

void publicFromISR(QueueHandle_t xQueue, const void * pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken)
{
  xQueueOverwriteFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken);
}

static void new(struct StateEstimator * const self)
{
  /*Specify function pointer*/
  self->initial=&initial; self->wait=&wait;
  self->public=&public; self->publicFromISR = &publicFromISR;

  /*Initialize members*/
  Ao_new(&self->super, (StateHandler)&initial);
  self->data_processor = ( struct Dsp * ) pvPortMalloc( sizeof( struct Dsp ) );
  Dsp_new(self->data_processor);

  /*Cache Ao for using in Encoder driver*/
  ao_estimator = &self->super;
  state_estimator = self;

  /*Initialize Queue for Mailbox as subsribers, publishers*/
  self->encoder_sub = xQueueCreate( 1, sizeof( Encoder ) );
  self->state_pub = xQueueCreate( 1, sizeof( State ) );
}
const struct StateEstimatorClass StateEstimator={.new=&new};

