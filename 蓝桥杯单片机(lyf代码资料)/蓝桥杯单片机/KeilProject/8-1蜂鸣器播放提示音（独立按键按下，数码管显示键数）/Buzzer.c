#include <STC15F2K60S2.H>
#include "intrins.h"

sbit Buzzer=P0^6;

unsigned int i;

void Delay500us()		//@11.0592MHz蜂鸣器标准延时
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 6;
	j = 93;
	do
	{
		while (--j);
	} while (--i);
}

void Buzzer_Init(unsigned int xms) //以1000Hz的频率响xms（可以调音调）
{
	for(i=0;i<xms;i++)
	{
		P2&=0x1F;
		P2|=0xA0;
		Buzzer=~Buzzer;
		P2&=0x1F;
		Delay500us();
	}
}

