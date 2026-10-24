#include <STC15F2K60S2.H>
#include "intrins.h"

sbit Key1=P3^3;
sbit Key2=P3^2;
sbit Key3=P3^1;
sbit Key4=P3^0;

bit S7,S6;

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


void main()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2=0x1f;
	while(1)
	{
//模式选择		
		P2=((P2&0x1f)|0x80);
		P0=0xff;
		P2=0x1f;
		if(Key4==0)
		{
			Delay20ms();
			while(Key4==0);
			Delay20ms();
			P2=((P2&0x1f)|0x80);
			P0=0xfe;
			P2=0x1f;
			S7=1;
			S6=0;
		}
		if(Key3==0)
		{
			Delay20ms();
			while(Key3==0);
			Delay20ms();
			P2=((P2&0x1f)|0x80);
			P0=0xfd;
			P2=0x1f;
			S7=0;
			S6=1;
		}
//S7
		while(S7)
		{
			if(Key4==0)
			{
				Delay20ms();
				while(Key4==0);
				Delay20ms();
				S7=0;
			}
			if(Key2==0)
			{
				Delay20ms();
				while(Key2==0)
				{
					P2=((P2&0x1f)|0x80);
					P0&=0xfb;
					P2=0x1f;
				}
				Delay20ms();
				P2=((P2&0x1f)|0x80);
				P0|=0xfe;
				P2=0x1f;
			}
			if(Key1==0)
			{
				Delay20ms();
				while(Key1==0)
				{
					P2=((P2&0x1f)|0x80);
					P0&=0xf7;
					P2=0x1f;
				}
				Delay20ms();
				P2=((P2&0x1f)|0x80);
				P0|=0xfe;
				P2=0x1f;
			}
		}
//S6
		while(S6)
		{
			if(Key3==0)
			{
				Delay20ms();
				while(Key3==0);
				Delay20ms();
				S6=0;
			}
			if(Key2==0)
			{
				Delay20ms();
				while(Key2==0)
				{
					P2=((P2&0x1f)|0x80);
					P0&=0xef;
					P2=0x1f;
				}
				Delay20ms();
				P2=((P2&0x1f)|0x80);
				P0|=0xfd;
				P2=0x1f;
			}
			if(Key1==0)
			{
				Delay20ms();
				while(Key1==0)
				{
					P2=((P2&0x1f)|0x80);
					P0&=0xdf;
					P2=0x1f;
				}
				Delay20ms();
				P2=((P2&0x1f)|0x80);
				P0|=0xfd;
				P2=0x1f;
			}
		}
	}
}