#ifndef __MYIIC4_H
#define __MYIIC4_H
#include "sys.h" 

   	   		   
//IO????
#define SDA4_IN()  {GPIOE->CRH&=0XFFFF0FFF;GPIOE->CRH|=8<<12;}
#define SDA4_OUT() {GPIOE->CRH&=0XFFFF0FFF;GPIOE->CRH|=3<<12;}

//IO????	 
#define IIC4_SCL    PEout(10) //SCL
#define IIC4_SDA    PEout(11) //SDA	 
#define READ4_SDA   PEin(11)  //??SDA 

//IIC所有操作函数
void IIC4_Init(void);                //初始化IIC的IO口				 
void IIC4_Start(void);				//发送IIC开始信号
void IIC4_Stop(void);	  			//发送IIC停止信号
void IIC4_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC4_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC4_Wait_Ack(void); 				//IIC等待ACK信号
void IIC4_Ack(void);					//IIC发送ACK信号
void IIC4_NAck(void);				//IIC不发送ACK信号

void IIC4_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC4_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















