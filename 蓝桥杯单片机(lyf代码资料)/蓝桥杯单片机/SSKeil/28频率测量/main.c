#include <STC15F2K60S2.H>

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={5,11,11,11,11,11,11,11};

unsigned int Count,Data;

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

void Timer0Init(void)		//1微秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x06;		//设置定时器模式
	TL0 = 0xFF;		//设置定时初值
	TH0 = 0xFF;		//设置定时重载值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
}

void Timer1Init(void)		//2毫秒@12.000MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x30;		//设置定时初值
	TH1 = 0xF8;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;
	EA=1;
}

void Timer0_Loop() interrupt 1
{
	Count++;
}

void Timer1_Loop() interrupt 3
{
	static unsigned int Count1;
	Nixie();
	if(++Count1==500)
	{
		Count1=0;
		Data=Count;
		Count=0;
	}
}

void Data_XianShi()
{
		if(Data>=10000)
		{
			Temp[3]=Data/10000;
			Temp[4]=Data%10000/1000;
			Temp[5]=Data%10000%1000/100;
			Temp[6]=Data%10000%1000%100/10;
			Temp[7]=Data%10000%1000%100%10;
		}
		if(Data<10000&&Data>=1000)
		{
			Temp[3]=11;
			Temp[4]=Data%10000/1000;
			Temp[5]=Data%10000%1000/100;
			Temp[6]=Data%10000%1000%100/10;
			Temp[7]=Data%10000%1000%100%10;
		}	
		if(Data<1000&&Data>=100)
		{
			Temp[3]=11;
			Temp[4]=11;
			Temp[5]=Data%10000%1000/100;
			Temp[6]=Data%10000%1000%100/10;
			Temp[7]=Data%10000%1000%100%10;
		}	
		if(Data<100&&Data>=10)
		{
			Temp[3]=11;
			Temp[4]=11;
			Temp[5]=11;
			Temp[6]=Data%10000%1000%100/10;
			Temp[7]=Data%10000%1000%100%10;
		}	
		if(Data<10)
		{
			Temp[3]=11;
			Temp[4]=11;
			Temp[5]=11;
			Temp[6]=11;
			Temp[7]=Data%10000%1000%100%10;
		}	
}

void main()
{
	Timer0Init();
	Timer1Init();
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	while(1)
	{
		Data_XianShi();
	}
}