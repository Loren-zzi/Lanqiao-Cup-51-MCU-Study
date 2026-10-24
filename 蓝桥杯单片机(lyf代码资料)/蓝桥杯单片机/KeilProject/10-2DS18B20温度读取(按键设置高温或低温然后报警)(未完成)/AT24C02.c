#include <STC15F2K60S2.H>
#include "I2C.h"

#define AT24C02_ADDRESS 0xA0

void AT24C02_WriteByte(unsigned char WordAddress,Data)//(字地址：0~255都可，最多256个数据存储)
{																											//，数据）先从机地址然后字地址
	unsigned char ACK;
	I2C_Start();
	I2C_SendByte(AT24C02_ADDRESS);
	ACK=I2C_ReceiveAck();
	if(~ACK)
	{
		I2C_SendByte(WordAddress);
		ACK=I2C_ReceiveAck();
		if(~ACK)
		{
			I2C_SendByte(Data);
			I2C_ReceiveAck();
			if(~ACK)
			I2C_Stop();
		}		
	}
}

unsigned char AT24C02_ReadByte(unsigned char WordAddress)//字地址
{
	unsigned char Data,ACK;
	I2C_Start();
	I2C_SendByte(AT24C02_ADDRESS);
	ACK=I2C_ReceiveAck();
	if(~ACK)
	{
		I2C_SendByte(WordAddress);
		ACK=I2C_ReceiveAck();
		if(~ACK)
		{
			I2C_Start();
			I2C_SendByte(AT24C02_ADDRESS|0x01);
			ACK=I2C_ReceiveAck();
			if(~ACK)
			{
				Data=I2C_ReceiveByte();
				I2C_SendAck(1);
				I2C_Stop();
			}
		}
	}	
	return Data;
}
	