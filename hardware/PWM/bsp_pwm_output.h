#ifndef __PWM_OUTPUT_H
#define	__PWM_OUTPUT_H

#include "stm32f10x.h"

void TIM3_PWM_Init(int upleft);
void TIM3_PWM_Out(uint16_t num);

#endif /* __PWM_OUTPUT_H */

