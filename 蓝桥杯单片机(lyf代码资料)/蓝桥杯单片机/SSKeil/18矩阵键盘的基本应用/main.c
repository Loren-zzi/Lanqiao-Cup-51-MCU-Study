#include <STC15F2K60S2.H>

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

sbit List_1=P4^4;
sbit List_2=P4^2;
sbit List_3=P3^5;
sbit List_4=P3^4;
sbit Line_1=P3^3;
sbit Line_2=P3^2;
sbit Line_3=P3^1;
sbit Line_4=P3^0;

unsigned char MkeyNum;

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

unsigned char Mkey_State()
{
	unsigned char Mkey=0;
	List_1=0;
	List_2=1;
	List_3=1;
	List_4=1;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=1;
	if(Line_2==0)
	Mkey=5;
	if(Line_3==0)
	Mkey=9;
	if(Line_4==0)
	Mkey=13;

	List_1=1;
	List_2=0;
	List_3=1;
	List_4=1;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=2;
	if(Line_2==0)
	Mkey=6;
	if(Line_3==0)
	Mkey=10;
	if(Line_4==0)
	Mkey=14;
	
	List_1=1;
	List_2=1;
	List_3=0;
	List_4=1;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=3;
	if(Line_2==0)
	Mkey=7;
	if(Line_3==0)
	Mkey=11;
	if(Line_4==0)
	Mkey=15;
	
	List_1=1;
	List_2=1;
	List_3=1;
	List_4=0;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=4;
	if(Line_2==0)
	Mkey=8;
	if(Line_3==0)
	Mkey=12;
	if(Line_4==0)
	Mkey=16;
	return Mkey;
}

void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(NowState==0&&LastState==1)
	MkeyNum=1;
	if(NowState==0&&LastState==2)
	MkeyNum=2;
	if(NowState==0&&LastState==3)
	MkeyNum=3;
	if(NowState==0&&LastState==4)
	MkeyNum=4;
	if(NowState==0&&LastState==5)
	MkeyNum=5;
	if(NowState==0&&LastState==6)
	MkeyNum=6;
	if(NowState==0&&LastState==7)
	MkeyNum=7;
	if(NowState==0&&LastState==8)
	MkeyNum=8;
	if(NowState==0&&LastState==9)
	MkeyNum=9;
	if(NowState==0&&LastState==10)
	MkeyNum=10;
	if(NowState==0&&LastState==11)
	MkeyNum=11;
	if(NowState==0&&LastState==12)
	MkeyNum=12;
	if(NowState==0&&LastState==13)
	MkeyNum=13;
	if(NowState==0&&LastState==14)
	MkeyNum=14;
	if(NowState==0&&LastState==15)
	MkeyNum=15;
	if(NowState==0&&LastState==16)
	MkeyNum=16;
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
	static unsigned int Count0,Count1;
	if(++Count0==2)
	{
		Count0=0;
		Nixie();
	}
	if(++Count1==20)
	{
		Count1=0;
		Mkey_Loop();
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
		switch(MkeyNum)
		{
			case 1:
			{
				MkeyNum=0;
				Temp[0]=0;
				Temp[1]=1;
			}break;
			case 2:
			{
				MkeyNum=0;
				Temp[0]=0;
				Temp[1]=2;
			}break;
			case 3:
			{
				MkeyNum=0;
				Temp[0]=0;
				Temp[1]=3;
			}break;
			case 4:
			{
				MkeyNum=0;
				Temp[0]=0;
				Temp[1]=4;
			}break;
			case 5:
			{
				MkeyNum=0;
				Temp[0]=0;
				Temp[1]=5;
			}break;
			case 6:
			{
				MkeyNum=0;
				Temp[0]=0;
				Temp[1]=6;
			}break;
			case 7:
			{
				MkeyNum=0;
				Temp[0]=0;
				Temp[1]=7;
			}break;
			case 8:
			{
				MkeyNum=0;
				Temp[0]=0;
				Temp[1]=8;
			}break;
			case 9:
			{
				MkeyNum=0;
				Temp[0]=0;
				Temp[1]=9;
			}break;
			case 10:
			{
				MkeyNum=0;
				Temp[0]=1;
				Temp[1]=0;
			}break;
			case 11:
			{
				MkeyNum=0;
				Temp[0]=1;
				Temp[1]=1;
			}break;
			case 12:
			{
				MkeyNum=0;
				Temp[0]=1;
				Temp[1]=2;
			}break;
			case 13:
			{
				MkeyNum=0;
				Temp[0]=1;
				Temp[1]=3;
			}break;
			case 14:
			{
				MkeyNum=0;
				Temp[0]=1;
				Temp[1]=4;
			}break;
			case 15:
			{
				MkeyNum=0;
				Temp[0]=1;
				Temp[1]=5;
			}break;
			case 16:
			{
				MkeyNum=0;
				Temp[0]=1;
				Temp[1]=6;
			}break;
		}
	}
}