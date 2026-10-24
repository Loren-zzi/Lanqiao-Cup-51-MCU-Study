#include <STC15F2K60S2.H>

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff};
unsigned char Temp[]={20,20,20,20,20,20,20,20};

unsigned char KeyNum,MkeyNum;

bit KeyStart,MkeyStart;

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

unsigned char Mkey_State()
{
	unsigned char Mkey;
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
	if(LastState==0&NowState==1)
	MkeyNum=1;
	if(LastState==0&NowState==2)
	MkeyNum=2;
	if(LastState==0&NowState==3)
	MkeyNum=3;
	if(LastState==0&NowState==4)
	MkeyNum=4;
	if(LastState==0&NowState==5)
	MkeyNum=5;
	if(LastState==0&NowState==6)
	MkeyNum=6;
	if(LastState==0&NowState==7)
	MkeyNum=7;
	if(LastState==0&NowState==8)
	MkeyNum=8;
	if(LastState==0&NowState==9)
	MkeyNum=9;
	if(LastState==0&NowState==10)
	MkeyNum=10;
	if(LastState==0&NowState==11)
	MkeyNum=11;
	if(LastState==0&NowState==12)
	MkeyNum=12;
	if(LastState==0&NowState==13)
	MkeyNum=13;
	if(LastState==0&NowState==14)
	MkeyNum=14;
	if(LastState==0&NowState==15)
	MkeyNum=15;
	if(LastState==0&NowState==16)
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
		//KeyStart=1;
		MkeyStart=1;
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

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 1:
		{
			MkeyNum=0;
			Temp[0]=1;
		}break;
		case 2:
		{
			MkeyNum=0;
			Temp[1]=2;
		}break;
		case 3:
		{
			MkeyNum=0;
			Temp[2]=3;
		}break;
		case 4:
		{
			MkeyNum=0;
			Temp[3]=4;
		}break;
		case 5:
		{
			MkeyNum=0;
			Temp[4]=5;
		}break;
		case 6:
		{
			MkeyNum=0;
			Temp[5]=6;
		}break;
		case 7:
		{
			MkeyNum=0;
			Temp[6]=7;
		}break;
		case 8:
		{
			MkeyNum=0;
			Temp[7]=8;
		}break;
	}
}

void main()
{
	Init();
	Timer0Init();
	while(1)
	{
//		if(KeyStart)
//		{
//			KeyStart=0;
//			Key_Loop();
//		}
		if(MkeyStart)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		//Key_Select();
		Mkey_Select();
	}
}

