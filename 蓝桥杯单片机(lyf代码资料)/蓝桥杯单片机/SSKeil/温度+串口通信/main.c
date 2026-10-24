#include <STC15F2K60S2.H>
#include "onewire.h"
#include "intrins.h"
#include "stdio.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xff};

unsigned char Temp[]={21,21,21,21,21,21,21,21};													

unsigned char Command;

float T,TT;

char SJ[20];

bit DS18B20Start=0;

void Delay800ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 37;
	j = 123;
	k = 92;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


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
	static unsigned char Count0;
	Nixie();
	if(++Count0>=200)
	{
		Count0=0;
		DS18B20Start=1;
	}
}

void UART_Write(unsigned char Data)
{
	SBUF=Data;
	while(TI==0);
	TI=0;
}

void UART_Read()
{
	if(RI)
	{
		RI=0;
		Command=SBUF;
	}
}

void Send_String(char *str)
{
	while(*str!='\n')
	UART_Write(*str++);
}

void DS18B20_Read()
{
	unsigned char Low,High;
	int TEMP;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	Low=Read_DS18B20();
	High=Read_DS18B20();
	TEMP=High<<8|Low;
	T=TEMP/16.0;
}

void main()
{
	Init();
	DS18B20_Read();
	Delay800ms();
	DS18B20_Read();
	Timer0Init();
	UartInit();
	while(1)
	{
		Temp[4]=(int)(T*100)/1000;
		Temp[5]=(int)(T*100)%1000/100;
		Temp[6]=(int)(T*100)%1000%100/10;
		Temp[7]=(int)(T*100)%10;
		if(DS18B20Start)
		{
			DS18B20Start=1;
			DS18B20_Read();
		}
		UART_Read();
		if(Command=='A')
		{
			Command=0;
			TT=(int)(T*100)/100;
			sprintf(SJ,"ÎÂ¶È£º%.2f",TT);
			Send_String(SJ);
		}
	}
}
