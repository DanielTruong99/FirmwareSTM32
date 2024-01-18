#ifndef _COMPUTER_COMMUNICATOR_H
#define _COMPUTER_COMMUNICATOR_H

#include "stm32f4xx_hal.h"
#include "active_object.h"
#include "topic.h"
#include "state_estimator.h"
#include "printf.h"

enum ComputerCommunicatorEvent{SENSOR_SENDED_SIG = USER_SIG + 1, STATE_UPDATED_SIG};

struct ComputerCommunicator
{
    // Members --------------------------------
    struct Ao super;
    QueueHandle_t state_sub;



    // Methods --------------------------------------------------------
    Status (*initial)(struct ComputerCommunicator *const self, Event const * const event);
    Status (*wait)(struct ComputerCommunicator *const self, Event const * const event);
    Status (*sending)(struct ComputerCommunicator *const self, Event const * const event);
    void (*public)(QueueHandle_t xQueue, const void * pvItemToQueue);
    void (*publicFromISR)(QueueHandle_t xQueue, const void * pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken);
};
extern const struct ComputerCommunicatorClass 
{
    void (*new)(struct ComputerCommunicator * self);
} ComputerCommunicator;

#endif /*_COMPUTER_COMMUNICATOR_H */