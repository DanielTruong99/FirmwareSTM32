#include "dsp.h"
extern float32_t pendlm_butterworth_coeffs[PENDULM_FILTER_STAGES_NUM * COEFFS_NUM];
extern float32_t motor_butterworth_coeffs[MOTOR_FILTER_STAGES_NUM * COEFFS_NUM];

static void Differentiator_apply(Differentiator * const self, float32_t * input, float32_t * output)
{
    float32_t diff_x;
    float32_t filtered_diff_x;
    float32_t x = *input;

    /*Estimate first derivative*/
    diff_x = self->coeffs[0] * x + self->coeffs[1] * self->state[1] + self->coeffs[2] * self->state[2];

    /*Store the updated state variables back into the state array*/
    self->state[2] = self->state[1]; self->state[1] = x;
    
    /*Filter estimated first derivative*/
    arm_biquad_cascade_df2T_f32(&self->filter, &diff_x, &filtered_diff_x, 1);

    /*Return output value*/
    *output = filtered_diff_x;
}

void Differentiator_new(Differentiator * const self, float32_t sample_time, uint8_t num_states, float32_t * filter_coeffs)
{
    /*Assign member functions*/
    self->apply = &Differentiator_apply;

    /*Initialize data members*/
    self->sample_time = sample_time;
    self->coeffs[0] = 3.0F / (2.0F * self->sample_time);
    self->coeffs[1] = -4.0F / (2.0F * self->sample_time);
    self->coeffs[2] = 1.0F / (2.0F * self->sample_time);
    self->state[0] = 0.0F; 
    self->state[1] = 0.0F;

    /*Initialize filter*/
    self->filter_state = ( float32_t * ) pvPortMalloc( 2 * num_states ); 

    arm_biquad_cascade_df2T_init_f32(&self->filter, num_states, filter_coeffs, self->filter_state);	
}

static void Reducer_apply(struct Reducer * const self, float32_t * raw_angle, float32_t * clean_angle)
{
    float32_t estimated_angle;
    float32_t reading_angle = *raw_angle;

    /*Estimate current angle*/
    estimated_angle = self->reduced_angle[1] + 1.5F * self->reduced_angle[1] - 2.0F * self->reduced_angle[2] + 0.5F * self->reduced_angle[3];

    /*Detect arbnormal angle data*/
    self->reduced_angle[0] = fabsf(estimated_angle - reading_angle) > self->threshold ? estimated_angle : reading_angle;
    *clean_angle = self->reduced_angle[0];

    /*Store the updated state variables back into the state array*/
    self->reduced_angle[3] = self->reduced_angle[2];
    self->reduced_angle[2] = self->reduced_angle[1];
    self->reduced_angle[1] = self->reduced_angle[0];
}

void Reducer_new(struct Reducer * const self, float32_t threshold)
{
    /*Assign member functions*/
    self->apply = &Reducer_apply;

    /*Initialize data members*/
    self->threshold = threshold;
    self->reduced_angle[0] = 0.0F; self->reduced_angle[1] = 0.0F;
    self->reduced_angle[2] = 0.0F; self->reduced_angle[3] = 0.0F;
}

static void cvtAngle(struct Dsp * const self, Encoder const * const encoder_topic)
{
    /*Estimate angle from reading encoder counter*/
    self->raw_motor_angle = self->motor_resol * encoder_topic->motor_counter;
    self->raw_pendlm_angle = self->pendlm_resol * encoder_topic->pendlm_counter;
}

static void filter(struct Dsp * const self)
{
    /*Reduce abnormal reading angle*/
    self->motor_reducer->apply(self->motor_reducer, &self->raw_motor_angle, &self->motor_angle);
    self->pendlm_reducer->apply(self->pendlm_reducer, &self->raw_pendlm_angle, &self->pendlm_angle);
    self->filtered_motor_angle = self->motor_angle;
    self->filtered_pendlm_angle = self->pendlm_angle;

    /*Estimate first derivative and filter*/
    self->motor_differentiator->apply(self->motor_differentiator, &self->filtered_motor_angle, &self->filtered_motor_vel);
    self->pendlm_differentiator->apply(self->pendlm_differentiator, &self->filtered_motor_angle, &self->filtered_pendlm_vel);
}

static void estimate(struct Dsp * const self)
{
    /*Output into interfaced variables estimated_motor_data*/
    self->estimated_motor_data.angle = self->filtered_motor_angle;
    self->estimated_motor_data.vel = self->filtered_motor_vel;
    self->estimated_pendlm_data.angle = self->filtered_pendlm_angle;
    self->estimated_pendlm_data.vel = self->filtered_pendlm_vel;

    /*Estimate x, v of cart
    * Because of utilizization of StateData structure, we need to note that 
    * estimated_cart_data.angle = x 
    * estimated_cart_data.vel = v
    */
    self->estimated_cart_data.angle = self->previous_x + self->filtered_motor_angle * self->gear_ratio;
    self->estimated_cart_data.vel = self->filtered_motor_vel * self->gear_ratio;
    self->previous_x = self->estimated_cart_data.angle;
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
    state_topic->cart = self->estimated_cart_data;
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
    self->raw_motor_angle = 0.0F; self->raw_pendlm_angle = 0.0F;
    self->filtered_motor_angle = 0.0F; self->filtered_pendlm_angle = 0.0F;
    self->filtered_motor_vel = 0.0F; self->filtered_pendlm_vel = 0.0F;

    /*Initialize Reducers*/
    self->motor_reducer = ( struct Reducer * ) pvPortMalloc( sizeof( struct Reducer ) );
    self->pendlm_reducer = ( struct Reducer * ) pvPortMalloc( sizeof( struct Reducer ) );
    Reducer_new(self->motor_reducer, 30.0F); 
    Reducer_new(self->pendlm_reducer, 0.5F);

    /*Initialize Differentiators*/
    self->motor_differentiator = ( Differentiator * ) pvPortMalloc( sizeof(Differentiator) );
    self->pendlm_differentiator = ( Differentiator * ) pvPortMalloc( sizeof(Differentiator) );
    Differentiator_new(self->motor_differentiator, 1E-3F, MOTOR_FILTER_STAGES_NUM, &motor_butterworth_coeffs[0]);
    Differentiator_new(self->pendlm_differentiator, 1E-3F, PENDULM_FILTER_STAGES_NUM, &pendlm_butterworth_coeffs[0]);

    /*Initialize estimated data output*/
    self->estimated_motor_data.angle = 0.0F;
    self->estimated_motor_data.vel = 0.0F;
    self->estimated_motor_data.acc = 0.0F;
    self->estimated_pendlm_data.angle = 0.0F;
    self->estimated_pendlm_data.vel = 0.0F;
    self->estimated_pendlm_data.acc = 0.0F;
    self->estimated_cart_data.angle = 0.0F; // angle = x 
    self->estimated_cart_data.vel = 0.0F;

    /**/
    /*Initialize gear ratio value
    * The assigned value: 
    *    self->gear_ratio = gear_ratio * motor_gear_radius
    */
    self->gear_ratio = ( 1.0F / 14.0F ) * 0.011F;
    self->previous_x = 0.0F;
}
