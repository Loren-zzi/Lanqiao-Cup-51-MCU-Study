#include "STC15F2K60S2.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF};
unsigned char Temp[]={10,10,10,10,10,10,0,0};

unsigned char Sec=0;

unsigned int i=0;

unsigned char T;

void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;			//允许定时器0申请中断
	EA=1;				//开启总中断
}

void Timer0_Loop() interrupt 1
{	
	i++;
	if(i==1000)
	{
		i=0;
		Sec++;
		if(Sec==60)
		{
			Sec=0;
		}
	}
}

void Delay2ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 24;
	j = 85;
	do
	{
		while (--j);
	} while (--i);
}

void display()  //段选之后必消隐
{
	P2=((P2&0x1f)|0xE0);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&0x1f)|0xC0);
	P0=0x01<<T;
	P2&=0x1f;
	P2=((P2&0x1f)|0xE0);
	P0=Seg[Temp[T]];
	P2&=0x1f;
	if(++T==8)
	{
		T=0;
	}
}

void main()
{
	Timer0Init();
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	while(1)
	{
		Temp[6]=Sec/10;
		Temp[7]=Sec%10;
		display();
		Delay2ms();	
	}
}



