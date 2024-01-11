#include "active_object.h"
static Event entry_evt = {.signal = ENTRY_SIG};
static Event exit_evt = {.signal = EXIT_SIG};

static void init(struct Ao * const self, Event * const event)
{
    (*self->handler)(self, event);
    (*self->handler)(self, &entry_evt);
}

static void dispatch(struct Ao * const self, Event * const event)
{
    Status status;
    StateHandler prev_handler = self->handler;

    status = (*self->handler)(self, event);
    if(status == TRAN_STATUS)
    {
        (*prev_handler)(self, &exit_evt);
        (*self->handler)(self, &entry_evt);
    }
}

static void start(  
    struct Ao * const self, 
    uint8_t priority, 
    uint32_t stack_size,
    uint32_t queue_len)
{
    self->queue = xQueueCreate(queue_len, sizeof(void **));
    xTaskCreate(&self->loopEvent, "Active Object", stack_size, (void *)self, priority, &self->thread);
}

static void post(struct Ao * const self, Event * const event)
{
    xQueueOverwrite(self->queue, (void *)event);
}

static void postFromISR(struct Ao * const self, Event * const event, BaseType_t *pxHigherPriorityTaskWoken)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueOverwriteFromISR(self->queue, (void *)event, &xHigherPriorityTaskWoken); 
}

static void loopEvent(void * pData)
{
    struct Ao * ao_instance = (struct Ao *)pData;
    Event * event;
    ao_instance->init(ao_instance, &entry_evt);

    while(1)
    {
        xQueueReceive(ao_instance->queue, (void *)event, portMAX_DELAY);
        ao_instance->dispatch(ao_instance, (Event *)event);
    }
}

static struct Ao new(StateHandler handler)
{
    return (struct Ao){
        .handler = &handler, 
        .init = &init,
        .dispatch = &dispatch,
        .post = &post,
        .postFromISR = &postFromISR,
        .loopEvent = &loopEvent,
        .start = &start,
    };
}
extern const struct AoClass Ao={.new=&new};