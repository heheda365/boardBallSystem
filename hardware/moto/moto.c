#include <math.h>
#include "pwmOutput.h"
#include "moto.h"

//
//
//
//
void motoInit(void){
	tim3PwmInit();
	tim5PwmInit();
}

//
// 电机速度更新
// speed -1000 ~ 1000 负数表示反转
// which : 0 和 1，电机 0， 电机 1
void motoSpeedUpdata(int16_t speed, uint8_t which){
	uint16_t speedTemp = 0;
	if(speed > 1000){
		speed = 1000;
	}
	if(speed < -1000){
		speed = -1000;
	}
	switch(which){
		case 0:
			if(speed < 0){
				speedTemp = - speed;
				tim3PwmSetDutyCycle(speedTemp, 1);
				tim3PwmSetDutyCycle(0, 2);
			} else {
				tim3PwmSetDutyCycle(0, 1);
				tim3PwmSetDutyCycle(speed, 2);
			}
			break;
			
		case 1:
			if(speed < 0){
				speedTemp = - speed;
				tim5PwmSetDutyCycle(speedTemp, 1);
				tim5PwmSetDutyCycle(0, 2);
			} else {
				tim5PwmSetDutyCycle(0, 1);
				tim5PwmSetDutyCycle(speed, 2);
			}
			break;
	}
}




