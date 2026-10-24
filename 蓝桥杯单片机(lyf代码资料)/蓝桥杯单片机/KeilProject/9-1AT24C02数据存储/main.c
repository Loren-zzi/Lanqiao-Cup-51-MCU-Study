#include <STC15F2K60S2.H>
#include "CLED.h"
#include "CBuzz.h"
#include "AT24C02.h"
#include "Nixie.h"
#include "Delay.h"

unsigned char Data=1;

void main()
{
	LED_Close();
	Buzz_Close();
	AT24C02_WriteByte(1,1);
	Delay(5);//写周期最长5ms，写完之后立刻读会出错
	Data=AT24C02_ReadByte(1);
	while(1)
	{
		Nixie(8,Data);
	}
}	
