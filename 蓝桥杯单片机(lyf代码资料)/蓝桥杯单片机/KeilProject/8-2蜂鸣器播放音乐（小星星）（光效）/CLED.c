#include <STC15F2K60S2.H>

void LED_Close()
{
	P2&=0x1f;
	P2|=0x80;
	P0=0xff;  
	P2&=0x1f;
}