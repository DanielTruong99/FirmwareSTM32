#include "motor_communicator.h"

extern UART_HandleTypeDef huart2;
static char tx_data[15];
struct Ao * ao_motor_communicator;
struct MotorCommunicator * motor_communicator;


static Status initial(struct MotorCommunicator *const self, Event const * const event)
{
  Status status;
  status = TRAN_STATUS;
  self->super.handler = (StateHandler)self->wait;
  return status;
}

static Status sending(struct MotorCommunicator *const self, Event const * const event)
{
    Status status;
    
    switch (event->signal)
    {
        case ENTRY_SIG:
        {
            status = HANDLED_STATUS;
            break;
        }

        case COMMAND_SENDED_SIG:
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

static Status wait(struct MotorCommunicator *const self, Event const * const event)
{

    Status status;
    
    switch (event->signal)
    {
        case ENTRY_SIG:
        {
            status = HANDLED_STATUS;
            break;
        }

        case PWC_TRIGGER_SIG:
        {
            static PWC pwc_topic = {0};
            BaseType_t is_success;
            is_success = xQueuePeek(self->pwc_sub, &pwc_topic, 0);

            if(is_success)
            {
                sprintf(tx_data, "N1 O d%d\n", pwc_topic.d);
                HAL_UART_Transmit_IT(&huart2, (uint8_t *)tx_data, (unsigned)strlen(tx_data));

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

static void new(struct MotorCommunicator * const self)
{
  /*Specify function pointer*/
  self->initial=&initial; self->wait=&wait; self->sending=&sending;
  self->public=&public; self->publicFromISR = &publicFromISR;

  /*Initialize members*/
  Ao_new(&self->super, (StateHandler)&initial);

  /*Cache Ao for using in Encoder driver*/
  ao_motor_communicator = &self->super;
  motor_communicator = self;

  /*Initialize Queue for Mailbox as subsribers, publishers*/
  self->pwc_sub = xQueueCreate( 1, sizeof( PWC ) );
}
const struct MotorCommunicatorClass MotorCommunicator={.new=&new};