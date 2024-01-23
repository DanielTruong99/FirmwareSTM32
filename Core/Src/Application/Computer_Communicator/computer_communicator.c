#include "computer_communicator.h"

extern UART_HandleTypeDef huart3;
extern struct StateEstimator * state_estimator;
static char tx_data[15];
static char rx_data[15];
struct Ao * ao_computer_communicator;
struct ComputerCommunicator * computer_communicator;

#define decimal(x) (int16_t)((x - (int16_t)x) * 100)
#define integer(x) (int16_t)x

static Status initial(struct ComputerCommunicator *const self, Event const * const event)
{
  Status status;
  status = TRAN_STATUS;
  self->super.handler = (StateHandler)self->wait;
  return status;
}

static Status sending(struct ComputerCommunicator *const self, Event const * const event)
{
    Status status;
    
    switch (event->signal)
    {
        case ENTRY_SIG:
        {
            status = HANDLED_STATUS;
            break;
        }

        case SENSOR_SENDED_SIG:
        {
            self->super.handler = (StateHandler)self->wait;
            status = TRAN_STATUS;

            break;
        }


        case EXIT_SIG:
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

static Status wait(struct ComputerCommunicator *const self, Event const * const event)
{

    Status status;
    
    switch (event->signal)
    {
        case ENTRY_SIG:
        {
            status = HANDLED_STATUS;
            break;
        }

        case STATE_UPDATED_SIG:
        {
            static State state_topic = {0};
            BaseType_t is_success;
            is_success = xQueuePeek(state_estimator->state_pub, &state_topic, 0);

            if(is_success)
            {
                // sprintf(tx_data , "S%.3f %.3f\n", state_topic.pendlm.angle, state_topic.motor.angle);
                sprintf(tx_data , ">v:%.3f\n>a:%.3f\n", state_topic.pendlm.vel, state_topic.pendlm.angle);
                HAL_UART_Transmit_IT(&huart3, (uint8_t *)tx_data, (unsigned)strlen(tx_data));

                self->super.handler = (StateHandler)self->sending;
                status = TRAN_STATUS;
            }
            else
            {
                status = HANDLED_STATUS;
            }
        
            break;
        }

        case EXIT_SIG:
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

static void public(QueueHandle_t xQueue, const void * pvItemToQueue)
{
  xQueueOverwrite(xQueue, pvItemToQueue);
}

static void publicFromISR(QueueHandle_t xQueue, const void * pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken)
{
  xQueueOverwriteFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken);
}

static void new(struct ComputerCommunicator * const self)
{
  /*Specify function pointer*/
  self->initial=&initial; self->wait=&wait; self->sending=&sending;
  self->public=&public; self->publicFromISR = &publicFromISR;

  /*Initialize members*/
  Ao_new(&self->super, (StateHandler)&initial);

  /*Cache Ao for using in Encoder driver*/
  ao_computer_communicator = &self->super;
  computer_communicator = self;

  /*Initialize Queue for Mailbox as subsribers, publishers*/
  self->state_sub = state_estimator->state_pub;
}
const struct ComputerCommunicatorClass ComputerCommunicator={.new=&new};

