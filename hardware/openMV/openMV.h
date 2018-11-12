#ifndef OPENMV_H__
#define OPENMV_H__

#include "stm32f10x.h"
#include "stdbool.h"

typedef struct{
	uint16_t x;
	uint16_t y;
} spot_t;

extern spot_t spot;

void openMVPortInit(void);
void openMVDataProess(uint8_t res);
void getCoordinate(void);


#endif

