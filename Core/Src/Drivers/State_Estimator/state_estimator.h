#ifndef _STATE_ESTIMATOR_H
#define _STATE_ESTIMATOR_H

#include "stm32f4xx_hal.h"
#include "active_object.h"
#include "arm_math.h"
#include "topic.h"

typedef struct StateEstimator StateEstimator;
typedef struct TimeEvent TimeEvent;

enum StateEstimatorEvent{TIMEOUT_2KHz_SIG = USER_SIG};

struct TimeEvent
{
    Event _super;
    int16_t _motor_counter;
    int16_t _pendlm_counter;
};
// TimeEvent constructor
void TimeEvent_ctor(TimeEvent * const self, Event event);


struct StateEstimator
{
    // Members --------------------------------
    Ao _super;
    StateTopic _states_topic;
    EncoderTopic _motor_topic;
    EncoderTopic _pendlm_topic;

    // Methods --------------------------------------------------------
    Status (*initial)(StateEstimator *const self, Event *const event);
    Status (*wait)(StateEstimator *const self, Event *const event);
};
Status StateEstimator_initial(StateEstimator *const self, Event *const event);
Status StateEstimator_wait(StateEstimator *const self, Event *const event);
// State Estimator constructor
void StateEstimator_ctor(StateEstimator *const self);


/*Public Interfacce*/
void Driver_StateEstimator_Init();
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim_encoder);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);


#endif /* _STATE_ESTIMATOR_H */