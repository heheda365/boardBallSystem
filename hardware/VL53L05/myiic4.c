#include "myiic4.h"
#include "delay.h"

void IIC4_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOE, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_10|GPIO_Pin_11); 	
}
void IIC4_Start(void)
{
	SDA4_OUT(); 
	IIC4_SDA=1;	  	  
	IIC4_SCL=1;
	delay_us(4);
 	IIC4_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC4_SCL=0;
}	  

void IIC4_Stop(void)
{
	SDA4_OUT();
	IIC4_SCL=0;
	IIC4_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC4_SCL=1; 
	IIC4_SDA=1;
	delay_us(4);							   	
}

u8 IIC4_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA4_IN();     
	IIC4_SDA=1;delay_us(1);	   
	IIC4_SCL=1;delay_us(1);	 
	while(READ4_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC4_Stop();
			return 1;
		}
	}
	IIC4_SCL=0;  
	return 0;  
} 

void IIC4_Ack(void)
{
	IIC4_SCL=0;
	SDA4_OUT();
	IIC4_SDA=0;
	delay_us(2);
	IIC4_SCL=1;
	delay_us(2);
	IIC4_SCL=0;
}
    
void IIC4_NAck(void)
{
	IIC4_SCL=0;
	SDA4_OUT();
	IIC4_SDA=1;
	delay_us(2);
	IIC4_SCL=1;
	delay_us(2);
	IIC4_SCL=0;
}					 				     
	  
void IIC4_Send_Byte(u8 txd)
{                        
  u8 t;   
	SDA4_OUT(); 	    
  IIC4_SCL=0;
  for(t=0;t<8;t++)
  {              
       
	 if((txd&0x80)>>7)
		  IIC4_SDA=1;
	 else
		  IIC4_SDA=0;
		txd<<=1; 	  
		delay_us(2);   
		IIC4_SCL=1;
		delay_us(2); 
		IIC4_SCL=0;	
		delay_us(2);
    }	 
} 	    
  
u8 IIC4_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA4_IN();
    for(i=0;i<8;i++ )
	{
      IIC4_SCL=0; 
      delay_us(2);
		  IIC4_SCL=1;
      receive<<=1;
      if(READ4_SDA)
				receive++;   
		  delay_us(1); 
  }					 
    if (!ack)
        IIC4_NAck();
    else
        IIC4_Ack(); 
    return receive;
}

























