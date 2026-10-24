#include <STC15F2K60S2.H>

sbit I2C_SCL=P2^0; //时序
sbit I2C_SDA=P2^1; //数据位

void I2C_Start()//开始
{
	I2C_SDA=1;//这两行确保S的提示信号在先发送再接收数据帧的中间时候，I2C_SDA I2C_SCL都为高电平
	I2C_SCL=1;//确保开始条件可以触发 如果S在开头，这两行代码也不影响，因为有上拉电阻，上电自动置1
	
	I2C_SDA=0;//在SCL高电平期间，SDA拉低 起始条件
	I2C_SCL=0;//为了后面发送地址
}

void I2C_Stop()//停止
{
	I2C_SDA=0;//
	I2C_SCL=1;//保证SDA为低电平 SCL为高电平 确保停止条件可以触发
	I2C_SDA=1;//在SCL高电平期间，SDA拉高 停止条件
}

void I2C_SendByte(unsigned char Byte)//发送字节1+2
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		I2C_SDA=Byte&(0x80>>i);//1
		I2C_SCL=1;//2
		I2C_SCL=0;//2只要高电平时间大于0.6us就可以读到，而单片机高低电平翻转一次需要1us，所以不用延时
	}
}
	
unsigned char I2C_ReceiveByte()//接收字节2夹1
{
	unsigned char Byte,i;
	Byte=0x00;
	I2C_SDA=1;//主机释放总线，便于从机发数据
	for(i=0;i<8;i++)
	{
		I2C_SCL=1;//2
		if(I2C_SDA)
		{
			Byte|=(0x80>>i);//1
		}
		I2C_SCL=0;//2
	}
	return Byte;
}

void I2C_SendAck(unsigned char ACKBit)//发送应答：主机接收到数据后，发送应答 0为收到 1+2
{
	I2C_SDA=ACKBit;
	I2C_SCL=1;
	I2C_SCL=0;
}

unsigned char I2C_ReceiveAck()//接收应答：从机接收到主机的数据后，发送应答  0为收到 2夹1
{
	unsigned char ACKBit;
	I2C_SDA=1;//释放总线，便于从机应答
	I2C_SCL=1;//2
	ACKBit=I2C_SDA;//1
	I2C_SCL=0;//2
	return ACKBit;
}



