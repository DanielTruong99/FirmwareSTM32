#include "active_object.h"

void Fsm_init(Fsm * const self, Event * const event);
void Fsm_dispatch(Fsm * const self, Event * const event);
void Fsm_setHandler(Fsm * const self, StateHandler handler);
void Ao_start(
    Ao * const self, 
    uint8_t priority, 
    uint32_t stack_size,
    uint32_t queue_len);
void Ao_post(Ao * const self, Event * const event);
void Ao_postFromISR(Ao * const self, Event * const event, BaseType_t *pxHigherPriorityTaskWoken);
void Ao_loopEvent(void * pData);

static Event const entry_evt = { ENTRY_SIG };
static Event const exit_evt  = { EXIT_SIG };

// Finite State Machine implementation --------------------------------
void Fsm_init(Fsm * const self, Event * const event)
{
    self->_handler(self, event);
    self->_handler(self, &entry_evt);
    
}

void Fsm_dispatch(Fsm * const self, Event * const event)
{
    Status status;
    StateHandler prev_handler = self->_handler;

    status = self->_handler(self, event);

    if(status == TRAN_STATUS)
    {
        prev_handler(self, &exit_evt);
        self->_handler(self, &entry_evt);
    }
}

void Fsm_setHandler(Fsm * const self, StateHandler handler)
{
    self->_handler = handler;
}

void Fsm_ctor(Fsm * const self, StateHandler handler)
{
    self->_handler = handler;
    self->init = Fsm_init;
    self->dispatch = Fsm_dispatch;
    self->setHandler = Fsm_setHandler;
}

// Active Object implementation --------------------------------
void Ao_start(
    Ao * const self, 
    uint8_t priority, 
    uint32_t stack_size,
    uint32_t queue_len)
{
    self->_queue = xQueueCreate(queue_len, sizeof(char *));
    xTaskCreate(
        self->loopEvent, 
        "Event Loop", 
        stack_size, 
        (void *)self, 
        tskIDLE_PRIORITY + priority, 
        self->_thread);
}

void Ao_post(Ao * const self, Event * const event)
{
    xQueueOverwrite(self->_queue, (char *)event);
}

void Ao_postFromISR(Ao * const self, Event * const event, BaseType_t *pxHigherPriorityTaskWoken)
{
    xQueueOverwriteFromISR(self->_queue, (char *)event, pxHigherPriorityTaskWoken);
}

void Ao_loopEvent(void * pData)
{
    Ao * ao_instance = (Ao *)pData;
    Event * event;
    /* initialize the AO */
    ao_instance->_super->init(ao_instance->_super, (Event *)0);

    /* event loop ("message pump") */
    while (1) {
    
        /* wait for any event and receive it into object 'e' */
        xQueueReceive(ao_instance->_queue, event, portMAX_DELAY); /* BLOCKING! */

        /* dispatch event to the active object 'ao_instance' */
        ao_instance->_super->dispatch(ao_instance->_super, event);
    }
}

void Ao_ctor(Ao * const self, StateHandler handler)
{
    Fsm_ctor(self->_super, handler);
    self->start = Ao_start;
    self->post = Ao_post;
    self->postFromISR = Ao_postFromISR;
    self->loopEvent = Ao_loopEvent;
}