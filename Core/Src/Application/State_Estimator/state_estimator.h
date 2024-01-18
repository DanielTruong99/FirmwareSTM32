#ifndef _STATE_ESTIMATOR_H
#define _STATE_ESTIMATOR_H

#include "stm32f4xx_hal.h"
#include "active_object.h"
#include "arm_math.h"
#include "topic.h"
#include "encoder.h"
#include "dsp.h"
#include "computer_communicator.h"

typedef struct TimeEvent TimeEvent;

enum StateEstimatorEvent{TIMEOUT_2KHz_SIG = USER_SIG};

struct StateEstimator
{
    // Members --------------------------------
    struct Ao super;
    struct Dsp * data_processor;
    QueueHandle_t encoder_sub;
    QueueHandle_t state_pub;



    // Methods --------------------------------------------------------
    Status (*initial)(struct StateEstimator *const self, Event const * const event);
    Status (*wait)(struct StateEstimator *const self, Event const * const event);
    void (*public)(QueueHandle_t xQueue, const void * pvItemToQueue);
    void (*publicFromISR)(QueueHandle_t xQueue, const void * pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken);
};
extern const struct StateEstimatorClass 
{
    void (*new)(struct StateEstimator * self);
} StateEstimator;



#endif /* _STATE_ESTIMATOR_H */