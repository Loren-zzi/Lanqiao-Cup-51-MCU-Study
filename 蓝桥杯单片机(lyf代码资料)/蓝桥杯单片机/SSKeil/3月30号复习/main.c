#include <STC15F2K60S2.H>
#include "iic.h"
#include "ds1302.h"

#define WP 0x8e

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xff};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

code unsigned char TimeWCommand[]={0x8c,0x88,0x86,0x84,0x82,0x80,0x8a};

code unsigned char TimeRCommand[]={0x8d,0x89,0x87,0x85,0x83,0x81,0x8b};

unsigned char Time[]={0x23,0x03,0x31,0x11,0x42,0x00,0x05};

unsigned char MkeyNum,ADCData;

unsigned int UData;

bit RQ=1,SJ=0;

//void Delay5ms()		//@12.000MHz
//{
//	unsigned char i, j;

//	i = 59;
//	j = 90;
//	do
//	{
//		while (--j);
//	} while (--i);
//}


void Init()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
}

void Time_Set()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<7;i++)
	Write_Ds1302_Byte(TimeWCommand[i],Time[i]);
	Write_Ds1302_Byte(WP,0x80);
}

void Time_Read()
{
	unsigned char i;
	for(i=0;i<7;i++)
	Time[i]=Read_Ds1302_Byte(TimeRCommand[i]);
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

unsigned char Mkey_State()
{
	unsigned char Mkey=0;
	P44=0;
	P42=1;
	P35=1;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=1;
	if(P32==0)
	Mkey=5;
	if(P31==0)
	Mkey=9;
	if(P30==0)
	Mkey=13;
	
	P44=1;
	P42=0;
	P35=1;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=2;
	if(P32==0)
	Mkey=6;
	if(P31==0)
	Mkey=10;
	if(P30==0)
	Mkey=14;

	P44=1;
	P42=1;
	P35=0;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=3;
	if(P32==0)
	Mkey=7;
	if(P31==0)
	Mkey=11;
	if(P30==0)
	Mkey=15;

	P44=1;
	P42=1;
	P35=1;
	P34=0;
	P3|=0x0f;
	if(P33==0)
	Mkey=4;
	if(P32==0)
	Mkey=8;
	if(P31==0)
	Mkey=12;
	if(P30==0)
	Mkey=16;
	
	return Mkey;
}

void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(NowState==0&&LastState==1)
	MkeyNum=1;
	if(NowState==0&&LastState==2)
	MkeyNum=2;
	if(NowState==0&&LastState==3)
	MkeyNum=3;
	if(NowState==0&&LastState==4)
	MkeyNum=4;
	if(NowState==0&&LastState==5)
	MkeyNum=5;
	if(NowState==0&&LastState==6)
	MkeyNum=6;
	if(NowState==0&&LastState==7)
	MkeyNum=7;
	if(NowState==0&&LastState==8)
	MkeyNum=8;
	if(NowState==0&&LastState==9)
	MkeyNum=9;
	if(NowState==0&&LastState==10)
	MkeyNum=10;
	if(NowState==0&&LastState==11)
	MkeyNum=11;
	if(NowState==0&&LastState==12)
	MkeyNum=12;
	if(NowState==0&&LastState==13)
	MkeyNum=13;
	if(NowState==0&&LastState==14)
	MkeyNum=14;
	if(NowState==0&&LastState==15)
	MkeyNum=15;
	if(NowState==0&&LastState==16)
	MkeyNum=16;
	
}
	
void Timer0Init(void)		//2??@12.000MHz
{
	AUXR |= 0x80;		//?????1T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x40;		//??????
	TH0 = 0xA2;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	ET0=1;
	EA=1;
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count0;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		Mkey_Loop();
	}
}

unsigned char ADC_Read()
{
	unsigned char Data;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x03);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0x90|0x01);
	I2CWaitAck();
	Data=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return Data;
}

void main()
{
	Init();
	Timer0Init();
	Time_Set();
	ADCData=ADC_Read();
	while(1)
	{
		Time_Read();
		switch(MkeyNum)
		{
			case 2:
			{
				MkeyNum=0;
				RQ=1;
				SJ=0;
			}break;
			case 3:
			{
				MkeyNum=0;
				RQ=0;
				SJ=1;
			}break;
		}
		if(RQ)
		{
			Temp[0]=Time[0]/16;
			Temp[1]=Time[0]%16;
			Temp[2]=20;
			Temp[3]=Time[1]/16;
			Temp[4]=Time[1]%16;
			Temp[5]=20;
			Temp[6]=Time[2]/16;
			Temp[7]=Time[2]%16;
		}
		if(SJ)
		{
			Temp[0]=Time[3]/16;
			Temp[1]=Time[3]%16;
			Temp[2]=20;
			Temp[3]=Time[4]/16;
			Temp[4]=Time[4]%16;
			Temp[5]=20;
			Temp[6]=Time[5]/16;
			Temp[7]=Time[5]%16;
		}
	}
}

