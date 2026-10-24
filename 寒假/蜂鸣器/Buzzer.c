#include <REGX52.H>
#include "DELAY.h"
sbit Buzzer=P2^5;

void Buzzer_Delay500us()		//500us
{
	unsigned char i, j;

	i = 6;
	j = 211;
	do
	{
		while (--j);
	} while (--i);
}


void Buzzer_Time(unsigned char ms)//响多久
{
						unsigned int i;
						for(i=0;i<ms;i++)
							{
										Buzzer=!Buzzer;
										Buzzer_Delay500us();
							}

}