#include <STC15F2K60S2.H>
#include "iic.h"
#include "onewire.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xff,0xc6,0x8c,0x88};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char MkeyNum,JM,DACData,MS=1,LEDState=0xff;

char TCST,CST=25;

float T;

int AData;

bit MkeyStart=0,TState=1,DS18B20Start=0,PState=0,AState=0,DACState=0;

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
	if(LastState==0&&NowState==1)
	MkeyNum=1;
	if(LastState==0&&NowState==2)
	MkeyNum=2;
	if(LastState==0&&NowState==3)
	MkeyNum=3;
	if(LastState==0&&NowState==4)
	MkeyNum=4;
	if(LastState==0&&NowState==5)
	MkeyNum=5;
	if(LastState==0&&NowState==6)
	MkeyNum=6;
	if(LastState==0&&NowState==7)
	MkeyNum=7;
	if(LastState==0&&NowState==8)
	MkeyNum=8;
	if(LastState==0&&NowState==9)
	MkeyNum=9;
	if(LastState==0&&NowState==10)
	MkeyNum=10;
	if(LastState==0&&NowState==11)
	MkeyNum=11;
	if(LastState==0&&NowState==12)
	MkeyNum=12;
	if(LastState==0&&NowState==13)
	MkeyNum=13;
	if(LastState==0&&NowState==14)
	MkeyNum=14;
	if(LastState==0&&NowState==15)
	MkeyNum=15;
	if(LastState==0&&NowState==16)
	MkeyNum=16;
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

void DAC_Write(unsigned char Data)
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x40);
	IIC_WaitAck();
	IIC_SendByte(Data);
	IIC_WaitAck();
	IIC_Stop();
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
	TEMP=Low|(High<<8);
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
	if(++Count1>=200)
	{
		Count1=0;
		DS18B20Start=1;
	}
	if(++Count2>=50)
	{
		Count2=0;
		DACData=1;
	}
}

void T_Loop()
{
	Temp[0]=22;
	Temp[4]=(int)(T)/10;
	Temp[5]=(int)(T)%10+10;
	Temp[6]=(unsigned int)(T*100)%1000%100/10;
	Temp[7]=(unsigned int)(T*100)%1000%100%10;
}

void P_Loop()
{
	
	Temp[0]=23;
	Temp[4]=21;
	Temp[5]=21;
	Temp[6]=TCST/10;
	Temp[7]=TCST%10;
}

void A_Loop()
{
	Temp[0]=24;
	Temp[4]=21;
	Temp[5]=AData/100+10;
	Temp[6]=AData%100/10;
	Temp[7]=AData%100%10;
}

void MS1_Loop()
{
	if(T<CST)
	{
		DACData=0;
		AData=0;
	}
	else
	{
		DACData=255;
		AData=500;
	}
}

void MS2_Loop()
{
	if(T<=20)
	{
		AData=100;
		DACData=51;
	}
	if(T>20&&T<40)
	{
		AData=15*T-200;
		DACData=AData/500.0*255;
	}
	if(T>=40)
	{
		AData=400;
		DACData=204;
	}
}

void LED_Loop()
{
	if(MS)
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
	if(TState)
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
	if(PState)
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xfb;
		P0=LEDState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x04;
		P0=LEDState;
		P2&=0x1f;
	}
	if(AState)
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xf7;
		P0=LEDState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x08;
		P0=LEDState;
		P2&=0x1f;
	}
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 1:
		{
			MkeyNum=0;
			switch(JM)
			{
				case 0:
				{
					TState=0;
					PState=1;
					TCST=CST;
					JM=1;
				}break;
				case 1:
				{
					TState=0;
					PState=0;
					AState=1;
					CST=TCST;
					JM=2;
				}break;
				case 2:
				{
					TState=1;
					PState=0;
					AState=0;
					JM=0;
				}break;
			}
		}break;
		case 6:
		{
			MkeyNum=0;
			if(PState)
			{
				TCST++;
				if(TCST>=100)
				TCST=0;
			}
		}break;
		case 2:
		{
			MkeyNum=0;
			if(PState)
			{
				TCST--;
				if(TCST<0)
				TCST=99;
			}
		}break;
		case 5:
		{
			MkeyNum=0;
			switch(MS)
			{
				case 1:
				{
					MS=0;
				}break;
				case 0:
				{
					MS=1;
				}break;
			}			
		}break;
	}
}

void main()
{
	Init();
	DS18B20_Read();
	Delay800ms();
	DS18B20_Read();
	if(T<CST)
	{
		DACData=0;
		AData=0;
	}
	else
	{
		DACData=255;
		AData=500;
	}
	DAC_Write(DACData);
	Timer0Init();
	while(1)
	{
		if(MkeyStart)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		if(DS18B20Start)
		{
			DS18B20Start=0;
			DS18B20_Read();
		}
		Mkey_Select();
		LED_Loop();
		if(MS==1)
		MS1_Loop();
		else
		MS2_Loop();
		if(DACState)
		{
			DACState=0;
			DAC_Write(DACData);
		}
		if(TState)
		T_Loop();
		if(PState)
		P_Loop();
		if(AState)
		A_Loop();
	}
}