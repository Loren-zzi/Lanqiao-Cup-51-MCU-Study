#include <STC15F2K60S2.H>
#include "Delay.h"
#include "CBuzz.h"

char Num=0;
sbit Key5=P3^2;
sbit Key4=P3^3;
void main()
{
	Buzz_Close();
	P2&=0x1f;
	P2|=0x80;
	P0=~0x01; //第一个LED亮
	while(1)
  {
		if(Key4==0)//只使用按位左移
		{
			Delay(20);
			while(Key4==0);
			Delay(30);
			Num++; 
			if(Num==8)
			{
				Num=0;
			}
			P0=~(0x01<<Num);
		}
		if(Key5==0)
		{
			Delay(20);
			while(Key5==0);
			Delay(30);
			if(Num==0)
			{
				Num=7;
			}
			else
			{
				Num--;
			}
			P0=~(0x01<<Num); 
		}
	}
}
	
/*
if(Key4==0)//按位左移和按位右移都用
	{
		Delay(20);
		while(Key4==0);
		Delay(30);
		if(Num==8)
		{
			Num=0;
		}
		P2&=0x1f;
		P2|=0x80;
		P0=~(0x01<<Num); 
		P2&=0x1f;
		Num++;
		if(Num==8)
		{
			Num=0;
		}
	}
	if(Key5==0)
	{
		Delay(20);
		while(Key5==0);
		Delay(30);
		if(P0==0xfe)
		{
			P2&=0x1f;
			P2|=0x80;
			P0=0x7f; 
			P2&=0x1f;
			Num=9;
		}
		else
		{
			P2&=0x1f;
			P2|=0x80;
			P0=~((~P0)>>1); 
			P2&=0x1f;
		}	
		Num--;
	}	
*/
