#ifndef _TOPIC_H
#define _TOPIC_H
#include "arm_math.h"
#include "stdint.h"

typedef struct EncoderStruct
{
  int16_t pendlm_counter;
  int16_t motor_counter;
} Encoder;

typedef struct StateDataStruct
{
  float32_t angle;
  float32_t vel;
  float32_t acc;
} StateData;


typedef struct StateStruct
{
  StateData motor;
  StateData pendlm;
  StateData cart;
} State;

typedef struct PWCStruct
{
  int16_t d;
} PWC;



#endif /* _TOPIC_H */