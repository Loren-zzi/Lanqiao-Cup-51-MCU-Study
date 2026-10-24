#include <STC15F2K60S2.H>
#include "intrins.h"

void Delay500ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 23;
	j = 205;
	k = 120;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void LED_Run()
{
	unsigned char i;
	for(i=0;i<=3;i++)
	{
		P2=((P2&0x1f)|0x80);
		P0=0x00;
		P2&=0x1f;
		Delay500ms();
		P2=((P2&0x1f)|0x80);
		P0=0xff;
		P2&=0x1f;
		Delay500ms();
	}
	for(i=0;i<=8;i++)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xfe<<i;
		P2&=0x1f;
		Delay500ms();
	}
	for(i=0;i<=8;i++)
	{
		P2=((P2&0x1f)|0x80);
		P0=~(0xfe<<i);
		P2&=0x1f;
		Delay500ms();
	}
}

void main()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	while(1)
	{
		LED_Run();
	}
}