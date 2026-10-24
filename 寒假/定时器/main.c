#include <REGX52.H>
#include "Timer0.h"
#include "Key.h"
#include <INTRINS.H>

unsigned char Keynum,led;
void main()
{
			Timer0Init();
			P2=0xfe;
      while(1)
        {
						Keynum=Key();
						if(Keynum)
						{
							if(Keynum==1)
							{
									led++;if(led>=2){led=0;}//确保led只有01俩个状态
								
							}
						}
        }

}


void Timer0_Routine() interrupt 1
{
	static unsigned int T0count;
		T0count++;
		TL0 = 0x18;		
		TH0 = 0xFC;
	if(T0count>=500)
	{
		T0count=0;
			if(led==0){P2=_crol_(P2,1);}
			if(led==1){P2=_cror_(P2,1);}
		
	}
	



}	
