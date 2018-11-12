#ifndef __KEY_H
#define	__KEY_H

#include "stm32f10x.h"
#include <stdbool.h>
 /*******
 *按键按下标置
 KEY_ON 0
 KEY_OFF 1
 ********/
 typedef struct {
 	uint8_t data;
 	uint8_t isPress; 
 } key_t;

extern key_t key;

void Key_GPIO_Config(void);
void Key_button(void);
	

#endif 

