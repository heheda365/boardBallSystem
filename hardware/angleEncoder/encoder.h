#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f10x.h"
#include "sys.h"

//=============================CSN======================================
#define  ENCOLD_CS	       PBout(6)           
//=============================SCK======================================
#define  ENCOLD_CLK		   PBout(9)

#define  ENCOLD_DATA	   PBin(8)

void encoder_gpio(void);
void getdata(void);
uint16_t getEncoldData(void);

#endif
