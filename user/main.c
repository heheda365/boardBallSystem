#include "stm32f10x.h"
#include "delay.h"
#include "sys.h" 
#include "stm32f10x_it.h"
#include "timer.h"
#include "usart.h"

#include "display.h"
#include "Control.h"

#include "moto.h"
#include "angle.h"
#include "bsp_ili9341_lcd.h"
#include "key.h" 
#include "openMV.h"

int cut = 0;
int main(void)
{	 
	SystemInit();
	delay_init();	    	 	//延时函数初始化	  
	NVIC_Configuration(); 	 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);			//调试用串口
	TIM2_Int_Init(9999,71);		//1M的计数频率，10ms进一次中断
	
	// 显示初始化
	ILI9341_Init();
	ILI9341_GramScan ( 6 );
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	
	
	// 角度传感器初始化
	initMPU();					
	motoInit();
	Key_GPIO_Config();
	openMVPortInit();
	
	
	PID_Init();
	controlDataInit();
	while(1)
	{
		// 更新数据
		if(getTime10MsFlag()){
			// 获取传感器数据
			getSensorData();
			// 按键
			Key_button();
			// 控制状态切换
			systemStateCtrl();
			
		}
		// 显示工作状态
		if(getTime100MsFlag()){
			// 显示
			displayWorkState();
			ballMoveCtrl();
		
		}
	}
}	
 
