#include <REGX52.H>
#include "DELAY.h"
#include "NIXIE.h"
#include "KEY.h"
#include "Buzzer.h"
unsigned char keynum;

void main()
{
			Nixie(1,0);			
			while(1)
			{ 
				keynum=Key();
				if(keynum)
				{
					Nixie(1,keynum);
					Buzzer_Time(10);

				}
			}	
				
				
		

}