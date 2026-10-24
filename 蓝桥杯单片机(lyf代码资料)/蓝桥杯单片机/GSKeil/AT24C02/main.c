#include <STC15F2K60S2.H>
#include "iic.h"

typedef unsigned char u8;
typedef unsigned int u16;
code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0x7f,0xff};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char KeyNum,AT24C02Data;

bit KeyStart;

void Delay5ms()		//@12.000MHz
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
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
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
	Bit=0;
}

u8 Key_State()
{
	u8 Key=0;
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
	static u8 NowState,LastState;
	LastState=NowState;
	NowState=Key_State();
	if(NowState==1&&LastState==0)
	KeyNum=1;
	if(NowState==2&&LastState==0)
	KeyNum=2;
	if(NowState==3&&LastState==0)
	KeyNum=3;
	if(NowState==4&&LastState==0)
	KeyNum=4;
}

void AT24C02_Write(unsigned char Command,Data)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(Command);
	I2CWaitAck();
	I2CSendByte(Data);
	I2CWaitAck();
	I2CStop();
}

unsigned char AT24C02_Read(unsigned char Command)
{
	unsigned char Data;
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(Command);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0xa0|0x01);
	I2CWaitAck();
	Data=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return Data;
}

void Timer0Init(void)		//2??@12.000MHz
{
	AUXR |= 0x80;		//?????1T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x40;		//??????
	TH0 = 0xA2;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	EA=1;
	ET0=1;
}

void Timer0_Loop() interrupt 1
{
	static u8 Count0;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		KeyStart=1;
	}
}

void Key_Select()
{
	switch(KeyNum)
	{
		case 1:
		{
			KeyNum=0;
			Temp[0]=1;	
			AT24C02_Write(1,1);
			Delay5ms();
			AT24C02Data=AT24C02_Read(1);
		}break;
		case 2:
		{
			KeyNum=0;
			Temp[1]=2;	
			AT24C02_Write(2,102);
			Delay5ms();
			AT24C02Data=AT24C02_Read(2);
		}break;
		case 3:
		{
			KeyNum=0;
			Temp[2]=3;
			AT24C02_Write(3,103);
			Delay5ms();
			AT24C02Data=AT24C02_Read(3);
			
		}break;
		case 4:
		{
			KeyNum=0;
			Temp[3]=4;	
			AT24C02_Write(4,104);
			Delay5ms();
			AT24C02Data=AT24C02_Read(4);
		}break;
	}
}

void main()
{
	Init();
	Timer0Init();
	while(1)
	{
		if(KeyStart)
		{
			KeyStart=0;
			Key_Loop();
		}
		Key_Select();
		Temp[5]=AT24C02Data/100;
		Temp[6]=AT24C02Data%100/10;
		Temp[7]=AT24C02Data%10;
	}
}
