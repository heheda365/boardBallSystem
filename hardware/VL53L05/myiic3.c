#include "myiic3.h"
#include "delay.h"

void IIC3_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOD, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_10|GPIO_Pin_11); 	
}
void IIC3_Start(void)
{
	SDA3_OUT();    
	IIC3_SDA=1;	  	  
	IIC3_SCL=1;
	delay_us(4);
 	IIC3_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC3_SCL=0;
}	  

void IIC3_Stop(void)
{
	SDA3_OUT();
	IIC3_SCL=0;
	IIC3_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC3_SCL=1; 
	IIC3_SDA=1;
	delay_us(4);							   	
}

u8 IIC3_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA3_IN();     
	IIC3_SDA=1;delay_us(1);	   
	IIC3_SCL=1;delay_us(1);	 
	while(READ3_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC3_Stop();
			return 1;
		}
	}
	IIC3_SCL=0;  
	return 0;  
} 

void IIC3_Ack(void)
{
	IIC3_SCL=0;
	SDA3_OUT();
	IIC3_SDA=0;
	delay_us(2);
	IIC3_SCL=1;
	delay_us(2);
	IIC3_SCL=0;
}
    
void IIC3_NAck(void)
{
	IIC3_SCL=0;
	SDA3_OUT();
	IIC3_SDA=1;
	delay_us(2);
	IIC3_SCL=1;
	delay_us(2);
	IIC3_SCL=0;
}					 				     
	  
void IIC3_Send_Byte(u8 txd)
{                        
  u8 t;   
	SDA3_OUT(); 	    
  IIC3_SCL=0;
  for(t=0;t<8;t++)
  {              
       
	 if((txd&0x80)>>7)
		  IIC3_SDA=1;
	 else
		  IIC3_SDA=0;
		txd<<=1; 	  
		delay_us(2);   
		IIC3_SCL=1;
		delay_us(2); 
		IIC3_SCL=0;	
		delay_us(2);
    }	 
} 	    
  
u8 IIC3_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA3_IN();
    for(i=0;i<8;i++ )
	{
      IIC3_SCL=0; 
      delay_us(2);
		  IIC3_SCL=1;
      receive<<=1;
      if(READ3_SDA)
				receive++;   
		  delay_us(1); 
  }					 
    if (!ack)
        IIC3_NAck();
    else
        IIC3_Ack(); 
    return receive;
}

























