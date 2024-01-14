#ifndef _DSP_H
#define _DSP_H

#include "encoder.h"
#include "arm_math.h"
#include "topic.h"

typedef struct Dsp Dsp;

struct Dsp
{
    // Members --------------------------------
    float32_t motor_resol;
    float32_t pendlm_resol;

    float32_t motor_angle; 
    float32_t pendlm_angle;

    float32_t filtered_motor_angle;
    float32_t filtered_pendlm_angle;

    StateData estimated_motor_data;
    StateData estimated_pendlm_data;

    // Methods --------------------------------------------------------
    void (*processNewData)(struct Dsp * const self, Encoder const * const encoder_topic, State * const state_topic);
    void (*cvtAngle)(struct Dsp * const self, Encoder const * const encoder_topic);
    void (*filter)(struct Dsp * const self);
    void (*estimate)(struct Dsp * const self);
};
void Dsp_new(struct Dsp * const self);


#endif /* _DSP_H */