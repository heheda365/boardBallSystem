#include "usart2.h"
#include "openMV.h"

spot_t spot;

//
// 连接openMV相关端口初始化
//
//
void openMVPortInit(void){
	USART2_Init(115200);
}

//
// 用在串口中断中处理接收到的openMV数据
//
//
void openMVDataProess(uint8_t res){
	static uint8_t rxBuf[200];
	static int16_t rxBufCut = 0;
	uint16_t vas = 0;
	uint16_t i = 0;
	// R 为起始标志
	if(res == 'R'){
		rxBufCut = 0;
	}
	if(rxBufCut > 199){
		rxBufCut = -1;
	}
	// 读值到缓冲区
	if(rxBufCut >= 0){
		rxBuf[rxBufCut ++] = res;
	}
	
	if(rxBuf[0] == 'R' && res == 'O'){
		// 收到一帧数据开始处理
		if(rxBuf[1] == '('){
			i = 0;
			while(rxBuf[2 + i] != ','){
				vas = (rxBuf[2 + i] - '0') + vas * 10;
				i ++;
			}
			spot.x = vas;
			vas = 0;
			while(rxBuf[4 + i] != ')'){
				vas = (rxBuf[4 + i] - '0') + vas * 10;
				i ++;
			}
			spot.y = vas;
			vas = 0;
		}
	}
	
}

//
// 获取位置数据
//
void getCoordinate(void){
	
}


