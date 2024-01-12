#include "active_object.h"

static void init(struct Ao * const self, Event const * const event)
{
    static Event const entry_evt = {.signal = ENTRY_SIG};
    (*self->handler)(self, event);
    (*self->handler)(self, &entry_evt);
}

static void dispatch(struct Ao * const self, Event const * const event)
{
    Status status;
    StateHandler prev_handler = self->handler;
    static Event const entry_evt = {.signal = ENTRY_SIG};
    static Event const exit_evt = {.signal = EXIT_SIG};

    status = (*self->handler)(self, event);
    if(status == TRAN_STATUS)
    {
        (*prev_handler)(self, &exit_evt);
        (*self->handler)(self, &entry_evt);
    }
}

static void start(struct Ao * const self,
                  uint8_t prio,       /* priority (1-based) */
                  uint32_t queueLen,
                  uint32_t stackSize,
                  uint16_t opt)
{
    uint32_t stk_depth = (stackSize / sizeof(StackType_t));
    uint32_t a[queueLen];
    (void)opt; /* unused parameter */
    
    self->queue = xQueueCreate(queueLen, sizeof(Event *));

    xTaskCreate(self->loopEvent, "AO", stk_depth, self, prio + tskIDLE_PRIORITY, &self->thread);
}

static void post(struct Ao * const self, Event const * const event)
{
    xQueueSendToBack(self->queue, (void *)&event, (TickType_t)0);
}

static void postFromISR(struct Ao * const self, Event const * const event, BaseType_t *pxHigherPriorityTaskWoken)
{
    xQueueSendToBackFromISR(self->queue, (void *)&event, pxHigherPriorityTaskWoken); 
}

static void loopEvent(void * pData)
{
    struct Ao * ao_instance = (struct Ao *)pData;
    static Event const entry_evt = {.signal = ENTRY_SIG};
    ao_instance->init(ao_instance, &entry_evt);

    while(1)
    {
        Event * event;
        xQueueReceive(ao_instance->queue, &event, portMAX_DELAY);
        ao_instance->dispatch(ao_instance, (Event *)event);
    }
}

void Ao_new(struct Ao * const self, StateHandler handler)
{
    self->handler = handler;
    self->init = &init;
    self->dispatch = &dispatch;
    self->post = &post;
    self->postFromISR = &postFromISR;
    self->loopEvent = &loopEvent;
    self->start = &start;
}
