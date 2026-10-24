#include <STC15F2K60S2.H>
#include "iic.h"


/*    实现掉电储存数据           */

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff};//0~9 A~F 
unsigned char Temp[]={20,20,20,20,20,20,20,20};

unsigned char KeyNum,AT24C02Data,ADCData;

unsigned int UData;

void Delay5ms()		//@12.000MHz      //写周期
{
	unsigned char i, j;

	i = 59;
	j = 90;
	do
	{
		while (--j);
	} while (--i);
}


void Init()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
}

void Nixie()
{
	static unsigned char Bit;
	P2=((P2&0x1f)|0xe0);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&0x1f)|0xc0);
	P0=0x01<<Bit;
	P2&=0x1f;
	P2=((P2&0x1f)|0xe0);
	P0=Seg[Temp[Bit]];
	P2&=0x1f;
	if(++Bit>=8)
	{
		Bit=0;
	}
}

unsigned char Key_State()
{
	unsigned char Key=0;
	if(P33==0)
	Key=1;
	if(P32==0)
	Key=2;
	if(P31==0)
	Key=3;
	if(P30==0)
	Key=4;
	return Key;
}

void Key_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Key_State();
	if(LastState==1&&NowState==0)
	KeyNum=1;
	if(LastState==2&&NowState==0)
	KeyNum=2;
	if(LastState==3&&NowState==0)
	KeyNum=3;
	if(LastState==4&&NowState==0)
	KeyNum=4;
}

void Timer0Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x40;		//设置定时初值
	TH0 = 0xA2;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	EA=1;
	ET0=1;
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count0;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		Key_Loop();
	}
}
//unsigned char Command,Data
void AT24C02_Write(unsigned char address,unsigned char Data)//发写等发等发等停
{
//	IIC_Start();
//	IIC_SendByte(0xa0);    //写入模式
//	IIC_WaitAck();
//	IIC_SendByte(Command);
//	IIC_WaitAck();
//	IIC_SendByte(Data);
//	IIC_WaitAck();
//	IIC_Stop();
		IIC_Start();
	IIC_SendByte(0xa0);           //¿ªÆôÐ´Ä£Ê½
	IIC_WaitAck();
	IIC_SendByte(address);
	IIC_WaitAck();
	IIC_SendByte(Data);
	IIC_WaitAck();
	IIC_Stop();
}
//unsigned Command
unsigned char AT24C02_Read(unsigned char address)
{
//	unsigned char Data;
//	IIC_Start();
//	IIC_SendByte(0xa0);           //¿ªÆôÐ´Ä£Ê½
//	IIC_WaitAck();
//	IIC_SendByte(address);
//	IIC_WaitAck();
//	IIC_Start();
//	IIC_SendByte(0xa1);            //¿ªÆô¶ÁÄ£Ê½
//	IIC_WaitAck();
//	Data=IIC_RecByte();
//	IIC_SendByte(1);
//	IIC_Stop();
//	return Data;	
	unsigned char Data;
	IIC_Start();
	IIC_SendByte(0xa0);           //¿ªÆôÐ´Ä£Ê½
	IIC_WaitAck();
	IIC_SendByte(address);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0xa1);            //¿ªÆô¶ÁÄ£Ê½
	IIC_WaitAck();
	Data=IIC_RecByte();
	IIC_SendByte(1);
	IIC_Stop();
	return Data;
}

unsigned char ADC_Read()
{
	unsigned char Data;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0x90|0x01);
	IIC_WaitAck();
	Data=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return Data;
}

void main()
{
	Timer0Init();
	Init();
	AT24C02_Write(1,5);
	Delay5ms();
	AT24C02_Write(2,7);
//	AT24C02_Write(3,7);
//	AT24C02_Write(4,5);
	Delay5ms();
	AT24C02Data=AT24C02_Read(1);
	Temp[1]=AT24C02Data;
	AT24C02Data=AT24C02_Read(2);
	Temp[2]=AT24C02Data;
//	AT24C02Data=AT24C02_Read(4);
//	Temp[3]=AT24C02Data;
	while(1)
	{		
//		AT24C02Data=AT24C02_Read(1);
//		Temp[0]=AT24C02Data;
//		if(KeyNum==1)
//		Temp[0]=8;
//		Temp[2]=AT24C02Data;
//		ADCData=ADC_Read();
//		UData=ADCData/255.0;
//		Temp[5]=UData/100+10;
//		Temp[6]=UData%100/10;
//		Temp[7]=UData%100%10;
	}
}

