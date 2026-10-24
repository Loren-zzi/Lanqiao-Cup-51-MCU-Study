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


void Init_IN0()
{
	IT0=1;
	EA=1;
	EX0=1;
	
}

void IN0_Loop() interrupt 0
{
	P2=((P2&0x1f)|0x80);
	P0&=0x7f;
	P2&=0x1f;
	Delay500ms();
	Delay500ms();
	Delay500ms();
	Delay500ms();
	P2=((P2&0x1f)|0x80);
	P0|=0x80;
	P2&=0x1f;
}

void LED()
{
	P2=((P2&0x1f)|0x80);
	P0=0xfe;
	P2&=0x1f;
	Delay500ms();
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	Delay500ms();
}

void main()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2=0x1f;
	Init_IN0();
	while(1)
	{
		LED();
	}
}