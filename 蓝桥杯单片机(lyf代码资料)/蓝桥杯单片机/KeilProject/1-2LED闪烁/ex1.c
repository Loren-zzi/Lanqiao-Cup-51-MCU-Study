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
			P0=0xfe;   //1111 1110从左边数第一个亮
			Delay(1000);
			P0=0xff;   //1111 1110从左边数第一个灭
			Delay(1000); 
	}
}