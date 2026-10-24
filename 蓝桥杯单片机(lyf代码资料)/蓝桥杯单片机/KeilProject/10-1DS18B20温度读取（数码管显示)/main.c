#include <STC15F2K60S2.H>
#include "CLED.h"
#include "CBuzz.h"
#include "Nixie.h"
#include "Timer0.h"
#include "DS18B20.h"
#include "Delay.h"

float T;//浮点数和整数之间转换，看视频
unsigned char Bit3,Bit4,Bit5,Bit6,Bit7,Bit8,MkeyNum;
sbit Last_1=P4^4;
sbit Last_2=P4^2;
sbit Last_3=P3^5;
sbit Last_4=P3^4;
sbit Line_1=P3^3;
sbit Line_2=P3^2;
sbit Line_3=P3^1;
sbit Line_4=P3^0;

void DS18B20()
{
	DS18B20_ConvertT(); 
	Delay(1000);					//等待转换完成
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


void Timer0_Routine() interrupt 1
{
	static unsigned int Count0;
	TH0=0xf8;		//重装初值
	TL0=0xcd;
	if(++Count0==1)
	{		
		Count0=0;
		Nixie_Loop();
	}
}

void main()
{
	LED_Close();
	Buzz_Close();
	Timer0_Init();
	while(1)
	{
		DS18B20();
	}
}


