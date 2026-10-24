// I2C 用 SDA 传数据、SCL 给时钟；一次访问由起始、地址、数据和停止组成。

#include "iic.h"
//#include <STC15F2K60S2.h>

#define DELAY_TIME 5

//I2C总线内部延时函数
// I2C 通信需要短暂停顿，保证时钟和数据线满足时序要求。
void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}

//I2C总线启动信号
// SCL 为高时把 SDA 从高拉低，表示一次 I2C 传输开始。
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//I2C总线停止信号
// SCL 为高时把 SDA 从低拉高，表示一次 I2C 传输结束。
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//发送应答或非应答信号
// 读完数据后回复应答位；0 为继续读取，1 为结束读取。
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//等待应答
// 发送字节后读取外设的应答位，确认它是否响应。
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//I2C总线发送一个字节数据
// 把一个字节的 8 位依次放到 SDA，并用 SCL 时钟送出。
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//I2C总线接收一个字节数据
// 逐位采样 SDA，拼成从外设读回的一个字节。
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}
