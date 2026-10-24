#include <STC15F2K60S2.H>
#include "intrins.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xff};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned int x,Time;

bit CSBStart;

sbit TX=P1^0;
sbit RX=P1^1;

void Delay12us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}


void Init()
{
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&0x1f)|0xA0);
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

void Timer1Init(void)		//65536??@12.000MHz
{
	AUXR &= 0xBF;		//?????12T??
	TMOD &= 0x0F;		//???????
	TMOD |= 0x10;		//???????
	TL1 = 0x00;		//??????
	TH1 = 0x00;		//??????
	TF1 = 0;		//??TF1??
	TR1 = 0;		//???1????
	
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
	static unsigned char Count0;
	Nixie();
	if(++Count0>=150)
	{
		Count0=0;
		CSBStart=1;
	}
}

void CSB_Loop()
{
	unsigned char i;
	for(i=0;i<9;i++)
	{
		TX=1;
		Delay12us();
		TX=0;
		Delay12us();
	}
	TR1=1;
	while(RX==1&&TF1==0);
	TR1=0;
	if(TF1)
	{
		TF1=0;
		x=999;
	}
	else
	{
		RX=1;
		Time=TH1<<8|TL1;
		x=Time*0.017;
	}
	TH1=0;
	TL1=0;
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
		Temp[7]=x%10;
	}
}


