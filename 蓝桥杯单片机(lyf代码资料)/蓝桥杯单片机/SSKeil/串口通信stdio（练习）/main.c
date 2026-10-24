#include <STC15F2K60S2.H>
#include "stdio.h"
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xff};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char Command,ADCData;

unsigned int UData;

float Dy;

char SJ[20];

bit ADCStart=0;

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

void Timer0Init(void)		//2??@12.000MHz
{
	AUXR |= 0x80;		//?????1T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x40;		//??????
	TH0 = 0xA2;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	ET0=1;
	EA=1;
}


//void UartInit(void)		//9600bps@12.000MHz
//{
//	SCON = 0x50;		//8???,?????
//	AUXR |= 0x40;		//???1???Fosc,?1T
//	AUXR &= 0xFE;		//??1?????1???????
//	TMOD &= 0x0F;		//?????1?16???????
//	TL1 = 0xC7;		//??????
//	TH1 = 0xFE;		//??????
//	ET1 = 0;		//?????1??
//	TR1 = 1;		//?????1
////	ES=1;
////	EA=1;
//}

//void UART_Write(unsigned char Data)
//{
//	SBUF=Data;
//	while(TI==0);
//	TI=0;
//}

//void UART_Read() //interrupt 4
//{
//	if(RI==1)
//	{
//		RI=0;
//		Command=SBUF;
//	}
//}

//void Send_String(char *str)
//{
//	while(*str!='\0')
//	{
//		UART_Write(*str++);
//	}
//}

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
	ES=1;
}

void UART_Write(unsigned char Data)
{
	SBUF=Data;
	while(TI==0);
	TI=0;
}

void UART_Read() interrupt 4
{
	if(RI)
	{
		RI=0;
		Command=SBUF;
	}
}

void Send_String(char *Str)
{
	while(*Str!='\0')//问（*Str！='\0'）区别
	{
		UART_Write(*Str++);
	}
	
}

unsigned char ADC_Read()
{
	unsigned char Data;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0x90|0x01);
	IIC_WaitAck();
	Data=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return Data;
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count0;
	Nixie();
	if(++Count0>=50)
	{
		Count0=0;
		ADCStart=1;
	}
}

void main()
{
	Init();
	UartInit();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	UData=ADCData/255.0*500;
	Timer0Init();
	while(1)
	{
		if(ADCStart)
		{
			ADCStart=0;
			ADCData=ADC_Read();
			UData=ADCData/255.0*500;
		}
		Temp[5]=UData/100+10;
		Temp[6]=UData%100/10;
		Temp[7]=UData%10;
		if(Command=='A')
		{
			Command=0;
			Dy=UData/100.0;
			sprintf(SJ,"电压：%.2fV\r\n",Dy);
			Send_String(SJ);
		}
	}
}

