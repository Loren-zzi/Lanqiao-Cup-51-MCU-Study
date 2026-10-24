#include <STC15F2K60S2.H>
#include "iic.h"
#include "ds1302.h"
#include "onewire.h"
#include "stdio.h"
#include "intrins.h"

#define WP 0x8e

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xff};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

code unsigned char TimeWCommand[]={0x8c,0x88,0x86,0x84,0x82,0x80,0x8a};

code unsigned char TimeRCommand[]={0x8d,0x89,0x87,0x85,0x83,0x81,0x8b};

unsigned char Time[]={0x23,0x03,0x31,0x11,0x42,0x00,0x05};

unsigned char MkeyNum,ADCData,UARTData;

unsigned int UData,FData,x,SJ;

float T,TT;

char WD[20];

bit RQ=1,CSBStart=1;

sbit TX=P1^0;
sbit RX=P1^1;

//void Delay5ms()		//@12.000MHz
//{
//	unsigned char i, j;

//	i = 59;
//	j = 90;
//	do
//	{
//		while (--j);
//	} while (--i);
//}

void Delay12us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
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

void Time_Set()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<7;i++)
	Write_Ds1302_Byte(TimeWCommand[i],Time[i]);
	Write_Ds1302_Byte(WP,0x80);
}

void Time_Read()
{
	unsigned char i;
	for(i=0;i<7;i++)
	Time[i]=Read_Ds1302_Byte(TimeRCommand[i]);
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

unsigned char Mkey_State()
{
	unsigned char Mkey=0;
	P44=0;
	P42=1;
	P35=1;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=1;
	if(P32==0)
	Mkey=5;
	if(P31==0)
	Mkey=9;
	if(P30==0)
	Mkey=13;
	
	P44=1;
	P42=0;
	P35=1;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=2;
	if(P32==0)
	Mkey=6;
	if(P31==0)
	Mkey=10;
	if(P30==0)
	Mkey=14;

	P44=1;
	P42=1;
	P35=0;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=3;
	if(P32==0)
	Mkey=7;
	if(P31==0)
	Mkey=11;
	if(P30==0)
	Mkey=15;

	P44=1;
	P42=1;
	P35=1;
	P34=0;
	P3|=0x0f;
	if(P33==0)
	Mkey=4;
	if(P32==0)
	Mkey=8;
	if(P31==0)
	Mkey=12;
	if(P30==0)
	Mkey=16;
	
	return Mkey;
}

void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(NowState==0&&LastState==1)
	MkeyNum=1;
	if(NowState==0&&LastState==2)
	MkeyNum=2;
	if(NowState==0&&LastState==3)
	MkeyNum=3;
	if(NowState==0&&LastState==4)
	MkeyNum=4;
	if(NowState==0&&LastState==5)
	MkeyNum=5;
	if(NowState==0&&LastState==6)
	MkeyNum=6;
	if(NowState==0&&LastState==7)
	MkeyNum=7;
	if(NowState==0&&LastState==8)
	MkeyNum=8;
	if(NowState==0&&LastState==9)
	MkeyNum=9;
	if(NowState==0&&LastState==10)
	MkeyNum=10;
	if(NowState==0&&LastState==11)
	MkeyNum=11;
	if(NowState==0&&LastState==12)
	MkeyNum=12;
	if(NowState==0&&LastState==13)
	MkeyNum=13;
	if(NowState==0&&LastState==14)
	MkeyNum=14;
	if(NowState==0&&LastState==15)
	MkeyNum=15;
	if(NowState==0&&LastState==16)
	MkeyNum=16;
	
}

void Timer0Init(void)		//65536??@12.000MHz
{
	AUXR &= 0x7F;		//?????12T??
	TMOD &= 0xF0;		//???????
	TMOD |= 0x01;		//???????
	TL0 = 0x00;		//??????
	TH0 = 0x00;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 0;		//???0????
}


void Timer1Init(void)		//2??@12.000MHz
{
	AUXR |= 0x40;		//?????1T??
	TMOD &= 0x0F;		//???????
	TL1 = 0x40;		//??????
	TH1 = 0xA2;		//??????
	TF1 = 0;		//??TF1??
	TR1 = 1;		//???1????
	EA=1;
	ET1=1;
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
//}

void UART_SendByte(char Data)
{
	SBUF=Data;
	while(TI==0);
	TI=0;
}

void UART_ReadByte() 
{
	if(RI==1)
	{
		RI=0;
		UARTData=SBUF;
	}
}

void CSB_Loop()
{
	unsigned char i;
	for(i=0;i<=8;i++)
	{
		TX=1;
		Delay12us();
		TX=0;
		Delay12us();
	}
	TR0=1;
	while(RX==1&&TF0==0);
	TR0=0;
	if(TF0==1)
	{
		TF0=0;
		x=999;
	}
	else
	{
		RX=1;
		SJ=TH0<<8|TL0;
		x=SJ*0.017;
	}
	TH0=0;
	TL0=0;
}

void UART_String(char *str)
{
	while(*str!='\0')
	UART_SendByte(*str++);
}

void Timer1_Loop() interrupt 3
{
	static unsigned char Count0,Count1,Count2;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		Mkey_Loop();
	}
	if(++Count1>=250)
	{
		Count1=0;
		FData=TH0<<8|TL0;
		FData*=2;
		TH0=0;
		TL0=0;
	}
	if(++Count2>=50)
	{
		Count2=0;
		CSBStart=1;
	}
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
	Temp[4]=(unsigned int)(T)/10;
	Temp[5]=(unsigned int)(T)%10+10;
	Temp[6]=(unsigned int)(T*100)%100/10;
	Temp[7]=(unsigned int)(T*100)%10;
}

unsigned char ADC_Read()
{
	unsigned char Data;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x03);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0x90|0x01);
	I2CWaitAck();
	Data=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return Data;
}

void main()
{
	Init();
	Time_Set();
	Timer1Init();
	Timer0Init();
	//UartInit();
	ADCData=ADC_Read();
	Time_Read();
	UART_SendByte(0xff);
	//DS18B20_Read();
	UART_ReadByte();
	switch(UARTData)
	{
		case 'A':
		{
			UARTData=0;
			TT=(int)(T*100)/100.0;
			sprintf(WD,"ÎÂ¶È£º%.2fC\r\n",TT);
			UART_String(WD);
		}break;
	}
	while(1)
	{
		if(CSBStart)
		{
			CSBStart=0;
			CSB_Loop();
		}
		Temp[5]=x/100;
		Temp[6]=x%100/10;
		Temp[7]=x%10;
	}	
}

