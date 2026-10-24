#include <STC15F2K60S2.H>
#include "Delay.h"
#include "CBuzz.h"
#include "UART.h"
#include "CLED.h"

unsigned char Sec;

void main()
{
	Buzz_Close();
	LED_Close();
	UART_Init();	
	while(1)
	{
		UART_SendByte(Sec);
		Sec++;
		Delay(1000);
	}
}