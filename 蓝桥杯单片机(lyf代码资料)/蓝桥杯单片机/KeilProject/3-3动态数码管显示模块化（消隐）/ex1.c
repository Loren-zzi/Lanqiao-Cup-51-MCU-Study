#include <STC15F2K60S2.H>
#include "Delay.h"
#include "CBuzz.h"
#include "Nixie.h"
#include "CLED.h"

void main()
{
	Buzz_Close();
	LED_Close();
	while(1)
	{
		Nixie(1,10);
		Nixie(2,1);
		Nixie(3,3);
		Nixie(4,1);
		Nixie(5,4);
		Nixie(6,5);
		Nixie(7,2);
		Nixie(8,0);
	}
}