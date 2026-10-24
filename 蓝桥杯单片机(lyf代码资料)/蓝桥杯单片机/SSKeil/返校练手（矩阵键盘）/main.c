#include <STC15F2K60S2.H>

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

unsigned char MkeyNum;

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
	{
		Bit=0;
	}
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
	if(LastState==1&&NowState==0)
	MkeyNum=1;
	if(LastState==2&&NowState==0)
	MkeyNum=2;
	if(LastState==3&&NowState==0)
	MkeyNum=3;
	if(LastState==4&&NowState==0)
	MkeyNum=4;
	if(LastState==5&&NowState==0)
	MkeyNum=5;
	if(LastState==6&&NowState==0)
	MkeyNum=6;
	if(LastState==7&&NowState==0)
	MkeyNum=7;
	if(LastState==8&&NowState==0)
	MkeyNum=8;
	if(LastState==9&&NowState==0)
	MkeyNum=9;
	if(LastState==10&&NowState==0)
	MkeyNum=10;
	if(LastState==11&&NowState==0)
	MkeyNum=11;
	if(LastState==12&&NowState==0)
	MkeyNum=12;
	if(LastState==13&&NowState==0)
	MkeyNum=13;
	if(LastState==14&&NowState==0)
	MkeyNum=14;
	if(LastState==15&&NowState==0)
	MkeyNum=15;
	if(LastState==16&&NowState==0)
	MkeyNum=16;
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

void main()
{
	Timer0Init();
	Init();
	while(1)
	{
		if(MkeyNum==1)
		Temp[0]=8;
	}
}

