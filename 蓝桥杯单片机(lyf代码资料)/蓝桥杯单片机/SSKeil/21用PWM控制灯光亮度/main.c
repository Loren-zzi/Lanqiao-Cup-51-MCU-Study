#include <STC15F2K60S2.H>

sbit Key_1=P3^3;
sbit Key_2=P3^2;
sbit Key_3=P3^1;
sbit Key_4=P3^0;

unsigned char KeyNum,Count,Num,Compare,Speed;

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

void Timer0Init(void)		//100微秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x9C;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count1;
	if(++Count1==200)
	{
		Count1=0;
		Key_Loop();
	}
	if(Count<Compare)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xfe;
		P2=0x1f;
	}
	if(Count>=Compare)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xff;
		P2=0x1f;
	}
	Count++;
	Count%=100;
}

void main()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2=0x1f;
	Timer0Init();
	while(1)
	{
		switch(KeyNum)
		{
			case 1:
			{
				KeyNum=0;
				Num++;
				Num%=4;
				Speed=Num;
			}break;
		}
		switch(Speed)
		{
			case 0:
			{
				Compare=0;
			}break;
			case 1:
			{
				Compare=10;
			}break;
			case 2:
			{
				Compare=50;
			}break;
			case 3:
			{
				Compare=90;
			}break;
		}
	}
}