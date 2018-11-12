#ifndef _VL53L04_H
#define _VL53L04_H

#include "sys.h"

u8 VL53L04X_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);//连续写
u8 VL53L04X_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);//连续读
u8 VL53L04X_Write_Byte(u8 reg,u8 data);//写一个字节
u8 VL53L04X_Read_Byte(u8 reg);//读一个字节

#endif

