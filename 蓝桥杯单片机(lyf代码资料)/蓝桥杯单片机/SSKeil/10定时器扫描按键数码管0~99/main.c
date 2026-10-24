#include "STC15F2K60S2.h"
#include "intrins.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

unsigned char	Count,KeyNum;

sbit Key_1=P3^3;
sbit Key_2=P3^2;
sbit Key_3=P3^1;
sbit Key_4=P3^0;

unsigned char Key_State()
{
	unsigned char Key=0;
	if(Key_1==0)
	Key=1;
	if(Key_2==0)
	Key=2;
	if(Key_3==0)
	Key=3;
	if(Key_4==0)
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

void Nixie()
{
	static unsigned char Bit;
	P2=((P2&=0x1f)|0xe0);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&=0x1f)|0xc0);
	P0=0x01<<Bit;
	P2&=0x1f;
	P2=((P2&=0x1f)|0xe0);
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
	static unsigned char Count0,Count1;
	if(++Count0==2)
	{
		Count0=0;
		Nixie();
	}
	if(++Count1==20)
	{
		Count1=0;
		Key_Loop();
	}
}

void main()
{
	Timer0Init();
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	while(1)
	{
		if(KeyNum==1)
		{
			KeyNum=0;
			Count++;
		}
		Count%=100;
		if(Count==100)
		Count=0;
		if(Count/10==0)
		{
			Temp[7]=Count%10;
			Temp[6]=11;
		}
		if(Count/10!=0)
		{
			Temp[7]=Count%10;
			Temp[6]=Count/10;
		}
	}
}



