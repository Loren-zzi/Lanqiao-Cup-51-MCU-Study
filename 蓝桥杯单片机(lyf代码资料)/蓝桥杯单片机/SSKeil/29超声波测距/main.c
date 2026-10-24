#include <STC15F2K60S2.H>
#include "intrins.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

unsigned int x,Time;

sbit TX=P1^0;
sbit RX=P1^1;

bit CSBStart;

void Delay12us()		//@12.000MHz//固定超声波12us
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
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
	if(++Bit==8)
	Bit=0;
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
void Timer1Init(void)		//@12.000MHz//12T模式
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x10;		//设置定时器模式
	TL1 = 0x00;		//设置定时初值
	TH1 = 0x00;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 0;		
}

void CSB_Loop()
{
	unsigned char i;
	for(i=0;i<=8;i++)//发送9个脉冲
	{
		TX=1;
		Delay12us();
		TX=0;
		Delay12us();
	}
	TR1=1;
	while(RX==1&&TF1==0);//RX=1表示没有收到，收到则RX=0；
	TR1=0;
	if(TF1==1)
	{
		TF1=0;
		x=999;
	}
	else
	{
		Time=(TH1<<8)|TL1;
		x=Time*0.017;//距离等于时间*0.017；
	}
	TH1=0;
	TL1=0;
}

void Timer0_Loop() interrupt 1
{
	static unsigned int Count0;
	Nixie();
	if(++Count0>=50)//100ms测一次，不要太快
	{
		Count0=0;
		CSBStart=1;
	}
}


void main()
{
	Init();
	Timer0Init();
	Timer1Init();
	while(1)
	{
		if(CSBStart)
		{
			CSBStart=0;
			CSB_Loop();
		}
		Temp[5]=x/100;
		Temp[6]=x%100/10;
		Temp[7]=x%100%10;
	}
}
