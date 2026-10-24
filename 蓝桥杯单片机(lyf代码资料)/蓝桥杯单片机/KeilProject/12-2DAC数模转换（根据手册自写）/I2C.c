#include <STC15F2K60S2.H>
#include "intrins.h"

sbit I2C_SCL=P2^0; //时钟线
sbit I2C_SDA=P2^1; //数据位

void Delay5us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	i = 11;
	while (--i);
}

void I2C_Start()//开始
{
	I2C_SDA=1;//这两行确保S的提示信号在先发送再接收数据帧的中间时候，I2C_SDA I2C_SCL都为高电平
	I2C_SCL=1;//确保开始条件可以触发 如果S在开头，这两行代码也不影响，因为有上拉电阻，上电自动置1
	Delay5us();//释放总线不低于4.7us
	I2C_SDA=0;//在SCL高电平期间，SDA拉低 起始条件
	I2C_SCL=0;//为了后面发送地址
	Delay5us();
}

void I2C_Stop()//停止
{
	I2C_SDA=0;//
	I2C_SCL=1;//保证SDA为低电平 SCL为高电平 确保停止条件可以触发
	Delay5us();//要求这个时间大于4us
	I2C_SDA=1;//在SCL高电平期间，SDA拉高 停止条件
}

void I2C_SendByte(unsigned char Byte)//发送字节1+2
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		I2C_SDA=Byte&(0x80>>i);//1
		I2C_SCL=1;//2//高电平持续时间大于4us
		Delay5us();
		I2C_SCL=0;//2低电平持续时间时间大于4us
		Delay5us();
	}
	
}
	
unsigned char I2C_ReceiveByte()//接收字节2夹1
{
	unsigned char Byte,i;
	Byte=0x00;
	I2C_SDA=1;//主机释放总线，便于从机发数据
	Delay5us();
	for(i=0;i<8;i++)
	{
		I2C_SCL=1;//2
		Delay5us();
		if(I2C_SDA)
		{
			Byte|=(0x80>>i);//1
		}
		I2C_SCL=0;//2
		Delay5us();
	}
	return Byte;
}

void I2C_SendAck(unsigned char ACKBit)//发送应答：主机接收到数据后，发送应答 0为收到 1+2
{
	I2C_SDA=ACKBit;
	I2C_SCL=1;
	Delay5us();   //传输数据High
	I2C_SCL=0;
	Delay5us();
}

unsigned char I2C_ReceiveAck()//接收应答：从机接收到主机的数据后，发送应答  0为收到 2夹1
{
	unsigned char ACKBit;
	I2C_SDA=1;//释放总线，便于从机应答
	Delay5us();
	I2C_SCL=1;//2
	Delay5us();
	ACKBit=I2C_SDA;//1
	I2C_SCL=0;//2
	Delay5us();
	return ACKBit;
}



