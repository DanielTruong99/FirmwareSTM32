#ifndef _ACTIVE_OBJECT_H
#define _ACTIVE_OBJECT_H

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"

typedef struct Event Event;
struct Event
{
    uint16_t _signal;
};

enum ReservedSignal{ INIT_SIG, ENTRY_SIG, EXIT_SIG, USER_SIG };
typedef enum Status Status;
typedef struct Fsm Fsm;
typedef Status (* StateHandler)(Fsm * const self, Event const * const event);

enum Status { TRAN_STATUS, HANDLED_STATUS, IGNORED_STATUS, INIT_STATUS };
struct Fsm
{
    // Members --------------------------------
    StateHandler _handler;

    // Methods --------------------------------
    void (*init)(Fsm * const self, Event * const event);
    void (*dispatch)(Fsm * const self, Event * const event);
    void (*setHandler)(Fsm * const self, StateHandler handler);
};
// Finite State Machine constructor
void Fsm_ctor(Fsm * const self, StateHandler handler);


//----------------------------------------------------------------------------------------

typedef struct Ao Ao;
struct Ao
{
    // Members ---------------------------------
    Fsm * _super;
    TaskHandle_t * _thread;
    QueueHandle_t _queue;

    // Methods --------------------------------
    void (*start)(  
            Ao * const self, 
            uint8_t priority, 
            uint32_t stack_size,
            uint32_t queue_len);

    void (*post)(Ao * const self, Event * const event);
    void (*postFromISR)(Ao * const self, Event * const event, BaseType_t *pxHigherPriorityTaskWoken);
    void (*loopEvent)(void * pData);
};
// Active Object constructor
void Ao_ctor(Ao * const self, StateHandler handler);

#endif /* _ACTIVE_OBJECT_H */