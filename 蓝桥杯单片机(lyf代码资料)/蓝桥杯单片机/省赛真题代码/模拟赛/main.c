#include <STC15F2K60S2.H>
#include "iic.h"
#include "onewire.h"
#include "intrins.h"
#include "stdio.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xff,0xc1};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

char du[]={0xa1,0xe3,0};

unsigned char MkeyNum=0,ADCData,LEDState=0xff,CountL3=0,BJState=0x00;

float T,U,TT;

unsigned int UData;

char Command;

char SJ[20];

bit MkeyStart=0,TState=1,UState=0,DS18B20Start=0,ADCStart=0,KG=1,L3State=1;

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

void Delay2ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 24;
	j = 85;
	do
	{
		while (--j);
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
	
	P44=1;
	P42=1;
	P35=0;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=3;
	return Mkey;
}

void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(LastState==0&&NowState==1)
	MkeyNum=1;
	if(LastState==0&&NowState==5)
	MkeyNum=5;
	if(LastState==0&&NowState==3)
	MkeyNum=3;
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
	TEMP=(High<<8)|Low;
	T=TEMP/16.0;
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count0,Count1,Count2;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		MkeyStart=1;
	}
	if(++Count1>=100)
	{
		Count1=0;
		DS18B20Start=1;
	}
	if(++Count2>=150)
	{
		Count2=0;
		ADCStart=1;
	}
	if(KG==0)
	{
		if(++CountL3>=50)
		{
			CountL3=0;
			L3State=~L3State;
		}
	}
}

void T_Loop()
{
	Temp[0]=22;
	Temp[1]=1;
	Temp[5]=(int)(T*10)/100;
	Temp[6]=(int)(T*10)%100/10+10;
	Temp[7]=(int)(T*10)%10;
}

void U_Loop()
{
	Temp[0]=22;
	Temp[1]=2;
	Temp[5]=UData/100+10;
	Temp[6]=UData%100/10;
	Temp[7]=UData%10;
}

void UART_Command()
{
	switch(Command)
	{
		case 'A':
		{
			TState=1;
			UState=0;
		}break;
		case 'B':
		{
			TState=0;
			UState=1;
		}break;
	}
	Command=0;
}

void LED_Loop()
{
	Delay2ms();
	if(TState)
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xfe;
		P0=LEDState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x01;
		P0=LEDState;
		P2&=0x1f;
	}
	Delay2ms();
	if(UState)
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xfd;
		P0=LEDState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x02;
		P0=LEDState;
		P2&=0x1f;
	}
	Delay2ms();
	if(KG==0)
	{
		if(L3State==1)
		{
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfb;
			P0=LEDState;
			P2&=0x1f;
		}
		if(L3State==0)
		{
			P2=((P2&0x1f)|0x80);
			LEDState|=0x04;
			P0=LEDState;
			P2&=0x1f;
		}
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x04;
		P0=LEDState;
		P2&=0x1f;
	}
	Delay2ms();
}

void JDQ_Loop()
{
	Delay2ms();
	if(T>28)
	{
		P2=((P2&0x1f)|0xa0);
		BJState|=0x10;
		P0=BJState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0xa0);
		BJState&=0xef;
		P0=0x00;
		P2&=0x1f;
	}
	Delay2ms();
}

void Beep_Loop()
{
	Delay2ms();
	if(UData>360)
	{
		P2=((P2&0x1f)|0xa0);
		BJState|=0x40;
		P0=BJState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0xa0);
		BJState&=0xbf;
		P0=0x00;
		P2&=0x1f;
	}
	Delay2ms();
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 1:
		{
			MkeyNum=0;
			KG=0;
		}break;
		case 5:
		{
			MkeyNum=0;
			if(KG==0)
			KG=1;
			CountL3=0;
			L3State=1;
		}break;
		case 3:
		{
			MkeyNum=0;
			if(TState)
			{
				TT=(int)(T*10)/10.0;
				sprintf(SJ,"温度：%.1f%sC\r\n",TT,du);
				Send_String(SJ);
			}
			if(UState)
			{
				U=UData/100.0;
				sprintf(SJ,"电压：%.2fV\r\n",U);
				Send_String(SJ);
			}
		}break;
	}
}

void main()
{
	Init();
	UartInit();
	DS18B20_Read();
	Delay800ms();
	DS18B20_Read();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	UData=ADCData/255.0*500;
	Timer0Init();
	while(1)
	{
		if(DS18B20Start)
		{
			DS18B20Start=0;
			DS18B20_Read();
		}
		if(ADCStart)
		{
			ADCStart=0;
			ADCData=ADC_Read();
			UData=ADCData/255.0*500;
		}
		JDQ_Loop();
		Beep_Loop();
		if(MkeyStart)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		Mkey_Select();
		LED_Loop();
		UART_Read();
		if(KG)
		UART_Command();
		else
		Command=0;
		if(TState)
		T_Loop();
		if(UState)
		U_Loop();
	}
}
