#include "delay.h"

#include "myiic1.h"
#include "myiic2.h"
#include "myiic3.h"
#include "myiic4.h"
#include "myiic5.h"

#include "VL53L01.h"
#include "VL53L02.h"
#include "VL53L03.h"
#include "VL53L04.h"
#include "VL53L05.h"
#include "laser.h"

#define VL53L0X_REG_IDENTIFICATION_MODEL_ID         0xc0
#define VL53L0X_REG_IDENTIFICATION_REVISION_ID      0xc2
#define VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD   0x50
#define VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x70
#define VL53L0X_REG_SYSRANGE_START                  0x00
#define VL53L0X_REG_RESULT_INTERRUPT_STATUS         0x13
#define VL53L0X_REG_RESULT_RANGE_STATUS             0x14
#define VL53L0X_Add 0x29

uint16_t bswap(u8 b[])
{
	uint16_t val = ((b[0]<< 8) & b[1]);
	return val;
}



uint16_t VL53L0X_decode_vcsel_period(short vcsel_period_reg)
{
	uint16_t vcsel_period_pclks = (vcsel_period_reg + 1) <<1;
	return vcsel_period_pclks;
}

uint16_t makeuint16(int lsb, int msb)
{
    return ((msb & 0xFF) << 8) | (lsb & 0xFF);
}

uint16_t counting(u8 gbuf[16])
{
	 uint16_t count[3] = {0};
   count[0] = makeuint16(gbuf[7], gbuf[6]);
	 count[1] = makeuint16(gbuf[9], gbuf[8]);
	 count[2] = makeuint16(gbuf[11], gbuf[10]);
	 return(count[2]);
}

uint16_t readLaser1(void)
{
	uint8_t gbuf[16] = {0};
	u8 val = 0;
	uint8_t cnt = 0;
     VL53L01X_Write_Byte(VL53L0X_REG_SYSRANGE_START, 0x01);	
		 while(cnt < 100)
		 {
				val = VL53L01X_Read_Byte(VL53L0X_REG_RESULT_RANGE_STATUS);
				if( val & 0x01) break;
				cnt++;
		 }
		 VL53L01X_Read_Len(VL53L0X_Add, 0x14 , 12, gbuf);  
		 
		 return counting(gbuf);
}

uint16_t readLaser2(void)
{
	uint8_t gbuf[16] = {0};
	u8 val = 0;
	uint8_t cnt = 0;
     VL53L02X_Write_Byte(VL53L0X_REG_SYSRANGE_START, 0x01);
		 while(cnt < 100)
		 {
				val = VL53L02X_Read_Byte(VL53L0X_REG_RESULT_RANGE_STATUS);
				if( val & 0x01) break;
				cnt++;
		 }
		 VL53L02X_Read_Len(VL53L0X_Add, 0x14 , 12, gbuf); 
		 
		 
		 return counting(gbuf);
}

uint16_t readLaser3(void)
{
	uint8_t gbuf[16] = {0};
	u8 val = 0;
	uint8_t cnt = 0;
     VL53L03X_Write_Byte(VL53L0X_REG_SYSRANGE_START, 0x01);
		 while(cnt < 100)
		 {
				val = VL53L03X_Read_Byte(VL53L0X_REG_RESULT_RANGE_STATUS);
				if( val & 0x01) break;
				cnt++;
		 }
		 VL53L03X_Read_Len(VL53L0X_Add, 0x14 , 12, gbuf); 
		 return counting(gbuf);
}

uint16_t readLaser4(void)
{
	uint8_t gbuf[16] = {0};
	u8 val = 0;
	uint8_t cnt = 0;
     VL53L04X_Write_Byte(VL53L0X_REG_SYSRANGE_START, 0x01);
		 while(cnt < 100)
		 {
				val = VL53L04X_Read_Byte(VL53L0X_REG_RESULT_RANGE_STATUS);
				if( val & 0x01) break;
				cnt++;
		 }
		 VL53L04X_Read_Len(VL53L0X_Add, 0x14 , 12, gbuf); 
		 return counting(gbuf);
}

uint16_t readLaser5(void)
{
	uint8_t gbuf[16] = {0};
	u8 val = 0;
	uint8_t cnt = 0;
     VL53L05X_Write_Byte(VL53L0X_REG_SYSRANGE_START, 0x01);
		 while(cnt < 100)
		 {
				
				val = VL53L05X_Read_Byte(VL53L0X_REG_RESULT_RANGE_STATUS);
				if( val & 0x01) break;
				cnt++;
		 }
		 VL53L05X_Read_Len(VL53L0X_Add, 0x14 , 12, gbuf);
		 
		  return counting(gbuf);
}

void initAllLaser(void){
	IIC1_Init();
 	IIC2_Init();
//	IIC3_Init();
	IIC4_Init();
	IIC5_Init();
}
void readAllLaser(uint16_t * laserData){
	laserData[0] = readLaser1();
	laserData[1] = readLaser2();
//	laserData[2] = readLaser3();
	laserData[3] = readLaser4();
	laserData[4] = readLaser5();
}


