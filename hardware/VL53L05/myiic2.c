#include "myiic2.h"
#include "delay.h"

void IIC2_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_10|GPIO_Pin_11); 	
}
void IIC2_Start(void)
{
	SDA2_OUT();	
	IIC2_SDA=1;	  	  
	IIC2_SCL=1;
	delay_us(4);
 	IIC2_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC2_SCL=0;
}	  

void IIC2_Stop(void)
{
	SDA2_OUT();
	IIC2_SCL=0;
	IIC2_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC2_SCL=1; 
	IIC2_SDA=1;
	delay_us(4);							   	
}

u8 IIC2_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA2_IN();     
	IIC2_SDA=1;delay_us(1);	   
	IIC2_SCL=1;delay_us(1);	 
	while(READ2_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC2_Stop();
			return 1;
		}
	}
	IIC2_SCL=0;  
	return 0;  
} 

void IIC2_Ack(void)
{
	IIC2_SCL=0;
	SDA2_OUT();
	IIC2_SDA=0;
	delay_us(2);
	IIC2_SCL=1;
	delay_us(2);
	IIC2_SCL=0;
}
    
void IIC2_NAck(void)
{
	IIC2_SCL=0;
	SDA2_OUT();
	IIC2_SDA=1;
	delay_us(2);
	IIC2_SCL=1;
	delay_us(2);
	IIC2_SCL=0;
}					 				     
	  
void IIC2_Send_Byte(u8 txd)
{                        
  u8 t;   
	SDA2_OUT(); 	    
  IIC2_SCL=0;
  for(t=0;t<8;t++)
  {              
       
	 if((txd&0x80)>>7)
		  IIC2_SDA=1;
	 else
		  IIC2_SDA=0;
		txd<<=1; 	  
		delay_us(2);   
		IIC2_SCL=1;
		delay_us(2); 
		IIC2_SCL=0;	
		delay_us(2);
    }	 
} 	    
  
u8 IIC2_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA2_IN();
    for(i=0;i<8;i++ )
	{
      IIC2_SCL=0; 
      delay_us(2);
		  IIC2_SCL=1;
      receive<<=1;
      if(READ2_SDA)
				receive++;   
		  delay_us(1); 
  }					 
    if (!ack)
        IIC2_NAck();
    else
        IIC2_Ack(); 
    return receive;
}

























