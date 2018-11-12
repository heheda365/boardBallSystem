#ifndef __STEPMOTO_H
#define __STEPMOTO_H

#include "stm32f10x.h"
#include "sys.h"

#define STEP_MOTO_DIR PEout(5)
#define STEP_MOTO_ENA PEout(6)

void stepMotoInit(void);



#endif


