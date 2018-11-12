#include "stdio.h"
#include "bsp_ili9341_lcd.h"
#include "fonts.h"	
#include "Control.h"
#include "display.h"

void displayInit(void){
	ILI9341_Init();
	ILI9341_GramScan ( 6 );
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
	ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	
}

void displayWorkState(void){
	static int8_t itemLast =  -1;
	static int8_t stateLast	=  -1;
	char str[20] = {0};
	
	// 当前项目
	if(systemControl.item != itemLast){
		switch(systemControl.item){
			case ITEM1:
				ILI9341_DispStringLine_EN(LINE(3), "     ITEM1       ");
				break;
				
			case ITEM2:
				ILI9341_DispStringLine_EN(LINE(3), "     ITEM2      ");
				break;
				
			case ITEM3:
				ILI9341_DispStringLine_EN(LINE(3), "     ITEM3       ");
				break;
				
			case ITEM4:
				ILI9341_DispStringLine_EN(LINE(3), "     ITEM4       ");
				break;
				
			case ITEM5:
				ILI9341_DispStringLine_EN(LINE(3), "     ITEM5       ");
				break;
			
			case ITEM6:
				ILI9341_DispStringLine_EN(LINE(3), "     ITEM6       ");
				break;
			case ITEM7: 
				ILI9341_DispStringLine_EN(LINE(3), "     ITEM7       ");
				break;
			
			case ITEM8:
				ILI9341_DispStringLine_EN(LINE(3), "     ITEM8        ");
				break;
			
			case ADJUST_ZORO_ANGLE:
				ILI9341_DispStringLine_EN(LINE(3), "     adjustAngle  ");
				break;
			
			case TEST:
				ILI9341_DispStringLine_EN(LINE(3), "     TEST   ");
				break;
			case MOTO_TEST:
				ILI9341_DispStringLine_EN(LINE(3), "     MOTO_TEST  ");
				break;
			
		}
	}
	// 工作状态
	if(systemControl.state != stateLast){
		switch(systemControl.state){
			case CALIBATIONING:
				ILI9341_DispStringLine_EN(LINE(1), "         calibationing...");
				break;
			case WORKING:
				ILI9341_DispStringLine_EN(LINE(1), "              working...");
				break;
			case PREPAREING:
				ILI9341_DispStringLine_EN(LINE(1), "           prerareing...");
				break;
			case COORDINATE_CAL:
				ILI9341_DispStringLine_EN(LINE(1), "   coordinateCalibretion...");
				break;
			case STOP:
				ILI9341_DispStringLine_EN(LINE(1), "                 stop...");
				break;
		}
	}
	itemLast = systemControl.item;
	stateLast = systemControl.state;
	
	
	
	// 显示小球坐标
	sprintf(str, "(%3d, %3d)", 
				systemControl.centrePosition[0],
				systemControl.centrePosition[1]);
	ILI9341_DispString_EN(0, 230, "centrePosition");
	ILI9341_DispString_EN(120, 230, str);
	
	// 显示中点坐标
	sprintf(str, "(%3d, %3d)", 
				systemControl.ballPosition[0],
				systemControl.ballPosition[1]);
	ILI9341_DispString_EN(0, 200, "ballPosition");
	ILI9341_DispString_EN(120, 200, str);
	
	// 角度数据
	
	sprintf(str, "(%.2f, %.2f)", 
				systemControl.pitch,
				systemControl.roll);
	ILI9341_DispString_EN(0, 260, "angle");
	ILI9341_DispString_EN(100, 260, str);
	
	sprintf(str, "(%.2f, %.2f)", 
				systemControl.zeroPitch,
				systemControl.zeroRoll);
	ILI9341_DispString_EN(0, 290, "zeroAngle");
	ILI9341_DispString_EN(100, 290, str);
	
	// 显示设定路径
	if(pathStringCut != 0){
		ILI9341_DispString_EN(0, 160, "path");
		ILI9341_DispString_EN(70, 160, pathString);
	} else {
		ILI9341_DispString_EN(0, 160, "                     ");
	}
	
}


