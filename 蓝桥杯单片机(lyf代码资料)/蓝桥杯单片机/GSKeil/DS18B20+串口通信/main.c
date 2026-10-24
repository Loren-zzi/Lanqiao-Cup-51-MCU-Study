#include <STC15F2K60S2.H>
#include "onewire.h"
#include "intrins.h"
#include "stdio.h"

typedef unsigned char u8;
typedef unsigned int u16;
code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xef,0xff};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char KeyNum,Command;

float T,TT;

bit KeyStart,DS18B20Start;

char WD[20];

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
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
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

u8 Key_State()
{
	u8 Key=0;
	if(P33==0)
	Key=1;
	if(P32==0)
	Key=2;
	if(P31==0)
	Key=3;
	if(P30==0)
	Key=4;
	return Key;
}

void Key_Loop()
{
	static u8 NowState,LastState;
	LastState=NowState;
	NowState=Key_State();
	if(NowState==1&&LastState==0)
	KeyNum=1;
	if(NowState==2&&LastState==0)
	KeyNum=2;
	if(NowState==3&&LastState==0)
	KeyNum=3;
	if(NowState==4&&LastState==0)
	KeyNum=4;
}

void DS18B20_Read()
{
	unsigned char TLSB,TMSB;
	int TEMP;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	TLSB=Read_DS18B20();
	TMSB=Read_DS18B20();
	TEMP=TMSB<<8|TLSB;
	T=TEMP/16.0;
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
	Command=SBUF;
	RI=0;
}

void UART_String(char *Str)
{
	while(*Str!='\0')
	UART_Write(*Str++);
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


void Timer0_Loop() interrupt 1
{
	static u8 Count0,Count1;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		KeyStart=1;
	}
	if(++Count1>=100)
	{
		Count1=0;
		DS18B20Start=1;
	}
}

void Key_Select()
{
	switch(KeyNum)
	{
		case 1:
		{
			KeyNum=0;
			Temp[0]=1;	
		}break;
		case 2:
		{
			KeyNum=0;
			Temp[1]=2;	
		}break;
		case 3:
		{
			KeyNum=0;
			Temp[2]=3;	
		}break;
		case 4:
		{
			KeyNum=0;
			Temp[3]=4;	
		}break;
	}
}

void UART_Command()
{
	switch(Command)
	{
		case 'A':
		{
			Command=0;
			TT=(int)(T*100)/100.0;
			sprintf(WD,"ÎÂ¶È£º%.2fC\r\n",TT);
			UART_String(WD);
		}break;
	}
}

void main()
{
	Init();
	Timer0Init();
	DS18B20_Read();
	Delay800ms();
	DS18B20_Read();
	UartInit();
	while(1)
	{
		if(KeyStart)
		{
			KeyStart=0;
			Key_Loop();
		}
		if(DS18B20Start)
		{
			DS18B20Start=0;
			DS18B20_Read();
		}
		Key_Select();
		UART_Read();
		UART_Command();
		Temp[4]=(unsigned int)(T)/10;
		Temp[5]=(unsigned int)(T)%10+10;
		Temp[6]=(unsigned int)(T*100)%1000%100/10;
		Temp[7]=(unsigned int)(T*100)%10;
	}
}
