#include <STC15F2K60S2.H>
#include "CLED.h"
#include "CBuzz.h"
#include "Delay.h"
/*P3^2是外部中断请求输入端*/
void WTimer0_Init()
{
	IT0=1;
	EX0=1;//允许外部中断
	EA=1;//开启总中断
}

void Timer0_Loop() interrupt 0
{
	P2=((P2&0x1f)|0x80);
	P0=0xfe;
	P2&=0x1f;
	Delay(1000);
	
}

void main()
{
	WTimer0_Init();
	LED_Close();
	Buzz_Close();
	while(1)
	{
		P2=((P2&0x1f)|0x80);
		P0=0x00;
		P2&=0x1f;
	}
}
