#include <STC15F2K60S2.H>
#include "CLED.h"
#include "CBuzz.h"
#include "AT24C02.h"
#include "Nixie.h"
#include "Delay.h"
#include "Timer0.h"

sbit Key_1=P3^3;
sbit Key_2=P3^2;
sbit Key_3=P3^1;
sbit Key_4=P3^0;

unsigned char Data,KeyNum,Min,Sec,MiniSec,Start;

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

void Sec_Loop()
{
	if(Start)
	{
		MiniSec++;
		if(MiniSec==100)
		{
			MiniSec=0;
			Sec++;
			if(Sec==60)
			{
				Sec=0;
				Min++;
				if(Min==100)
				{
					Min=0;
				}
			}
		}
	}
}

void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count1=0,T0Count2=0,T0Count3=0;
	TH0=(65536-917)/256;		//重装初值
	TL0=(65536-917)%256;		
	T0Count1++;
	T0Count2++;
	T0Count3++;
	if(T0Count1>=21)
	{
		T0Count1=0;
		Key_Loop();
	}
	if(T0Count2>=2)
	{
		T0Count2=0;
		Nixie_Loop();
	}
	if(T0Count3>=10)
	{
		T0Count3=0;
		Sec_Loop();
	}
}

void main()
{
	Timer0_Init();
	LED_Close();
	Buzz_Close();
	while(1)
	{
		if(KeyNum==1)
		{
			KeyNum=0;
			Start=!Start;
		}
		if(KeyNum==2)
		{
			KeyNum=0;
			Min=0;
			Sec=0;
			MiniSec=0;
		}
		if(KeyNum==3)
		{
			KeyNum=0;
			AT24C02_WriteByte(0,Min);
			Delay(5);//写周期最长5ms，要给芯片时间先擦除再写入，写完之后立刻读会出错
			AT24C02_WriteByte(1,Sec);
			Delay(5);//写周期最长5ms，要给芯片时间先擦除再写入
			AT24C02_WriteByte(2,MiniSec);
			Delay(5);//写周期最长5ms，要给芯片时间先擦除再写入
		}
		if(KeyNum==4)
		{
			KeyNum=0;
			Data=AT24C02_ReadByte(0);
			Min=Data;
			Data=AT24C02_ReadByte(1);
			Sec=Data;
			Data=AT24C02_ReadByte(2);
			MiniSec=Data;
		}
		Nixie(1,Min/10);
		Nixie(2,Min%10);
		Nixie(3,11);
		Nixie(4,Sec/10);
		Nixie(5,Sec%10);
		Nixie(6,11);
		Nixie(7,MiniSec/10);
		Nixie(8,MiniSec%10);
	}
}	
