#ifndef __MYIIC5_H
#define __MYIIC5_H

#include "sys.h" 

   	   		   
//IO????
#define SDA5_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
#define SDA5_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}

//IO????	 
#define IIC5_SCL    PGout(10) //SCL
#define IIC5_SDA    PGout(11) //SDA	 
#define READ5_SDA   PGin(11)  //??SDA 

//IIC所有操作函数
void IIC5_Init(void);                //初始化IIC的IO口				 
void IIC5_Start(void);				//发送IIC开始信号
void IIC5_Stop(void);	  			//发送IIC停止信号
void IIC5_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC5_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC5_Wait_Ack(void); 				//IIC等待ACK信号
void IIC5_Ack(void);					//IIC发送ACK信号
void IIC5_NAck(void);				//IIC不发送ACK信号

void IIC5_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC5_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















