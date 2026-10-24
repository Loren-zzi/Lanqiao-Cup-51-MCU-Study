#include <STC15F2K60S2.H>
#include "ds1302.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};
unsigned Time[]={0x22,0x01,0x26,0x15,0x38,0x50,0x05};
code unsigned TimeWCommand[]={0x8c,0x88,0x86,0x84,0x82,0x80,0x8A};
code unsigned TimeRCommand[]={0x8D,0x89,0x87,0x85,0x83,0x81,0x8B};
#define WRITE 0x8E

void DS1302_Set()
{
	unsigned char i;
	Write_Ds1302(WRITE,0x00);
	for(i=0;i<=6;i++)
	Write_Ds1302(TimeWCommand[i],Time[i]);
	Write_Ds1302(WRITE,0x80);
}


void DS1302_Read()
{
	unsigned char i;
	for(i=0;i<=6;i++)
	Time[i]=Read_Ds1302(TimeRCommand[i]);
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
	if(++Bit==8)
	Bit=0;
}

void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
}

void Timer0_Loop() interrupt 1
{
	static unsigned int Count0;
	if(++Count0==2)
	{
		Count0=0;
		Nixie();
	}
}

void main()
{
	Timer0Init();
	DS1302_Set();
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	while(1)
	{
		DS1302_Read();
		Temp[0]=Time[3]/16;
		Temp[1]=Time[3]%16;
		Temp[2]=10;
		Temp[3]=Time[4]/16;
		Temp[4]=Time[4]%16;
		Temp[5]=10;
		Temp[6]=Time[5]/16;
		Temp[7]=Time[5]%16;
	}
}