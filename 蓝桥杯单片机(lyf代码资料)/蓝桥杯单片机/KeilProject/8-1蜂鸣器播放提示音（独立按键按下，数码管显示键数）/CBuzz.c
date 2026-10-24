#include <STC15F2K60S2.H>

void Buzz_Close()
{
	P2&=0x1F;
	P2|=0xA0;
	P0=0x00;
	P2&=0x1F;
}