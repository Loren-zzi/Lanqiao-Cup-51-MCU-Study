#include <STC15F2K60S2.H>

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={0,0,10,0,0,10,0,0};
bit MiaoBiao_Start;
sbit Key_1=P3^3;
sbit Key_2=P3^2;
sbit Key_3=P3^1;
sbit Key_4=P3^0;

unsigned char KeyNum,MiniSec,Min,Sec;

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

void MiaoBiao_Loop()
{
	if(MiaoBiao_Start)
	{
		if(++MiniSec==100)
		{
			MiniSec=0;
			if(++Sec==60)
			{
				Sec=0;
				if(++Min==60)
				Min=0;
			}
		}
	}
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
	static unsigned int Count0,Count1,Count2;
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
	if(++Count2==10)
	{
		Count2=0;
		MiaoBiao_Loop();
	}
}

void main()
{
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	Timer0Init();
	while(1)
	{
		switch(KeyNum)
		{
			case 1:
			{
				KeyNum=0;
				MiaoBiao_Start=~MiaoBiao_Start;
			}break;
			case 2:
			{
				KeyNum=0;
				MiniSec=0;
				Sec=0;
				Min=0;
			}break;
		}
		Temp[0]=Min/10;
		Temp[1]=Min%10;
		Temp[3]=Sec/10;
		Temp[4]=Sec%10;
		Temp[6]=MiniSec/10;
		Temp[7]=MiniSec%10;
	}
}