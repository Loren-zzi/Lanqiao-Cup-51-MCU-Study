// I2C 驱动的函数声明；起始、读写和应答的实现见 iic.c。
#ifndef __IIC_H_
#define __IIC_H_

#include <STC15F2K60S2.h>
#include <intrins.h>

sbit SDA = P2^1;
sbit SCL = P2^0;

void IIC_Start(void);
void IIC_Stop(void);
void IIC_SendAck(bit ackbit);
bit IIC_WaitAck(void);
void IIC_SendByte(unsigned char byt);
unsigned char IIC_RecByte(void);

#endif




