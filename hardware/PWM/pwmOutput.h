#ifndef PWMOUTPUT_H__
#define PWMOUTPUT_H__
#include "stm32f10x.h"

#define TIM_PERIOD					3000
#define PERCENT_TO_CONPARE			TIM_PERIOD / 1000

void tim3PwmInit(void);
void tim3PwmSetDutyCycle(uint16_t percent, uint8_t channel);
void tim5PwmInit(void);
void tim5PwmSetDutyCycle(uint16_t percent, uint8_t channel);


#endif


