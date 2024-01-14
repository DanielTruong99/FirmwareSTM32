#include "dsp.h"

static void cvtAngle(struct Dsp * const self, Encoder const * const encoder_topic)
{
    self->motor_angle = self->motor_resol * encoder_topic->motor_counter;
    self->pendlm_angle = self->pendlm_resol * encoder_topic->pendlm_counter;
}

static void filter(struct Dsp * const self)
{
    self->filtered_motor_angle = self->motor_angle;
    self->filtered_pendlm_angle = self->pendlm_angle;
}

static void estimate(struct Dsp * const self)
{
    self->estimated_motor_data.angle = self->filtered_motor_angle;
    self->estimated_pendlm_data.angle = self->filtered_pendlm_angle;
}

static void processNewData(struct Dsp * const self, Encoder const * const encoder_topic, State * const state_topic)
{
    /*Output to self->motor_angle, pendlm_angle*/
    self->cvtAngle(self, encoder_topic);

    /*Output to self->filtered_motor_angle, self->filtered_pendlm_angle*/
    self->filter(self);

    /*Output to self->estimated_motor_data, self->estimated_pendlm_data*/
    self->estimate(self);

    /*Copy estimated data into state_topic*/
    state_topic->motor = self->estimated_motor_data;
    state_topic->pendlm = self->estimated_pendlm_data;
}

void Dsp_new(struct Dsp * const self)
{
    /*Assign function pointer*/
    self->processNewData = &processNewData;
    self->cvtAngle = &cvtAngle;
    self->estimate = &estimate;
    self->filter = &filter;

    /*Define resolution for encoders*/
    self->motor_resol = 2.0F * (float32_t)PI / (4.0F * 500.0F);
    self->pendlm_resol = 2.0F * (float32_t)PI / (4.0F * 1000.0F);

    /*Initialize value for data member*/
    self->motor_angle = 0.0F; self->pendlm_angle = 0.0F;
    self->filtered_motor_angle = 0.0F; self->filtered_pendlm_angle = 0.0F;

    self->estimated_motor_data.angle = 0.0F;
    self->estimated_motor_data.vel = 0.0F;
    self->estimated_motor_data.acc = 0.0F;
    self->estimated_pendlm_data.angle = 0.0F;
    self->estimated_pendlm_data.vel = 0.0F;
    self->estimated_pendlm_data.acc = 0.0F;
}
