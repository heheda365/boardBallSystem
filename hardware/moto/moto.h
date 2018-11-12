#ifndef MOTO_H__
#define MOTO_H__
#include "stm32f10x.h"

void motoInit(void);
void motoSpeedUpdata(int16_t speed, uint8_t which);

#endif

