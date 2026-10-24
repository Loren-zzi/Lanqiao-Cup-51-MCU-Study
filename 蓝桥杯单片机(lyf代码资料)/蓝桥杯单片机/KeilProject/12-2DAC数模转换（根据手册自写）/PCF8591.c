#include <STC15F2K60S2.H>
#include "I2C.h"

#define PCF8591_Address 0x90


//unsigned char PCF8591_ADC(unsigned char Command)
//{
//	//目前只有两个输入端 0x01 0x03
//	unsigned char Data;
//	I2C_Start();
//	I2C_SendByte(PCF8591_Address);
//	I2C_ReceiveAck();
//	I2C_SendByte(Command);  
//	I2C_ReceiveAck();
//	I2C_Start();
//	I2C_SendByte(PCF8591_Address|0x01);
//	I2C_ReceiveAck();
//	Data=I2C_ReceiveByte();
//	I2C_SendAck(1);
//	I2C_Stop();
//	return Data;
//}

void PCF8591_DAC(unsigned char Data)
{
	//Out_Put 0x40
	I2C_Start();
	I2C_SendByte(PCF8591_Address);
	I2C_ReceiveAck();
	I2C_SendByte(0x40);  
	I2C_ReceiveAck();
	I2C_SendByte(Data);  
	I2C_ReceiveAck();
	I2C_Stop();
}

