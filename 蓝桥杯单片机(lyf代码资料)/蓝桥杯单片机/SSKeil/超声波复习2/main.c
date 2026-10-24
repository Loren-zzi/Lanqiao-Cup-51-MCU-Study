#include <STC15F2K60S2.H>
#include "intrins.h"
#include "stdio.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

unsigned char Command;

float T=25.25;

char WD[20];

void Init()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
}

void Nixie()
{
	static unsigned char Bit;
	P2=((P2&0x1f)|0xe0);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&0x1f)|0xc0);
	P0=0x01<<Bit;
	P2&=0x1f;
	P2=((P2&0x1f)|0xe0);
	P0=Seg[Temp[Bit]];
	P2&=0x1f;
	if(++Bit>=8)
	Bit=0;
}

void UART_ReadByte()
{
	if(RI==1)
	{
		RI=0;
		Command=SBUF;
	}
}

void UART_SendByte(unsigned char Data)
{
	SBUF=Data;
	while(TI==0);
	TI=0;
}

void UART_String(char *str)
{
	while(*str!='\0')
	UART_SendByte(*str++);
}

void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8???,?????
	AUXR |= 0x40;		//???1???Fosc,?1T
	AUXR &= 0xFE;		//??1?????1???????
	TMOD &= 0x0F;		//?????1?16???????
	TL1 = 0xC7;		//??????
	TH1 = 0xFE;		//??????
	ET1 = 0;		//?????1??
	TR1 = 1;		//?????1
}

void Timer0Init(void)		//2??@12.000MHz
{
	AUXR |= 0x80;		//?????1T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x40;		//??????
	TH0 = 0xA2;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	EA=1;
	ET0=1;
}

void Timer0_Loop() interrupt 1
{
	Nixie();
}

void main()
{
	Init();
	Timer0Init();
	UartInit();
	while(1)
	{
		UART_ReadByte();
		if(Command==0xff)
		{
			Command=0;
			sprintf(WD,"ÎÂ¶È£º%.2fC\r\n",T);
			UART_String(WD);
		}
	}
}
