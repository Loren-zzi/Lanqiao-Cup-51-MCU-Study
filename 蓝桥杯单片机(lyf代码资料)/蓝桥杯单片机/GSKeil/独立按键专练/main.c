#include <STC15F2K60S2.H>

typedef unsigned char u8;
typedef unsigned int u16;
code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0x86};
unsigned char Temp[]={20,20,20,20,20,20,20,20};

unsigned char KeyNum;

bit KeyStart;

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
			Temp[0]=21;	
		}break;
		case 2:
		{
			KeyNum=0;
			Temp[1]=2;	
		}break;
		case 3:
		{
			KeyNum=0;
			Temp[2]=3;	
		}break;
		case 4:
		{
			KeyNum=0;
			Temp[3]=4;	
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
	}
}
