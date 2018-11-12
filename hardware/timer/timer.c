#include <stdint.h>
#include <stdbool.h>

#include "timer.h"

static u8 time_10MsFlag = 0;
static u8 time100MsFlag = 0;
static u8 time30MsFlag	= 0;
static u8 time100MsCut	= 0;
static uint64_t	timeCut	= 0;
static uint8_t	time30MsCut = 0; 

static bool timeSFlag = 0;
static uint16_t timeSCut = 0;

void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM2, ENABLE);  //使能TIMx					 
}
//定时器3中断服务程序
void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update );  //清除TIMx更新中断标志 
		time_10MsFlag = 1;
		time100MsCut ++;
		timeSCut ++;
		time30MsCut ++;
		if(time100MsCut == 10){
			time100MsCut = 0;
			time100MsFlag = 1;
		}
		if(timeSCut == 100){
			timeSCut = 0;
			timeSFlag = 1;
		}
		if(time30MsCut >= 3){
			time30MsCut = 0;
			time30MsFlag = 1;
		}
		timeCut ++;
	}
}
//
// 10ms标志
//
u8 getTime10MsFlag(void)
{
	if(time_10MsFlag == 1)
	{
		time_10MsFlag = 0;
		return  1;
	}
	else return 0;
}
//
// 100ms标志
//
u8 getTime100MsFlag(void){
	if(time100MsFlag == 1){
		time100MsFlag = 0;
		return 1;
	}
	else return 0;
}
//
// 1S 的标志
//
//
bool getTime1SFlag(void){
	if(timeSFlag == 1){
		timeSFlag = 0;
		return true;
	}
	else return false;
}

//
// 30ms 标志
//
//
bool getTime30MsFlag(void){
	if(time30MsFlag == 1){
		time30MsFlag = 0;
		return true;
	}
	else return false;
}
//
// 返回系统运行时间
// 单位 1us
//
uint64_t getTime(void){
	uint64_t temp = 0;
	temp = timeCut * 10000 + TIM_GetCounter(TIM2); 
	return temp;
}










