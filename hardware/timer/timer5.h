#ifndef TIMER5_H__
#define TIMER5_H__
#include "stm32f10x.h"
#include "stdbool.h"

void TIM5_Int_Init(u16 arr,u16 psc);
uint64_t getTimeUs(void);

extern u8 timer5IntFlag;
#endif

