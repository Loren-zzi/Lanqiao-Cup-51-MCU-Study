#include <STC15F2K60S2.H>
#include "intrins.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

sbit TX=P1^0;
sbit RX=P1^1;

int Time,x;

char SJ[20];

unsigned char MkeyNum=0;

bit CSBStart=0,CSB=1;

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
	if(P32==0)
	Mkey=5;
	return Mkey;
}

void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(LastState==0&&NowState==5)
	MkeyNum=5;
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
	static unsigned char Count0,Count1;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		Mkey_Loop();
	}
	if(++Count1>=50)
	{
		Count1=0;
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

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 5:
		{
			MkeyNum=0;
			if(CSB)
			CSB=0;
			else
			CSB=1;
		}break;
	}
}

void main()
{
	Init();
	Timer1Init();
	Timer0Init();
	while(1)
	{
		Mkey_Select();
		if(CSBStart)
		{
			CSBStart=0;
			CSB_Loop();
		}
		if(CSB)
		{
			Temp[5]=x/100;
			Temp[6]=x%100/10;
			Temp[7]=x%10;
		}
		else
		{
			Temp[5]=11;
			Temp[6]=11;
			Temp[7]=11;	
		}
		
	}
}
