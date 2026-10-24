#include <REGX52.H>
#include "Delay.h"
#include "UART.h"


void main()
{
	Uart_Init();
      while(1)
        {
					
				}			

}
void Uart_Routine()  interrupt 4
{
			if(RI==1)
			{
				P2=~SBUF;#include
				RI=0;
			}
				

}