#include "pwmOutput.h"


void tim3PwmInit(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;


	/* 设置TIM3CLK 为 72MHZ */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

	/* GPIOA and GPIOB clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 

	/*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = TIM_PERIOD;       //当定时器从0计数到999，即为1000次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Prescaler = 8;	    //设置预分频：不预分频，即为72MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频(这里用不到)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = 0;	   				//设置跳变值，当计数器计数到这个值时，电平发生跳变
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 //使能通道1
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);			 // 使能TIM3重载寄存器ARR

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);                   //使能定时器3	
}

//
// 改变占空比 
// 百分比表示 0 - 999
//
void tim3PwmSetDutyCycle(uint16_t percent, uint8_t channel){
	uint16_t compare = 0;
	compare = percent * PERCENT_TO_CONPARE;
	switch(channel){
		case 1:
			TIM_SetCompare1(TIM3, compare);
			break;
		case 2:
			TIM_SetCompare2(TIM3, compare);
			break;
	}
}

//
// 定时器1的PWM初始化
//
//
void tim5PwmInit(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;


	/* 设置TIM5CLK 为 72MHZ */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 

	/* GPIOA and GPIOB clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 

	/*GPIOA Configuration: TIM5 channel 1 and 2 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = TIM_PERIOD;       //当定时器从0计数到999，即为1000次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Prescaler = 8;	    //设置预分频：不预分频，即为72MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频(这里用不到)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = 0;	   				//设置跳变值，当计数器计数到这个值时，电平发生跳变
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平
	
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);	 //使能通道1
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM5, ENABLE);			 // 使能TIM5重载寄存器ARR

	/* TIM5 enable counter */
	TIM_Cmd(TIM5, ENABLE);                   //使能定时器3	
}

//
// 改变占空比 
// 百分比表示 0 - 1000
//
void tim5PwmSetDutyCycle(uint16_t percent, uint8_t channel){
	uint16_t compare = 0;
	compare = percent * PERCENT_TO_CONPARE;
	switch(channel){
		case 1:
			TIM_SetCompare1(TIM5, compare);
			break;
		case 2:
			TIM_SetCompare2(TIM5, compare);
			break;
	}
}


/*********************************************END OF FILE**********************/
