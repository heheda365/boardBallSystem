#include "myiic5.h"
#include "delay.h"

void IIC5_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOG, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG,GPIO_Pin_10|GPIO_Pin_11); 	
}
void IIC5_Start(void)
{
	SDA5_OUT();    
	IIC5_SDA=1;	  	  
	IIC5_SCL=1;
	delay_us(4);
 	IIC5_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC5_SCL=0;
}	  

void IIC5_Stop(void)
{
	SDA5_OUT();
	IIC5_SCL=0;
	IIC5_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC5_SCL=1; 
	IIC5_SDA=1;
	delay_us(4);							   	
}

u8 IIC5_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA5_IN();     
	IIC5_SDA=1;delay_us(1);	   
	IIC5_SCL=1;delay_us(1);	 
	while(READ5_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC5_Stop();
			return 1;
		}
	}
	IIC5_SCL=0;  
	return 0;  
} 

void IIC5_Ack(void)
{
	IIC5_SCL=0;
	SDA5_OUT();
	IIC5_SDA=0;
	delay_us(2);
	IIC5_SCL=1;
	delay_us(2);
	IIC5_SCL=0;
}
    
void IIC5_NAck(void)
{
	IIC5_SCL=0;
	SDA5_OUT();
	IIC5_SDA=1;
	delay_us(2);
	IIC5_SCL=1;
	delay_us(2);
	IIC5_SCL=0;
}					 				     
	  
void IIC5_Send_Byte(u8 txd)
{                        
  u8 t;   
	SDA5_OUT(); 	    
  IIC5_SCL=0;
  for(t=0;t<8;t++)
  {              
       
	 if((txd&0x80)>>7)
		  IIC5_SDA=1;
	 else
		  IIC5_SDA=0;
		txd<<=1; 	  
		delay_us(2);   
		IIC5_SCL=1;
		delay_us(2); 
		IIC5_SCL=0;	
		delay_us(2);
    }	 
} 	    
  
u8 IIC5_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA5_IN();
    for(i=0;i<8;i++ )
	{
      IIC5_SCL=0; 
      delay_us(2);
		  IIC5_SCL=1;
      receive<<=1;
      if(READ5_SDA)
				receive++;   
		  delay_us(1); 
  }					 
    if (!ack)
        IIC5_NAck();
    else
        IIC5_Ack(); 
    return receive;
}

























