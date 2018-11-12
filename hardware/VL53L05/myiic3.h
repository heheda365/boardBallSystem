#ifndef __MYIIC3_H
#define __MYIIC3_H
#include "sys.h" 

   	   		   
//IO????
#define SDA3_IN()  {GPIOD->CRH&=0XFFFF0FFF;GPIOD->CRH|=8<<12;}
#define SDA3_OUT() {GPIOD->CRH&=0XFFFF0FFF;GPIOD->CRH|=3<<12;}

//IO????	 
#define IIC3_SCL    PDout(10) //SCL
#define IIC3_SDA    PDout(11) //SDA	 
#define READ3_SDA   PDin(11)  //??SDA 

//IIC所有操作函数
void IIC3_Init(void);                //初始化IIC的IO口				 
void IIC3_Start(void);				//发送IIC开始信号
void IIC3_Stop(void);	  			//发送IIC停止信号
void IIC3_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC3_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC3_Wait_Ack(void); 				//IIC等待ACK信号
void IIC3_Ack(void);					//IIC发送ACK信号
void IIC3_NAck(void);				//IIC不发送ACK信号

void IIC3_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC3_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















