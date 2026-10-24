#include <STC15F2K60S2.H>
#include "CLED.h"
#include "CBuzz.h"
#include "Nixie.h"
#include "Timer0.h"
#include "DS18B20.h"
#include "Delay.h"
#include "AT24C02.h"
#include "Matrixkey.h"

float T;//浮点数和整数之间转换，看视频
unsigned char Bit3,Bit4,Bit5,Bit6,Bit7,Bit8;
unsigned char KeyNum,Temp=1;
void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count1=0;//,T0Count2=0;
	TH0=(65536-917)/256;		//重装初值
	TL0=(65536-917)%256;		
	T0Count1++;
	if(T0Count1>=2)
	{
		//T0Count1=0;//先去掉，我也不知道为什么
		Nixie_Loop();
	}
//	T0Count2++;
//	if(T0Count2>=21)
//	{
//		T0Count2=0;
//		MKey_Loop();
//	}
	
}

void main()
{
	LED_Close();
	Buzz_Close();
	Timer0_Init();
	DS18B20_ConvertT();		//上电先转换一次温度，防止第一次读数据错误
	Delay(1000);					//等待转换完成
	while(1)
	{
		KeyNum=MKey();
		//判断哪个键按下，卡循环
		if(KeyNum)
		{
			Temp=KeyNum;
		}
		//温度读取Key1
		if(Temp==1)
		{
			DS18B20_ConvertT();
			T=DS18B20_ReadT();
			if(T>0)
			{
				Nixie(1,1);			
			}
			else
			{
				T=-T;
				Nixie(1,0);
			}
			Bit3=(unsigned int)(T)/10;  //unsigned int类型强制转换使得浮点数以十进制整数去掉小数表示
			Bit4=(unsigned int)(T)%10+11;
			Bit5=(unsigned long)(T*10000)%10000/1000;
			Bit6=(unsigned long)(T*10000)%10000/100%10;
			Bit7=(unsigned long)(T*10000)%10000/10%100%10;
			Bit8=(unsigned long)(T*10000)%10000%1000%100%10;
			Nixie(3,Bit3);
			Nixie(4,Bit4);
			Nixie(5,Bit5);
			Nixie(6,Bit6);
			Nixie(7,Bit7);
			Nixie(8,Bit8);
		}
			
		//温度阈值Key2
		if(Temp==2)
		{
			P2&=0x1f;
			P2|=0x80;
			P0=0x00;  
			P2&=0x1f;
		}
	}
}


