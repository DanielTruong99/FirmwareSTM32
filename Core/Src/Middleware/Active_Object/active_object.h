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
typedef Status (* StateHandler)(struct Ao * const self, Event const * const event);
enum Status { TRAN_STATUS, HANDLED_STATUS, IGNORED_STATUS, INIT_STATUS };

struct Ao
{
    // Members ---------------------------------
    StateHandler handler;
    TaskHandle_t thread;
    QueueHandle_t queue;

    // Methods --------------------------------
    void (*init)(struct Ao * const self, Event * const event);
    void (*dispatch)(struct Ao * const self, Event * const event);
    void (*start)(  
            struct Ao * const self, 
            uint8_t priority, 
            uint32_t stack_size,
            uint32_t queue_len);
    void (*post)(struct Ao * const self, Event * const event);
    void (*postFromISR)(struct Ao * const self, Event * const event, BaseType_t *pxHigherPriorityTaskWoken);
    void (*loopEvent)(void * pData);
};

extern const struct AoClass 
{
    struct Ao (*new)(StateHandler handler, TaskHandle_t thread, QueueHandle_t queue);
} Ao;

#endif /* _ACTIVE_OBJECT_H */