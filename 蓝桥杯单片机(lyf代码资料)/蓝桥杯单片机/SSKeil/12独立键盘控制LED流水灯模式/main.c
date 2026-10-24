#include <STC15F2K60S2.H>
#include "intrins.h"


sbit Key_1=P3^3;
sbit Key_2=P3^2;
sbit Key_3=P3^1;
sbit Key_4=P3^0;

unsigned char KeyNum,LED1,LED2,LED3,LED4;

bit Switch1=1,Switch2,Switch3,Switch4;

void Delay1000ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 46;
	j = 153;
	k = 245;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


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
	if(NowState==0&&LastState==1)
	KeyNum=1;
	if(NowState==0&&LastState==2)
	KeyNum=2;
	if(NowState==0&&LastState==3)
	KeyNum=3;
	if(NowState==0&&LastState==4)
	KeyNum=4;
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
	static unsigned char Count0;
	if(++Count0==20)
	{
		Count0=0;
		Key_Loop();
	}
}

void LED_1()
{
	LED2=0;
	LED3=0;
	LED4=0;
	P2=((P2&0x1f)|0x80);
	P0=~(0x01<<LED1);
	P2&=0x1f;
	if(++LED1==8)
	LED1=0;
	Delay1000ms();
}

void LED_2()
{
	LED1=0;
	LED3=0;
	LED4=0;
	P2=((P2&0x1f)|0x80);
	P0=~(0x80>>LED2);
	P2&=0x1f;
	if(++LED2==8)
	LED2=0;
	Delay1000ms();
}

void LED_3()
{
	LED1=0;
	LED2=0;
	LED4=0;
	P2=((P2&0x1f)|0x80);
	P0=0xfe<<LED3;
	P2&=0x1f;
	if(++LED3==8)
	LED3=0;
	Delay1000ms();
}

void LED_4()
{
	LED1=0;
	LED2=0;
	LED3=0;
	P2=((P2&0x1f)|0x80);
	P0=0x7f>>LED4;
	P2&=0x1f;
	if(++LED4==8)
	LED4=0;
	Delay1000ms();
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
//		if(KeyNum==1)
//		{
//			KeyNum=0;
//			Switch1=1;
//			Switch2=0;
//			Switch3=0;
//			Switch4=0;
//		}
//		if(KeyNum==2)
//		{
//			KeyNum=0;
//			Switch1=0;
//			Switch2=1;
//			Switch3=0;
//			Switch4=0;
//		}
//		if(KeyNum==3)
//		{
//			KeyNum=0;
//			Switch1=0;
//			Switch2=0;
//			Switch3=1;
//			Switch4=0;
//		}
//		if(KeyNum==4)
//		{
//			KeyNum=0;
//			Switch1=0;
//			Switch2=0;
//			Switch3=0;
//			Switch4=1;
//		}
		switch(KeyNum)
		{
			case 1:
			{
				KeyNum=0;
				Switch1=1;
				Switch2=0;
				Switch3=0;
				Switch4=0;
			}break;
			case 2:
			{
				KeyNum=0;
				Switch1=0;
				Switch2=1;
				Switch3=0;
				Switch4=0;
			}break;
			case 3:
			{
				KeyNum=0;
				Switch1=0;
				Switch2=0;
				Switch3=1;
				Switch4=0;
			}break;
			case 4:
			{
				KeyNum=0;
				Switch1=0;
				Switch2=0;
				Switch3=0;
				Switch4=1;
			}break;
		}
		
		if(Switch1)
		LED_1();
		if(Switch2)
		LED_2();
		if(Switch3)
		LED_3();
		if(Switch4)
		LED_4();
	}
}

