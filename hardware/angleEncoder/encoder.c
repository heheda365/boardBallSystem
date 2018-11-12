#include "encoder.h"
#include "delay.h"

uint16_t m_iPrvSSI;
uint16_t iSSI = 0;
uint16_t inc_data;	   //每刷新一次的增量值

void encoder_gpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
												
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 // 上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //| GPIO_Pin_2 | GPIO_Pin_3;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void getdata(void)   //根据时序读取传感器数据
{	
	char bCrc = 0,uState=0;
	int a,ix,iSSI = 0,ire = 0;
	ENCOLD_CS = 0;
	delay_us(4);
	ENCOLD_CLK = 0;
	delay_us(4);
	ENCOLD_CLK = 1;
	delay_us(4);
	
	for(a=0;a<10;a++)
	{
		ENCOLD_CLK = 0;
		iSSI<<=1;
		if(ENCOLD_DATA)
		{
			iSSI|=1;
			bCrc=!bCrc;
		}
		ENCOLD_CLK = 1;
		delay_us(4);
    }
	 delay_us(8);
	 for(ix = 0; ix < 5;ix++)//读5次状态位标志!
	 {
	 	ENCOLD_CLK = 0;
		uState <<= 1; //后五位的数据状态位
		if(ENCOLD_DATA)
		{
			bCrc = !bCrc;//数位中“1”的个数是奇数或偶数来进行校验
 			uState |= 1;//数据等于1置1
		}
		ENCOLD_CLK = 1;
		delay_us(4);
	 }
	 delay_us(8);
	
	 
	 if(ENCOLD_DATA == bCrc)//最后一位校验一下,要是奇偶通不过,就认为CRC错误 ,就当这次无效				
	 {	
	 	ENCOLD_CS = 1;//使能CSN置高,为下次准备
	 	if((uState == 16)||(uState == 19))//;///&&(uState == uPrvState))	//16=10000;19=10011  表示状态位(5位uState) 都可以理解为数据正常
		{
			if((iSSI < 256)&&(m_iPrvSSI > 768))	//                                      	iSSI(0-1023)是这次读出的数据   	m_iPrvSSI(0-1023)上一次读出的数据
			{
				ire = iSSI - m_iPrvSSI + 1024;	//     第一种情况
			}
			else if((m_iPrvSSI < 256)&&(iSSI > 768)) 
			{
				ire = iSSI - m_iPrvSSI - 1024;	//     第二种情况
			}
			else
			{
				ire = iSSI - m_iPrvSSI;//		       第三种情况	  新数据减去老数据
			}
			m_iPrvSSI = iSSI;//	刷新数据,新数据替换老数据 ,为下一次做好准备
			inc_data=ire;    
		}
   	}
	ENCOLD_CS = 1;//使能CSN置高,为下次准备
}


uint16_t getEncoldData(void){
	getdata();
	return m_iPrvSSI;
}



