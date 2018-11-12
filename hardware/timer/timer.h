#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"
#include <stdbool.h>



void TIM2_Int_Init(u16 arr,u16 psc);
u8 getTime10MsFlag(void);
u8 getTime100MsFlag(void);
uint64_t getTime(void);
bool getTime1SFlag(void);
bool getTime30MsFlag(void);
 
#endif
