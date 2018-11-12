#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "stm32f10x.h"
#include <stdbool.h>

//#define Debug  //调试与否的条件编译
#define u16 uint16_t
#define SPEED_TO_PWM 18000.0F
#define ITEM1					1
#define	ITEM2					2
#define	ITEM3					3
#define	ITEM4					4
#define	ITEM5					5
#define	ITEM6					6
#define	ITEM7					7
#define	ITEM8					8
#define ADJUST_ZORO_ANGLE		9

#define TEST		13

#define MOTO_TEST	11

#define	CALIBATIONING		0
#define WORKING				1
#define PREPAREING			2
#define STOP				3
#define COORDINATE_CAL		4			// 坐标校准

#define LEVER_ANGLE_PITCH	-0.01
#define LEVER_ANGLE_ROLL	0.11


#define LEVER_ANGLE_THRE	0.05

#define MAX_ANGLE			5

#define MOTO0_DOWN_START_PWM		260
#define MOTO0_UP_START_PWM			340

#define MOTO1_DOWN_START_PWM		170
#define MOTO1_UP_START_PWM			275

#define FRICTION_ANG				0.2     // 0.9还OK 校服球

#define ALLOW_ERR					10		// 允许的到点坐标误差



// P ID结构体
typedef struct
{
    float P;
    float I;
    float D;
    float Desired;
    float Error;
    float PreError;
    float PrePreError;
    float Increment;
    float Integ;
	float iLimit;
    float Deriv;
    float Output;
 
}PID_Typedef;

typedef struct
{
	float 		xCtrl_mea;			// 左边速度 测量
	float 		yCtrl_mea;			//右边速度  测量
	float		xCtrl_exp;			//左边速度  期望
	float		yCtrl_exp;			// 右边速度 期望
	
	float		xCtrlOut;			// 左边速度输出 单位 m/s
	float		yCtrlOut;			// 右边速度输出
	
	float		pitch;				// 角度
	float		roll;				// 角度
	float		zeroPitch;			// 水平角度 
	float		zeroRoll;			// 水平角度
	uint16_t	ballPosition[2];	// 小球的位置 			
	uint16_t	centrePosition[2];	// 板中心位置	
	
	uint16_t  	thro;				// 油门
	bool 		start;				// 开始？
	uint8_t 	item;				// 项目
	uint8_t		state;				// 状态
	bool  		needRst;			// 一些状态初始化
	bool		haveCalCooldinate;	// 是否已经校准坐标
} systemControl_t;

extern systemControl_t systemControl;
extern char pathString[20];
extern uint8_t pathStringCut;

void PID_Init(void);
static void PID_Postion_Cal(PID_Typedef * PID,float target,float measure,int32_t dertT);
void CtrlSpeed(void);
void speedUpdata(void);
void controlDataInit(void);
void getSensorData(void);
void systemStateCtrl(void);
void ballMoveCtrl(void);
void coordinateCalibretion(void);

#endif





