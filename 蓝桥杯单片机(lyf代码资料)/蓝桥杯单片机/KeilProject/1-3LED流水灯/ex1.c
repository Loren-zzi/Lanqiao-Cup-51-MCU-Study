#include <STC15F2K60S2.H>
#include "Delay.h"
#include "CBuzz.h"

void main()
{
	Buzz_Close();
	P2&=0x1f;
	P2|=0x80;
	while(1)
  {		
		P0=0xfe;  //1111 1110从左边数第一个亮
		Delay(500);
		P0=0xfd;  //1111 1101
		Delay(500);
		P0=0xfb;  //1111 1011
		Delay(500);
		P0=0xf7;  //1111 0111
		Delay(500);
		P0=0xef;  //1110 1111
		Delay(500);
		P0=0xdf;  //1101 1111
		Delay(500);
		P0=0xbf;  //1011 1111
		Delay(500);
		P0=0x7f;  //0111 1111
		Delay(500);
	}
}