#include <STC15F2K60S2.H>
#include "ds1302.h"
#include "stdio.h"

#define WP 0x8e
code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

code unsigned char TimeWCommand[]={0x8c,0x88,0x86,0x84,0x82,0x80};
code unsigned char TimeRCommand[]={0x8d,0x89,0x87,0x85,0x83,0x81};

unsigned char Time[]={0x23,0x05,0x27,0x15,0x18,0x00,0x06};

unsigned char MkeyNum,Command;

unsigned int Nian,Yue,Ri,Hour,Min,Sec;

bit MkeyStart,RQ=0,SJ=1;

char S[20];
char R[20];

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
	if(NowState==1&&LastState==0)
	MkeyNum=1;
	if(NowState==2&&LastState==0)
	MkeyNum=2;
	if(NowState==3&&LastState==0)
	MkeyNum=3;
	if(NowState==4&&LastState==0)
	MkeyNum=4;
	if(NowState==5&&LastState==0)
	MkeyNum=5;
	if(NowState==6&&LastState==0)
	MkeyNum=6;
	if(NowState==7&&LastState==0)
	MkeyNum=7;
	if(NowState==8&&LastState==0)
	MkeyNum=8;
	if(NowState==9&&LastState==0)
	MkeyNum=9;
	if(NowState==10&&LastState==0)
	MkeyNum=10;
	if(NowState==11&&LastState==0)
	MkeyNum=11;
	if(NowState==12&&LastState==0)
	MkeyNum=12;
	if(NowState==13&&LastState==0)
	MkeyNum=13;
	if(NowState==14&&LastState==0)
	MkeyNum=14;
	if(NowState==15&&LastState==0)
	MkeyNum=15;
	if(NowState==16&&LastState==0)
	MkeyNum=16;
}

void Time_Write()
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
	Nian=Time[0]/16*10+Time[0]%16;
	Yue=Time[1]/16*10+Time[1]%16;
	Ri=Time[2]/16*10+Time[2]%16;
	Hour=Time[3]/16*10+Time[3]%16;
	Min=Time[4]/16*10+Time[4]%16;
	Sec=Time[5]/16*10+Time[5]%16;
}

//void UART_Write(unsigned char Data)
//{
//	SBUF=Data;
//	while(TI==0);
//	TI=0;
//}

//void UART_Read()
//{
//	if(RI)
//	Command=SBUF;
//	RI=0;
//}

//void UART_String(char *Str)
//{
//	while(*Str!='\0')
//	UART_Write(*Str++);
//}

void UART_Write(char Data)
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
	static unsigned char Count0;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		MkeyStart=1;
	}
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 1:
		{
			MkeyNum=0;
			if(SJ)
			{
				SJ=0;
				RQ=1;
			}
			else
			{
				SJ=1;
				RQ=0;
			}
		}break;
	}
}

void SJ_Loop()
{
	if(SJ)
	{
		Temp[0]=Time[3]/16;
		Temp[1]=Time[3]%16;
		Temp[2]=20;
		Temp[3]=Time[4]/16;
		Temp[4]=Time[4]%16;
		Temp[5]=20;
		Temp[6]=Time[5]/16;
		Temp[7]=Time[5]%16;
	}
	if(RQ)
	{
		Temp[0]=Time[0]/16;
		Temp[1]=Time[0]%16;
		Temp[2]=20;
		Temp[3]=Time[1]/16;
		Temp[4]=Time[1]%16;
		Temp[5]=20;
		Temp[6]=Time[2]/16;
		Temp[7]=Time[2]%16;
	}
}

void UART_Command()
{
	switch(Command)
	{
		case 'A':
		{
			Command=0;
			sprintf(S,"%d-%d-%d\r\n",Hour,Min,Sec);
			UART_String(S);
		}break;
		case 'B':
		{
			Command=0;
			sprintf(R,"%d-%d-%d\r\n",Nian,Yue,Ri);
			UART_String(R);
		}break;
	}
}

void main()
{
	Init();
	Time_Write();
	Timer0Init();
	Time_Read();
	UartInit();
	while(1)
	{
		Time_Read();
		if(MkeyStart)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		Mkey_Select();
		SJ_Loop();
		UART_Read();
		UART_Command();
	}
}

