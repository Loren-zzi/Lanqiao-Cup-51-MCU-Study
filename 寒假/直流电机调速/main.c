#include <REGX52.H>
#include "Delay.h"
#include "Key.h"
#include "Nixie.h"
#include "Timer0.h"

sbit led1=P2^0;
sbit led2=P2^1;

unsigned char counter,compare1,compare2;
unsigned char keynum;

void main()
{
			unsigned char speed=0;
			compare1=50;
			compare2=5;
			Timer0_Init();
      while(1)
        {
					keynum=Key();
					if(keynum==1)
					{
						speed++;
						speed%=4;
						if(speed==0){compare1=95;}
						if(speed==1){compare1=60;}
						if(speed==2){compare1=30;}
						if(speed==3){compare1=5;}
					}
							
        }

}

void Timer0_Routine() interrupt 1
{

	TL0 = 0x9C;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	counter++;
	if(counter<compare1)
	{
		led1=0;
	}
	
	if(counter<compare1)
	{
		led1=1;
	}
	
			if(counter<compare2)
	{
		led2=0;
	}
	
		if(counter<compare2)
	{
		led2=1;
	}
	if(counter==100)
	{counter=0;}




}