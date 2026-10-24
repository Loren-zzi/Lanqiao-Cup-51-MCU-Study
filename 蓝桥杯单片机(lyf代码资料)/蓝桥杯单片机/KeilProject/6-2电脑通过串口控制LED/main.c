#include <STC15F2K60S2.H>
#include "CBuzz.h"
#include "UART.h"

void UART_Routine() interrupt 4
{
	if(RI==1)
	{
		P0=SBUF;  
		UART_SendByte(SBUF);
		RI=0;
	}
}

void main()
{
	Buzz_Close();
	P2&=0x1f;
	P2|=0x80;
	UART_Init();	
	while(1)
	{
		
	}
}