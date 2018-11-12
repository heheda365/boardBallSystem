#include "bsp_pwm_output.h"
#include "stepMoto.h"
 


static void stepMotoGpioFig(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE); 

	// PE5 ÊÇ DiR  PE6 ÊÇ ENA
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void stepMotoInit(void){
	stepMotoGpioFig();
	STEP_MOTO_DIR = 1;
	STEP_MOTO_ENA = 1;
}

