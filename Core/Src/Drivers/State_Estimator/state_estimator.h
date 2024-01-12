#ifndef _STATE_ESTIMATOR_H
#define _STATE_ESTIMATOR_H

#include "stm32f4xx_hal.h"
#include "active_object.h"
#include "arm_math.h"
#include "topic.h"

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


/*Public Interfacce*/
void Driver_StateEstimator_Init();
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_encoder);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);


#endif /* _STATE_ESTIMATOR_H */