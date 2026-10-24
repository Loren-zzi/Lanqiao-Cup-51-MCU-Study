#include "STC15F2K60S2.h"
#include "intrins.h"

unsigned char KeyNum;

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

void Key_Play(unsigned char Num)
{
	switch(Num)
	{
		case 1:
			P2=((P2&0x1f)|0x80);
			P0=~(0x01);
			P2&=0x1f;
		break;
		case 2:
			P2=((P2&0x1f)|0x80);
			P0=~(0x02);
			P2&=0x1f;
		break;
		case 3:
			P2=((P2&0x1f)|0x80);
			P0=~(0x04);
			P2&=0x1f;
		break;
		case 4:
			P2=((P2&0x1f)|0x80);
			P0=~(0x08);
			P2&=0x1f;
		break;
	}
}
	
void main()
{
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
				Key_Play(KeyNum);
				while(P3!=0xff);
			}
		}			
	}
}



