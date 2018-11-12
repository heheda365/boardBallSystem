#include "stdbool.h"
#include "delay.h"
#include "exti.h"
#include "timer5.h"
#include "ultrasonic.h"

u16 ultrasonicData[5] = {0};			// 超声波数据 mm为单位
//************************************************
// 以下函数是使用的是模拟的超声波器件（非串口读值）
//************************************************

//
// 超声波相关端口初始化
//
void UTInit(void){
	// 输出IO配置
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	// 接收端口
	extiPinPD11Init();
	
	TIM5_Int_Init(10000,72); // 计数频率 1mhz 10ms溢出一次
}
//
// 启动超声波发送
//
void TrigUT(void){
	GPIO_SetBits(GPIOD, GPIO_Pin_10);

	delay_us(20);

	GPIO_ResetBits(GPIOD, GPIO_Pin_10);
}

//
// 超声波中断处理
// 使用于中断中
// 参数表示那个超声波
void UTProcess(uint8_t wat){
	static uint64_t start[5] = {0};
	uint32_t detTime[5] = {0};
	switch(wat){
		case 0:
			if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11)){	// 位高电平起始
				start[0] = getTimeUs();
			} else {										// 低电平结束时间
				detTime[0] = getTimeUs() - start[0];		// 计算时间差
				if(detTime[0] > 200000){
					ultrasonicData[0] = 6000; 
				} else {
					ultrasonicData[0] = (uint16_t)(detTime[0] / 5.566);
				}
			}
			break;
		case 1:
			if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)){	// 位高电平起始
				start[1] = getTimeUs();
			} else {										// 低电平结束时间
				detTime[1] = getTimeUs() - start[1];		// 计算时间差
				if(detTime[1] > 200000){
					ultrasonicData[0] = 6000; 
				} else {
					ultrasonicData[1] = (uint16_t)(detTime[1] / 5.566);
				}
			}
			break;
		case 2:
			if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)){	// 位高电平起始
				start[2] = getTimeUs();
			} else {										// 低电平结束时间
				detTime[2] = getTimeUs() - start[2];		// 计算时间差
				if(detTime[2] > 200000){
					ultrasonicData[2] = 6000; 
				} else {
					ultrasonicData[2] = (uint16_t)(detTime[2] / 5.566);
				}
			}
			break;
		case 3:
			if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6)){	// 位高电平起始
				start[3] = getTimeUs();
			} else {										// 低电平结束时间
				detTime[3] = getTimeUs() - start[3];		// 计算时间差
				if(detTime[3] > 200000){
					ultrasonicData[3] = 6000; 
				} else {
					ultrasonicData[3] = (uint16_t)(detTime[3] / 5.566);
				}
			}
			break;
		
		case 4:
			if(GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9)){	// 位高电平起始
				start[4] = getTimeUs();
			} else {										// 低电平结束时间
				detTime[4] = getTimeUs() - start[4];		// 计算时间差
				if(detTime[4] > 200000){
					ultrasonicData[4] = 6000; 
				} else {
					ultrasonicData[4] = (uint16_t)(detTime[4] / 5.566);
				}
			}
			break;
	}
	
}

