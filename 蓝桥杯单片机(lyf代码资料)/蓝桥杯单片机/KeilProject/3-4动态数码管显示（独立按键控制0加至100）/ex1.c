#include <STC15F2K60S2.H>
#include "Delay.h"
#include "CBuzz.h"
#include "Nixie.h"
#include "Key.h"
#include "CLED.h"

unsigned char Unit,Tens,Hundreds,KeyNum,Shi=0,Bai=0;
void main()
{
	Buzz_Close();
	LED_Close();
	Unit=0;
	Tens=0;
	Hundreds=0;
	while(1)
	{
		Nixie(8,Unit);
		KeyNum=Key();
		if(Shi)
		{
			Nixie(7,Tens);
		}
		if(Bai)
		{
			Nixie(6,Hundreds);
		}		
		if(KeyNum==4)
		{
			Unit++;			
			if(Unit==10)
			{
				Unit=0;
				Tens++;
				if(Tens==1)
				{
					Shi=1;
				}
			}
			if(Tens==10)
			{
				Tens=0;
				Hundreds++;
				if(Hundreds==1)
				{
					Bai=1;
				}
			}
			if(Unit==1&&Hundreds==1)
			{
				Unit=0;
				Hundreds=0;
				Shi=0;
				Bai=0;
			}			
		}
	}
}