#ifndef ULTRASONIC_H__
#define ULTRASONIC_H__
#include "stm32f10x.h"

void bspUltrasonicRead(u16 data);
u16 getUltrasonicData(void);
void TrigUT(void);
void UTInit(void);
void UTProcess(uint8_t wat);

extern u16 ultrasonicData[5];

#endif

