#include <STC15F2K60S2.H>
#include "intrins.h"


code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,
													0x8e,0x89,0x88,0x8c};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char KeyNum;

unsigned int x,t,FData;


sbit TX=P1^0;
sbit RX=P1^1;

bit KeyStart,CSBStart,CSBJMStart=1,PLJMStart,CSBSY=1,PLSY;

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
	if(NowState==1&&LastState==0)
	KeyNum=1;
	if(NowState==2&&LastState==0)
	KeyNum=2;
	if(NowState==3&&LastState==0)
	KeyNum=3;
	if(NowState==4&&LastState==0)
	KeyNum=4;
}

void Timer0Init_CSB(void)		//65536??@12.000MHz
{
	AUXR &= 0x7F;		//?????12T??
	TMOD &= 0xF0;		//???????
	TMOD |= 0x01;		//???????
	TL0 = 0x00;		//??????
	TH0 = 0x00;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 0;		//???0????
}

void Timer0Init_PL(void)		//65536??@12.000MHz
{
	AUXR &= 0x7F;		//?????12T??
	TMOD &= 0xF0;		//???????
	TMOD |= 0x05;		//???????
	TL0 = 0x00;		//??????
	TH0 = 0x00;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
}


void Timer1Init(void)		//200??@12.000MHz
{
	AUXR |= 0x40;		//?????1T??
	TMOD &= 0x0F;		//???????
	TL1 = 0xA0;		//??????
	TH1 = 0xF6;		//??????
	TF1 = 0;		//??TF1??
	TR1 = 1;		//???1????
	EA=1;
	ET1=1;
}

void Timer1_Loop() interrupt 3
{
	static unsigned char Count0,Count1;
	static unsigned int Count2,Count3;
	if(++Count0>=10)
	{
		Count0=0;
		Nixie();
	}
	if(++Count1>=100)
	{
		Count1=0;
		KeyStart=1;
	}
	if(CSBSY)
	{
		if(++Count2>=1500)
		{
			Count2=0;
			CSBStart=1;
		}
	}
	if(PLSY)
	{
		if(++Count3>=2500)
		{
			Count3=0;
			FData=TH0<<8|TL0;
			FData*=2;
			TH0=0;
			TL0=0;
			PLSY=0;
			CSBSY=1;
			Timer0Init_CSB();
		}
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
	TR0=1;
	while(RX==1&&TF0==0);
	TR0=0;
	if(TF0)
	{
		TF0=0;
		x=999;
	}
	else
	{
		RX=1;
		t=TH0<<8|TL0;
		x=t*0.017;
	}
	TH0=0;
	TL0=0;
}

void CSBJM_Loop()
{
		Temp[3]=21;
		Temp[4]=21;

		Temp[5]=x/100;
		Temp[6]=x%100/10;
		Temp[7]=x%10;
}

void PLJM_Loop()
{
		Temp[3]=FData/10000;
		Temp[4]=FData%10000/1000;
		Temp[5]=FData%10000%1000/100;
		Temp[6]=FData%10000%1000%100/10;
		Temp[7]=FData%10;
}


void Key_Select()
{
	switch(KeyNum)
	{
		case 1:
		{
			KeyNum=0;
			if(CSBJMStart)
			{
				CSBJMStart=0;
				PLJMStart=1;
			}
			else
			{
				CSBJMStart=1;
				PLJMStart=0;
			}
		}break;
		case 2:
		{
			KeyNum=0;
		}break;
		case 3:
		{
			KeyNum=0;
		}break;
		case 4:
		{
			KeyNum=0;
		}break;
	}
}

void main()
{
	Init();
	//Timer0Init_PL();
	Timer1Init();
	Timer0Init_CSB();
	while(1)
	{
		if(KeyStart)
		{
			KeyStart=0;
			Key_Loop();
		}
		if(CSBStart)
		{
			CSBStart=0;
			CSB_Loop();
			CSBSY=0;
			PLSY=1;
			Timer0Init_PL();
		}
		
		Key_Select();
		if(CSBJMStart)
		CSBJM_Loop();
		if(PLJMStart)
		PLJM_Loop();
	}
}
