#include <STC15F2K60S2.H>
#include "Delay.h"

sbit Key7=P3^0;
sbit Key6=P3^1;
sbit Key5=P3^2;
sbit Key4=P3^3;
unsigned char Key()
{
	unsigned char KeyNumber=0;
	if(Key4==0)
	{
		Delay(20);
		while(Key4==0);
		Delay(30);
		KeyNumber=4;
	}
	if(Key5==0)
	{
		Delay(20);
		while(Key5==0);
		Delay(30);
		KeyNumber=5;
	}
	if(Key6==0)
	{
		Delay(20);
		while(Key6==0);
		Delay(30);
		KeyNumber=6;
	}
	if(Key7==0)
	{
		Delay(20);
		while(Key7==0);
		Delay(30);
		KeyNumber=7;
	}
	return KeyNumber;
}
