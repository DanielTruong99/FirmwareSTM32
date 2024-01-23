#ifndef _DSP_H
#define _DSP_H

#include "encoder.h"
#include "arm_math.h"
#include "topic.h"
#include "queue.h"
#include "filter_data.h"

typedef struct Differentiator Differentiator;
struct Differentiator
{
    float32_t coeffs[3];
    float32_t sample_time;
    float32_t state[2];
    arm_biquad_cascade_df2T_instance_f32 filter;
    float32_t * filter_state;

    void (*apply)(Differentiator * const self, float32_t * input, float32_t * output);
};
void Differentiator_new(Differentiator * const self, float32_t sample_time, uint8_t num_states, float32_t * filter_coeffs);


typedef struct Reducer Reducer;
struct Reducer
{
    float32_t threshold;
    float32_t reduced_angle[4];

    void (*apply)(struct Reducer * const self, float32_t * raw_angle, float32_t * clean_angle);
};
void Reducer_new(struct Reducer * const self, float32_t threshold);

typedef struct Dsp Dsp;
struct Dsp
{
    // Members --------------------------------
    float32_t raw_motor_angle;
    float32_t raw_pendlm_angle;

    float32_t motor_resol;
    float32_t pendlm_resol;

    float32_t gear_ratio;
    float32_t previous_x;

    float32_t motor_angle; 
    float32_t pendlm_angle;

    float32_t filtered_motor_angle;
    float32_t filtered_pendlm_angle;
    float32_t filtered_motor_vel;
    float32_t filtered_pendlm_vel;

    StateData estimated_motor_data;
    StateData estimated_pendlm_data;
    StateData estimated_cart_data;

    Reducer * motor_reducer;
    Reducer * pendlm_reducer;

    Differentiator * motor_differentiator;
    Differentiator * pendlm_differentiator;

    // Methods --------------------------------------------------------
    void (*processNewData)(struct Dsp * const self, Encoder const * const encoder_topic, State * const state_topic);
    void (*cvtAngle)(struct Dsp * const self, Encoder const * const encoder_topic);
    void (*filter)(struct Dsp * const self);
    void (*estimate)(struct Dsp * const self);
};
void Dsp_new(struct Dsp * const self);


#endif /* _DSP_H */