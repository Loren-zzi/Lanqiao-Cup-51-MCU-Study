#include "STC15F2K60S2.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF};
unsigned char Temp[]={10,10,10,10,10,1,2,3};

unsigned char Bit;

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
	P2=0x1f;
	P2=((P2&0x1f)|0xC0);
	P0=0x01<<Bit;
	P2=0x1f;
	P2=((P2&0x1f)|0xE0);
	P0=Seg[Temp[Bit]];
	P2=0x1f;
	if(++Bit==8)
	{
		Bit=0;
	}
}

void main()
{
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2=0x1f;
	while(1)
	{
		display();
		Delay2ms();	
	}
}



