#ifndef _STATE_ESTIMATOR_H
#define _STATE_ESTIMATOR_H

#include "stm32f4xx_hal.h"
#include "active_object.h"
#include "arm_math.h"
#include "topic.h"
#include "encoder.h"

typedef struct TimeEvent TimeEvent;

enum StateEstimatorEvent{TIMEOUT_2KHz_SIG = USER_SIG};

struct TimeEvent
{
    Event super;
};



struct StateEstimator
{
    // Members --------------------------------
    struct Ao super;


    // Methods --------------------------------------------------------
    Status (*initial)(struct StateEstimator *const self, Event const * const event);
    Status (*wait)(struct StateEstimator *const self, Event const * const event);
};
extern const struct StateEstimatorClass 
{
    void (*new)(struct StateEstimator * self);
} StateEstimator;



#endif /* _STATE_ESTIMATOR_H */