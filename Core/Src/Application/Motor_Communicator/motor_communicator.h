#ifndef _MOTOR_COMMUNICATOR_H
#define _MOTOR_COMMUNICATOR_H

#include "stm32f4xx_hal.h"
#include "active_object.h"
#include "topic.h"

enum MotorCommunicatorEvent{PWC_TRIGGER_SIG = USER_SIG, COMMAND_SENDED_SIG};

struct MotorCommunicator
{
    // Members --------------------------------
    struct Ao super;
    QueueHandle_t pwc_sub;



    // Methods --------------------------------------------------------
    Status (*initial)(struct MotorCommunicator *const self, Event const * const event);
    Status (*wait)(struct MotorCommunicator *const self, Event const * const event);
    Status (*sending)(struct MotorCommunicator *const self, Event const * const event);
    void (*public)(QueueHandle_t xQueue, const void * pvItemToQueue);
    void (*publicFromISR)(QueueHandle_t xQueue, const void * pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken);
};
extern const struct MotorCommunicatorClass 
{
    void (*new)(struct MotorCommunicator * self);
} MotorCommunicator;


#endif /*_MOTOR_COMMUNICATOR_H */