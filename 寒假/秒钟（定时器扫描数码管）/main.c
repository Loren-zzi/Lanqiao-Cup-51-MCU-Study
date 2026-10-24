#include <REGX52.H>
#include "Timer0.h"
#include "Key.h"
#include "Nixie.h"
#include "Delay.h"
#include "AT24C02.h"

void main()
{
	Timer0_Init();
	
      while(1)
        {
	
        }

}


void Timer0_Routine() interrupt 1
{
	static unsigned int T0count;
		T0count++;
		TL0 = 0x18;		
		TH0 = 0xFC;
	if(T0count>=1000)
	{
		T0count=0;
		P2=~P2;
	}
	



}	
