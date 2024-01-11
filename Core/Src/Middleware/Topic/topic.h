#ifndef _TOPIC_H
#define _TOPIC_H


typedef struct EncoderTopic EncoderTopic;
struct EncoderTopic
{
  float _resol; 
  int16_t _counter;
};

typedef struct State State;
struct State
{
    float _angle;
    float _vel;
    float _accel;
};

typedef struct StateTopic StateTopic;
struct StateTopic
{
  State _motor; 
  State _pendlm;
};


#endif /* _TOPIC_H */