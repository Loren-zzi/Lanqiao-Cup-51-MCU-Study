// I2C 驱动的函数声明；起始、读写和应答的实现见 iic.c。
#ifndef __IIC_H_
#define __IIC_H_

#include "intrins.h"
#include <STC15F2K60S2.h>

sbit sda=P2^1;
sbit scl=P2^0;
void I2CStart(void);
void I2CStop(void);
void I2CSendByte(unsigned char byt);
unsigned char I2CReceiveByte(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(unsigned char ackbit);



#endif

