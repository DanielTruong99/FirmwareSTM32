#ifndef _ACTIVE_OBJECT_H
#define _ACTIVE_OBJECT_H

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"

typedef struct Event Event;
struct Event
{
    uint16_t signal;
};

enum ReservedSignal{ INIT_SIG, ENTRY_SIG, EXIT_SIG, USER_SIG };
typedef enum Status Status;

enum Status { TRAN_STATUS, HANDLED_STATUS, IGNORED_STATUS, INIT_STATUS };
typedef struct Ao Ao;
typedef Status (* StateHandler)(struct Ao * const self, Event const * const event);
struct Ao
{
    // Members ---------------------------------
    StateHandler handler;
    TaskHandle_t thread;    
    StaticTask_t thread_cb;  /* thread control-block (FreeRTOS static alloc) */
    QueueHandle_t queue;
    StaticQueue_t queue_cb; /* queue control-block (FreeRTOS static alloc) */

    // Methods --------------------------------
    void (*init)(struct Ao * const self, Event const * const event);
    void (*dispatch)(struct Ao * const self, Event const * const event);
    void (*start)(struct Ao * const self,
                  uint8_t prio,       /* priority (1-based) */
                  uint32_t queueLen,
                  uint32_t stackSize,
                  uint16_t opt);
    void (*post)(struct Ao * const self, Event const * const event);
    void (*postFromISR)(struct Ao * const self, Event const * const event, BaseType_t *pxHigherPriorityTaskWoken);
    void (*loopEvent)(void * pData);
};
void Ao_new(struct Ao * const self, StateHandler handler);


#endif /* _ACTIVE_OBJECT_H */