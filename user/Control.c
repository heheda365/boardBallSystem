/*    

功能：
1.PID参数初始化
2.控制函数
------------------------------------
*/
#include <math.h>
#include <stdlib.h>
#include "timer.h"
#include "angle.h"
#include "openMV.h"
#include "moto.h"
#include "key.h"
#include "Control.h"

char pathString[20] = {0};
uint8_t pathStringCut = 0;
enum{
	AREA1 = 0,
	AREA2 = 1, 
	AREA3 = 2,
	AREA4 = 3,
	AREA5 = 4, 
	AREA6 = 5,
	AREA7 = 6,
	AREA8 = 7,
	AREA9 = 8,
}; // 区域序号

typedef struct{
	spot_t spot[9];
} cyclo_area_t;

cyclo_area_t cycloArea;		// 记录9个圆形区域的坐标


#define micros getTime
#define SPEED_MAX	0.4f
#define THRO_MAX	1000

systemControl_t systemControl;

PID_Typedef xCtrl;
PID_Typedef yCtrl;

PID_Typedef pitchCtrl;
PID_Typedef rollCtrl;

PID_Typedef xDistanceCtrl;
PID_Typedef yDistanceCtrl;


//
// 保存目标区域的位置
//
//
void cycloAreaSpotInit(void){
	cycloArea.spot[AREA1].x = 244;
	cycloArea.spot[AREA1].y = 188;
	
	cycloArea.spot[AREA2].x = 176;
	cycloArea.spot[AREA2].y = 188;
	
	cycloArea.spot[AREA3].x = 106;
	cycloArea.spot[AREA3].y = 186;
	
	cycloArea.spot[AREA4].x = 248;
	cycloArea.spot[AREA4].y = 118;
	
	cycloArea.spot[AREA5].x = 177;
	cycloArea.spot[AREA5].y = 117;
	
	cycloArea.spot[AREA6].x = 104;
	cycloArea.spot[AREA6].y = 116;
	
	cycloArea.spot[AREA7].x = 246;
	cycloArea.spot[AREA7].y = 50;
	
	cycloArea.spot[AREA8].x = 177;
	cycloArea.spot[AREA8].y = 45;
	
	cycloArea.spot[AREA9].x = 107;
	cycloArea.spot[AREA9].y = 46;
}

/*
* PID CH
*
*/
void PID_Init(void)
{
	
	
	xCtrl.P		= 0.030;		// 0.030
	xCtrl.I		= 0.1;		// 0.1
	xCtrl.D		= 0.040;	//	0.035
	xCtrl.iLimit = 1;

	yCtrl.P		= 0.030;			// 0.030
	yCtrl.I		= 0.1;			//	0.1
	yCtrl.D		= 0.040;		// 35
	yCtrl.iLimit = 1;
	
	
	pitchCtrl.P = 180;
	pitchCtrl.I = 0.05;
	pitchCtrl.D = 0;
	pitchCtrl.iLimit = 20;
	
	rollCtrl.P = 180;
	rollCtrl.I = 0.05;
	rollCtrl.D = 0;
	rollCtrl.iLimit = 20;
	
	
	xDistanceCtrl.P		= 250;		// 0.030
	xDistanceCtrl.I		= 0.05;		// 0.1
	xDistanceCtrl.D		= 0;	//	0.035
	xDistanceCtrl.iLimit = 20;

	yDistanceCtrl.P		= 250;			// 0.030
	yDistanceCtrl.I		= 0.05;			//	0.1
	yDistanceCtrl.D		= 0;		// 35
	yDistanceCtrl.iLimit = 20;
	
	

}   
//***************************************************
//
//  控制数据初始化
//
//***************************************************
void controlDataInit(void){
	
	cycloAreaSpotInit();
	systemControl.item		= ITEM1;
	systemControl.xCtrlOut	= 0;
	systemControl.xCtrl_exp	= 0;
	systemControl.xCtrl_mea	= 0;
	systemControl.yCtrlOut	= 0;
	systemControl.yCtrl_exp	= 0;
	systemControl.yCtrl_mea	= 0;
	systemControl.start			= true;
	systemControl.thro				= 0;
	
	systemControl.state = STOP;
	
	systemControl.needRst = false;
	
	systemControl.centrePosition[0] = cycloArea.spot[AREA5].x;
	systemControl.centrePosition[1] = cycloArea.spot[AREA5].y;
	systemControl.haveCalCooldinate = false;
	systemControl.zeroPitch = LEVER_ANGLE_PITCH;
	systemControl.zeroRoll = LEVER_ANGLE_ROLL;
}
//-----------位置式PID-----------
static void PID_Postion_Cal(PID_Typedef * PID,float target,float measure,int32_t dertT)
{
 float termI=0;
 float dt= dertT/1000000.0;
	//-----------位置式PID-----------
	//误差=期望值-测量值
	PID->Error=target-measure;
	
	PID->Deriv= (PID->Error-PID->PreError)/dt;
	
	PID->Output=(PID->P * PID->Error) + (PID->I * PID->Integ) + (PID->D * PID->Deriv);    //PID:比例环节+积分环节+微分环节
	
	PID->PreError=PID->Error;

	if(systemControl.start)   // 
	{
	
		termI=(PID->Integ) + (PID->Error) * dt;     //积分环节
		if(termI > - PID->iLimit && termI < PID->iLimit && PID->Output > - PID->iLimit && PID->Output < PID->iLimit) 
				PID->Integ=termI;
	
	}
	else
			PID->Integ = 0;
}

//*********************************************************
//
// 两轮速度PID控制
// 使用在100HZ的循环中
//*********************************************************
void CtrlSpeed(void)
{
	static uint64_t tPrev=0;
	uint64_t dt=0,t=0;
	t = micros();
	dt=(tPrev>0)?(t-tPrev):0;
	tPrev=t;
	
	PID_Postion_Cal(&xCtrl,systemControl.xCtrl_exp,systemControl.xCtrl_mea,dt);
	PID_Postion_Cal(&yCtrl,systemControl.yCtrl_exp,systemControl.yCtrl_mea,dt);	
	
	systemControl.xCtrlOut = xCtrl.Output + systemControl.xCtrl_exp;
	systemControl.yCtrlOut  = yCtrl.Output + systemControl.yCtrl_exp;
	
}

//
// 获取角度和坐标数据
//
void getSensorData(void){
	float pitch = 0;
	float roll = 0;
	float yaw = 0;
	short acc[3] ={0};
	// 获取角度
	if(readAngleData(&pitch, &roll, &yaw, acc) == 1){
		systemControl.pitch = pitch;
		systemControl.roll = roll;
	}
	
	// 获取坐标
	systemControl.ballPosition[0] = spot.x;
	systemControl.ballPosition[1] = spot.y;
}

static int16_t pwm = 300;
void motoTestUp(void){
	pwm += 10;
	motoSpeedUpdata(-pwm, 1);
	
}
void motoTestDown(void){
	pwm -= 10;
	motoSpeedUpdata(-pwm, 1);
}
//
// 系统状态控制
//
//
void systemStateCtrl(void){
	if(key.isPress){
		if(systemControl.item != ITEM6 && systemControl.item != ADJUST_ZORO_ANGLE){
			switch(key.data){
				case 1:
					systemControl.item = ITEM1;
					break;
				case 2:
					systemControl.item = ITEM2;
					break;
				case 3:
					systemControl.item = ITEM3;
					break;
				case 4:
					systemControl.item = ITEM4;
					break;
				case 5:
					systemControl.item = ITEM5;
					break;
				case 6:
					systemControl.item = ITEM6;
					break;
				case 7:
					systemControl.item = ITEM7;
					break;
				case 8:
					systemControl.item = ITEM8;
					break;
				case 9:
					systemControl.item = ADJUST_ZORO_ANGLE;
					break;
			}
		} else {
			if((systemControl.item == ITEM6)){
			// 第五项时用于设置小球运动路径
				switch(key.data){
					case 1:
						pathString[pathStringCut] = '1';
						break;
					case 2:
						pathString[pathStringCut] = '2';
						break;
					case 3:
						pathString[pathStringCut] = '3';
						break;
					case 4:
						pathString[pathStringCut] = '4';
						break;
					case 5:
						pathString[pathStringCut] = '5';
					
						break;
					case 6:
						pathString[pathStringCut] = '6';
					
						break;
					case 7:
						pathString[pathStringCut] = '7';
						break;
					case 8:
						pathString[pathStringCut] = '8';
						break;
					case 9:
						pathString[pathStringCut] = '9';
						break;
				}
			
				pathStringCut ++;
				if(pathStringCut > 3){
					pathStringCut = 3;
				}
			}
			
			//
			// 调整角度时的按键处理
			if(systemControl.item == ADJUST_ZORO_ANGLE){
				switch(key.data){
					case 1:
						systemControl.zeroPitch -= 0.01;
						break;
					case 2:
						systemControl.zeroPitch += 0.01;
						break;
					case 3:
						systemControl.zeroRoll -= 0.01;
						break;
					case 4:
						systemControl.zeroRoll += 0.01;
						break;
				}
			}
		}
		switch(key.data){
			
			
			case 11:
				systemControl.item = MOTO_TEST;
				motoTestUp();
				break;
			case 12:
				systemControl.state = COORDINATE_CAL;
				coordinateCalibretion();
				break;
				
			case 13:
				// 测试
				systemControl.item = TEST;
				break;
			case 14:
				systemControl.state = STOP;
				systemControl.needRst = true;
				break;
			case 15:
				// 调平校准
				systemControl.start = true;
				systemControl.state = CALIBATIONING;
				systemControl.needRst = true;
				
				if(systemControl.item == ITEM6){
					systemControl.item = ITEM1;
					for(int i=0; i<10; i++){
						pathString[i] = 0;
					}
					pathStringCut = 0;
				}
				if(systemControl.item == ADJUST_ZORO_ANGLE){
					systemControl.item = ITEM1;
				}
				break;
			case 16:
				// 开始
				systemControl.start = true;
				systemControl.state = WORKING;
				break;
		}
		key.isPress = false;
	}	
}
//
// 校准
// 根据角度传感器的数值将板子校准至水平
// 完成后自动切换到准备状态
void calibrationProcess(void){
	// 控制电机 调整至 水平
	static uint64_t tPrev=0;
	uint64_t dt=0,t=0;
	t = micros();
	dt=(tPrev>0)?(t-tPrev):0;
	tPrev=t;
//	int16_t pitchTemp = 0;
//	int16_t rollTemp = 0;
	
	int16_t moto0 = 0;
	int16_t moto1 = 0;
	
	PID_Postion_Cal(&pitchCtrl, systemControl.zeroPitch, systemControl.pitch, dt);
	PID_Postion_Cal(&rollCtrl,  systemControl.zeroRoll, systemControl.roll, dt);	
	
//	if(pitchCtrl.Output < 0){
//		pitchTemp = pitchCtrl.Output - MOTO_START_PWM;
//	} else {
//		pitchTemp = pitchCtrl.Output + MOTO_START_PWM;
//	}
//	if(rollCtrl.Output < 0){
//		rollTemp = rollCtrl.Output - MOTO_START_PWM;
//	} else {
//		rollTemp = rollCtrl.Output + MOTO_START_PWM;
//	}
	
	moto0 = - rollCtrl.Output;
	moto1 = - pitchCtrl.Output;
	if(moto0 < -10){
		moto0 = moto0 - MOTO0_UP_START_PWM;
	} 
	if(moto0 > 10) {
		moto0 = moto0 + MOTO0_DOWN_START_PWM;
	}
	if(moto1 < -10){
		moto1 = moto1 - MOTO1_UP_START_PWM;
	}
	if(moto1 > 10){
		moto1 = moto1 + MOTO1_DOWN_START_PWM;
	}
	motoSpeedUpdata(moto0, 0);
	motoSpeedUpdata(moto1, 1);
	// 判断是否达到水平
	if(systemControl.pitch < systemControl.zeroPitch + LEVER_ANGLE_THRE && 
		systemControl.pitch > systemControl.zeroPitch - LEVER_ANGLE_THRE &&
		systemControl.roll < systemControl.zeroRoll + LEVER_ANGLE_THRE &&
		systemControl.roll > systemControl.zeroRoll - LEVER_ANGLE_THRE){
		// 达到水平切换至 准备状态
		systemControl.state = PREPAREING;
	}
}
//
// 
//
//
//
// 直线运动测试
//
void lineMoveTest(void){
	
}
//
// 运动到指定点
//
void moveToSpot(spot_t destination){
	static uint64_t tPrev=0;
	uint64_t dt=0,t=0;
	
	int16_t moto0 = 0;
	int16_t moto1 = 0;
	
	float pitchExpect = 0;
	float rollExpect = 0;
	
	t = micros();
	dt=(tPrev>0)?(t-tPrev):0;
	tPrev=t;
	
	PID_Postion_Cal(&xCtrl, destination.x, systemControl.ballPosition[0], dt);
	PID_Postion_Cal(&yCtrl,  destination.y, systemControl.ballPosition[1], dt);
	
//	if(xCtrl.Output < -0.3){
//		rollExpect = xCtrl.Output - FRICTION_ANG;
//	}
//	if(xCtrl.Output > 0.3){
//		rollExpect = xCtrl.Output + FRICTION_ANG;
//	}
//	if(yCtrl.Output < -0.3){
//		pitchExpect = yCtrl.Output - FRICTION_ANG;
//	}
//	if(yCtrl.Output > 0.3){
//		pitchExpect = yCtrl.Output + FRICTION_ANG;
//	}
	rollExpect = xCtrl.Output;
	pitchExpect = yCtrl.Output;
	
	PID_Postion_Cal(&pitchCtrl, systemControl.zeroPitch - pitchExpect, systemControl.pitch, dt);
	PID_Postion_Cal(&rollCtrl,  systemControl.zeroRoll + rollExpect, systemControl.roll, dt);	
	
	moto0 = - rollCtrl.Output;
	moto1 = - pitchCtrl.Output;
	
	if(moto0 < -10){
		moto0 = moto0 - MOTO0_UP_START_PWM;
	} 
	if(moto0 > 10) {
		moto0 = moto0 + MOTO0_DOWN_START_PWM;
	}
	if(moto1 < -10){
		moto1 = moto1 - MOTO1_UP_START_PWM;
	}
	if(moto1 > 10){
		moto1 = moto1 + MOTO1_DOWN_START_PWM;
	}

	motoSpeedUpdata(moto0, 0);
	motoSpeedUpdata(moto1, 1);
	
}
//
// 第二项的处理函数
// 孔子小球从区域1到区域5
// 
void item2Process(void){
	// 移动至区域5
	moveToSpot(cycloArea.spot[AREA5]);
}
//
// 第三项处理函数
// 小球从区域1到区域4停留2S 
// 然后从区域4到区域5
enum{
	AREA1_TO_AREA4 = 0,
	IN_AREA4 = 1,
	AREA4_TO_AREA5
};
void item3Process(void){
	static uint16_t in4Cut = 0;
	static uint8_t state = AREA1_TO_AREA4;
	int errx = 0, erry = 0;
	
	if(systemControl.needRst == true){
		state = AREA1_TO_AREA4;
		in4Cut = 0;
		systemControl.needRst = false;
	}
	 
	errx = systemControl.ballPosition[0] - cycloArea.spot[AREA4].x;
	erry = systemControl.ballPosition[1] - cycloArea.spot[AREA4].y;
	//
	// 判断是否到4
	//
	if((errx * errx + erry * erry) < ALLOW_ERR * ALLOW_ERR){
		in4Cut ++;		
		state = IN_AREA4;
	}
	// 3S
	if(in4Cut > 35){
		state = AREA4_TO_AREA5;
	}
	
	switch(state){
		case AREA1_TO_AREA4:
			// 移动到4
			moveToSpot(cycloArea.spot[AREA4]);
			break;
		case IN_AREA4:
			moveToSpot(cycloArea.spot[AREA4]);
			break;
		case AREA4_TO_AREA5:
			moveToSpot(cycloArea.spot[AREA5]);
			break;
	}
}

//ucos II
// 第四项处理函数
// 从区域1到区域9 
//
void item4Process(void){
	static uint8_t state = 0; 
	spot_t spot;
	spot_t spot2;
	int errx = 0, erry = 0;
	int errx1 = 0, erry1 = 0;
	if(systemControl.needRst == true){
		state = 0;
		systemControl.needRst = false;
	}
	
	spot.x = (//cycloArea.spot[AREA2].x + cycloArea.spot[AREA3].x + 
			cycloArea.spot[AREA2].x + cycloArea.spot[AREA5].x) / 2;
	spot.y = (//cycloArea.spot[AREA2].y + cycloArea.spot[AREA3].y + 
				cycloArea.spot[AREA2].y + cycloArea.spot[AREA5].y) / 2;
				
	spot2.x = (cycloArea.spot[AREA5].x + cycloArea.spot[AREA6].x) / 2;
	spot2.y = (cycloArea.spot[AREA6].y + cycloArea.spot[AREA6].y) / 2;
			
	errx = systemControl.ballPosition[0] - spot.x;
	erry = systemControl.ballPosition[1] - spot.y;
	
	errx1 = systemControl.ballPosition[0] - spot2.x;
	erry1 = systemControl.ballPosition[1] - spot2.y;
	
	
	if((errx * errx + erry * erry) < ALLOW_ERR * ALLOW_ERR * 2 && state == 0){
		state = 1;
	}
	if((errx1 * errx1 + erry1 * erry1) < ALLOW_ERR * ALLOW_ERR * 2){
		state = 2;
	}
	
	switch(state){
		case 0:
			moveToSpot(spot);
			break;
		case 1:
			moveToSpot(spot2);
			break;
		case 2:
			moveToSpot(cycloArea.spot[AREA9]);
			break;
	}
	
}
//
// 发挥部分，第五项处理函数
// 1 到 2 再到 6 再到 9 ，在9内不少于2秒 
//
void item5process(void){
	static uint8_t state = 0;
	int errx = 0, erry = 0;
	if(systemControl.needRst == true){
		state = 0;
		systemControl.needRst = false;
	}
	switch(state){
		case 0:
			// 1到2
			moveToSpot(cycloArea.spot[AREA2]);
			errx = systemControl.ballPosition[0] - cycloArea.spot[AREA2].x;
			erry = systemControl.ballPosition[1] - cycloArea.spot[AREA2].y;
			if((errx * errx + erry * erry) < ALLOW_ERR * ALLOW_ERR){
				state = 1;
			}
			break;
		case 1:
			//2到6
			moveToSpot(cycloArea.spot[AREA6]);
			errx = systemControl.ballPosition[0] - cycloArea.spot[AREA6].x;
			erry = systemControl.ballPosition[1] - cycloArea.spot[AREA6].y;
			if((errx * errx + erry * erry) < ALLOW_ERR * ALLOW_ERR){
				state = 2;
			}
			break;
		case 2:
			// 6到9
			moveToSpot(cycloArea.spot[AREA9]);
			break;
		
	}
}
//
// 项目6处理函数
// 键盘指定ABCD 4个区域，小球依次经过
//
void item6process(void){
	static uint8_t state = 0;
	int errx = 0, erry = 0;
	spot_t spot[4];
	
	if(systemControl.needRst == true){
		state = 0;
		systemControl.needRst = false;
	}
	//
	// 路径设置 
	//
	if(pathStringCut == 3){
		for(int i = 0; i < 4; i++){
			if(pathString[i] > '0' && pathString[i] <= '9'){
				spot[i] = cycloArea.spot[pathString[i] - '1'];	
			}
		}
	}
	// 移动控制
	//
	switch(state){
		case 0:
			// 1到2
			moveToSpot(spot[1]);
			errx = systemControl.ballPosition[0] - spot[1].x;
			erry = systemControl.ballPosition[1] - spot[1].y;
			if((errx * errx + erry * erry) < ALLOW_ERR * ALLOW_ERR){
				state = 1;
			}
			break;
		case 1:
			//2到6
			moveToSpot(spot[2]);
			errx = systemControl.ballPosition[0] - spot[2].x;
			erry = systemControl.ballPosition[1] - spot[2].y;
			if((errx * errx + erry * erry) < ALLOW_ERR * ALLOW_ERR){
				state = 2;
			}
			break;
		case 2:
			// 6到9
			moveToSpot(spot[3]);
			break;
		
	}
}

//
// 画圆
// spot 画圆中心点
// radius 期望半径
// speed 旋转速度
// direction 旋转方向 0 为逆时针 1为顺时针
#define COORDINATE_TO_ANGLE		1
void drawCircle(spot_t spot, uint16_t radius, float speed, uint8_t direction){
	int16_t dx = 0;
	int16_t dy = 0;
	float dxNor = 0;
	float dyNor = 0;
	
	float pitchExp = 0;
	float rollExp = 0;
	
	static uint64_t tPrev=0;
	spot_t radiusSpot;
	
	uint16_t distance = 0;
	uint64_t dt=0,t=0;
	t = micros();
	dt=(tPrev>0)?(t-tPrev):0;
	tPrev=t;
	
	int16_t moto0 = 0;
	int16_t moto1 = 0;
	
	dx = systemControl.ballPosition[0] - spot.x;
	dy = systemControl.ballPosition[1] - spot.y;
	
	distance = sqrt(dx * dx + dy * dy);
	
	if((abs(dx) + abs(dy) != 0)){
		dxNor = 1.0 * dx / (abs(dx) + abs(dy));
	}
	if(abs(dx) + abs(dy)){
		dyNor = 1.0 * dy / (abs(dx) + abs(dy));
	}
	
	if(0 == direction){
		pitchExp = dxNor * speed;
		rollExp = dyNor * speed;
	} else {
		pitchExp = - dxNor * speed;
		rollExp = - dyNor * speed;
	}
	
	// 旋转速度控制
	PID_Postion_Cal(&pitchCtrl, pitchExp + systemControl.zeroPitch, systemControl.pitch, dt);
	PID_Postion_Cal(&rollCtrl,  rollExp + systemControl.zeroRoll, systemControl.roll, dt);	
	
	
	// 计算需要的目标点
	if(distance != 0){
		radiusSpot.x = spot.x + 1.0 * radius / distance * dx;
		radiusSpot.y = spot.y + 1.0 * radius / distance * dy;
	}
	// 旋转半径控制
	PID_Postion_Cal(&xCtrl, radiusSpot.x, systemControl.ballPosition[0], dt);
	PID_Postion_Cal(&yCtrl,  radiusSpot.y, systemControl.ballPosition[1], dt);
	
	PID_Postion_Cal(&yDistanceCtrl, systemControl.zeroPitch - yCtrl.Output, systemControl.pitch, dt);
	PID_Postion_Cal(&xDistanceCtrl,  systemControl.zeroRoll + xCtrl.Output, systemControl.roll, dt);	
	
	moto0 = - (rollCtrl.Output + xDistanceCtrl.Output);
	moto1 = - (pitchCtrl.Output + yDistanceCtrl.Output);
	
	if(moto0 < -10){
		moto0 = moto0 - MOTO0_UP_START_PWM;
	} 
	if(moto0 > 10) {
		moto0 = moto0 + MOTO0_DOWN_START_PWM;
	}
	if(moto1 < -10){
		moto1 = moto1 - MOTO1_UP_START_PWM;
	}
	if(moto1 > 10){
		moto1 = moto1 + MOTO1_DOWN_START_PWM;
	}
	motoSpeedUpdata(moto0, 0);
	motoSpeedUpdata(moto1, 1);
}

//
// 项目7
// 画圆
// 根据两点的位置方向计算倾角方向
// 位置方向和两点的连线方向垂直
void item7Process(void){
	static uint8_t state = 0;
	spot_t sp;
	static uint16_t cut = 0;
	static uint8_t cut1 = 0;
	
	if(systemControl.needRst == true){
		state = 0;
		cut = 0;
		cut1 = 0;
		systemControl.needRst = false;
	}
	
	if(getTime1SFlag()){
		cut ++;
	}
	
	sp.x = (cycloArea.spot[AREA4].x + cycloArea.spot[AREA8].x) / 2;
	sp.y = (cycloArea.spot[AREA4].y + cycloArea.spot[AREA8].y) / 2;
	if(abs(systemControl.ballPosition[0] - sp.x) < ALLOW_ERR / 2 &&
		abs(systemControl.ballPosition[1] - sp.y) < ALLOW_ERR * 4 
		){
		if(cut > 1){ 
			cut1 ++;
			cut = 0;
		}
	}
	if(cut1 > 4){
		state = 1;
		cut1 = 0;
	}
	switch(state){
		case 0:
			drawCircle(cycloArea.spot[AREA5], 0, 2.5, 0);
			break;
		case 1:
			moveToSpot(cycloArea.spot[AREA9]);
			break;
	}
}
//
// 项目8
// 终于最后一项了
// 画 8 字
void item8Process(void){
	static uint8_t state = 0;
	spot_t centreSpot1, centreSpot2, centreSpot3, centreSpot4;
	static uint16_t cut = 0;
	static uint8_t cut1 = 0;
	
	if(systemControl.needRst == true){
		state = 0;
		cut = 0;
		cut1 = 0;
		systemControl.needRst = false;
	}
	if(getTime1SFlag()){
		cut ++;
	}
	// 计算中心坐标
	centreSpot1.x = (cycloArea.spot[AREA2].x + cycloArea.spot[AREA5].x) / 2;
	centreSpot1.y = (cycloArea.spot[AREA2].y + cycloArea.spot[AREA5].y) / 2;
	
	centreSpot2.x = (cycloArea.spot[AREA5].x + cycloArea.spot[AREA8].x) / 2;
	centreSpot2.y = (cycloArea.spot[AREA5].y + cycloArea.spot[AREA8].y) / 2;
	
	centreSpot3.x = (cycloArea.spot[AREA5].x + cycloArea.spot[AREA6].x) / 2;
	centreSpot3.y = (cycloArea.spot[AREA5].y + cycloArea.spot[AREA6].y) / 2;
	
	centreSpot4.x = (cycloArea.spot[AREA4].x + cycloArea.spot[AREA5].x) / 2;
	centreSpot4.y = (cycloArea.spot[AREA4].y + cycloArea.spot[AREA5].y) / 2;
	
	if(abs(systemControl.ballPosition[0] - cycloArea.spot[AREA5].x) < ALLOW_ERR * 2 &&
		abs(systemControl.ballPosition[1] - cycloArea.spot[AREA5].y) < ALLOW_ERR * 2
		){
		if(cut > 2){
			state ++;
			if(cut1 < 2){
				if(state > 3){
					state = 0;
					cut1 ++;
				}
				cut = 0;
			}
		}
		if(cut1 >= 2){
			state = 4;
		}
	}
	switch(state){
		case 0:
			drawCircle(centreSpot1, 0, 2.3, 0);
			break;
		case 1:
			drawCircle(centreSpot2, 0, 2.3, 1);
			break;
		case 2:
			drawCircle(centreSpot3, 0, 2.3, 0);
			break;
		case 3:
			drawCircle(centreSpot4, 0, 2.3, 1);
			break;
		case 4:
			moveToSpot(cycloArea.spot[AREA5]);
			break;
	}
}
//
// 坐标校准函数
// 读取中心点坐标 实时校准其他各点
//
void coordinateCalibretion(void){
	spot_t err = {0, 0};
	
	if(systemControl.haveCalCooldinate == false){
		systemControl.centrePosition[0] = systemControl.ballPosition[0];
		systemControl.centrePosition[1] = systemControl.ballPosition[1];
		err.x = systemControl.centrePosition[0] - cycloArea.spot[AREA5].x;
		err.y = systemControl.centrePosition[1] - cycloArea.spot[AREA5].y;
		if(err.x != 0 && err.y != 0){
			// 校准
			for(int i = AREA1; i < AREA9 + 1; i++){
				cycloArea.spot[i].x += err.x;
				cycloArea.spot[i].y += err.y;
			}	
		}
	}
	systemControl.haveCalCooldinate = true;
}

//
// 调整水平的角度初值
// 
//
void adjustZeroAngle(void){
	
}
void workProcess(void){
	switch (systemControl.item){
		case ITEM1:  
			break;
		case ITEM2:
			item2Process();
			break;
		case ITEM3:
			item3Process();
			break;
		case ITEM4:
			item4Process();
			break;
		case ITEM5:
			item5process();
			break;
			
		case ITEM6:
			item6process();
			break;
		case TEST:
			moveToSpot(cycloArea.spot[AREA1]);
			break;
		case ITEM7:
			item7Process();
			break;
		case ITEM8:
			item8Process();
			break;
		
	}
}
//
// 准备
//
void prepareProcess(void){
	motoSpeedUpdata(0, 0);
	motoSpeedUpdata(0, 1);
}
//
// 停止
//
void stopProcess(void){
	motoSpeedUpdata(0, 0);
	motoSpeedUpdata(0, 1);
}
//
// 运动控制
//
//
void ballMoveCtrl(void){
	// 几种状态分别控制
	switch(systemControl.state){
		case CALIBATIONING:
			// 校准处理 校准完后 转为准备状态
			calibrationProcess();
			break;
		case WORKING:
			// 工作状态
			workProcess();
			break;
		case PREPAREING:
			// 准备状态
			prepareProcess();
			break;
		case STOP:
			// 停止运行
			stopProcess();
			break;
	}
}





