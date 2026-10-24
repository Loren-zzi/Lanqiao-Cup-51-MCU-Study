#include "STC15F2K60S2.h"
#include "intrins.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,0};

unsigned char KeyNum,Bit,Count0,T=0;

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

void display()  //段选之后必消隐
{
	P2=((P2&0x1f)|0xE0);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&0x1f)|0xC0);
	P0=0x01<<Bit;
	P2&=0x1f;
	P2=((P2&0x1f)|0xE0);
	P0=Seg[Temp[Bit]];
	P2&=0x1f;
	if(++Bit==8)
	{
		Bit=0;
	}
}

void Timer0_Loop() interrupt 1
{	
	Count0++;
	if(Count0>=2)
	{
		Count0=0;
		display();
	}		
}



void Delay20ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 1;
	j = 234;
	k = 113;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

unsigned char Key()
{
	unsigned char K=0xff;
	unsigned char Temp;
	Temp=P3&0x0f;
	switch(Temp)
	{
		case 0x07:
		K=1;
			break;
		case 0x0b:
		K=2;
			break;
		case 0x0d:
			K=3;
		break;
		case 0x0e:
			K=4;
		break;
	}
	return K;
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
		KeyNum=Key();
		if(KeyNum!=0xff)
		{
			Delay20ms();
			if(KeyNum!=0xff)
			{
				if(KeyNum==1)
				{
					T++;
					if(T==100)
					{
						T=0;
					}
					if(T/10==0)
					{
						Temp[7]=T%10;
						Temp[6]=11;
					}
					if(T/10>0)
					{
						Temp[7]=T%10;
						Temp[6]=T/10;
					}
				}					
				while(P3!=0xff);
			}
		}			
	}
}



