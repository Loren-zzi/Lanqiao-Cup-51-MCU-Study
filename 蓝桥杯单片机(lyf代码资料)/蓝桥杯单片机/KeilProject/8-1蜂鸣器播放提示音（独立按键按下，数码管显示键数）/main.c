#include <STC15F2K60S2.H>
#include "CBuzz.h"
#include "CLED.h"
#include "Key.h"
#include "Nixie.h"
#include "Buzzer.h"

unsigned char Num;

void main()
{
	Buzz_Close();
	LED_Close();
	Nixie(8,0);
	while(1)
	{
		Num=Key();
		if(Num)
		{		
			Nixie(8,Num);
			Buzzer_Init(100);
		}
	}
}