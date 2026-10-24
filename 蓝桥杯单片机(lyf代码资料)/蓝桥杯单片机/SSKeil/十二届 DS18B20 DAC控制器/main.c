#include <STC15F2K60S2.H>
#include "iic.h"
#include "onewire.h"
#include "intrins.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xc6,0x8c,0x88};
unsigned char Temp[]={20,20,20,20,20,20,20,20};

unsigned char MkeyNum,DACData,JM,LState=0xff;

unsigned int DAC;

char SZT=25,TSZT=25;
unsigned char led=0xff;

float T;

bit MkeyStart,DS18B20Start,DACStart,MS,TStart=1,PStart,AStart;

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
	if(++Bit==8)
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
	Mkey=2;

	P44=1;
	P42=0;
	P35=1;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=3;
	if(P32==0)
	Mkey=4;
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
}

void Timer0Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x40;		//设置定时初值
	TH0 = 0xA2;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	EA=1;
	ET0=1;
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
	if(++Count2>=50)
	{
		Count2=0;
		DACStart=1;
	}
	if(++Count1>=200)
	{
		Count1=0;
		    DS18B20Start=1;
	}
}

void T_Loop()
{
	Temp[0]=21;
	Temp[4]=(int)(T)/10;
	Temp[5]=(int)(T)%10+10;
	Temp[6]=(unsigned int)(T*100)%100/10;
	Temp[7]=(unsigned int)(T*100)%100%10;
}

void P_Loop()
{
	Temp[0]=22;
	Temp[4]=20;
	Temp[5]=20;
	Temp[6]=TSZT/10;
	Temp[7]=TSZT%10;
}

void A_Loop()
{
	Temp[0]=23;
	Temp[4]=20;
	Temp[5]=DAC/100+10;
	Temp[6]=DAC%100/10;
	Temp[7]=DAC%100%10;
}

void LED_Loop()
{
	if(TStart)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xfd;
		P0=LState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LState|=0x02;
		P0=LState;
		P2&=0x1f;
	}
	if(PStart)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xfb;
		P0=LState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LState|=0x04;
		P0=LState;
		P2&=0x1f;
	}
	if(AStart)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xf7;
		P0=LState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LState|=0x08;
		P0=LState;
		P2&=0x1f;
	}
	if(MS==0)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xfe;
		P0=LState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LState|=0x01;
		P0=LState;
		P2&=0x1f;
	}
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 4:
		{
			MkeyNum=0;
			if(PStart==1)
			{
				TSZT++;
				if(TSZT>=100)
				TSZT=0;
			}
		}break;
		case 3:
		{
			MkeyNum=0;
			if(PStart==1)
			{
				TSZT--;
				if(TSZT<0)
				TSZT=99;
			}
		}break;
		case 1:
		{
			MkeyNum=0;
			switch(JM)
			{
				case 0:
				{
					JM=1;
					TSZT=SZT;
					TStart=0;
					PStart=1;
					AStart=0;
				}break;
				case 1:
				{
					JM=2;
					TStart=0;
					PStart=0;
					AStart=1;
					SZT=TSZT;
				}break;
				case 2:
				{
					JM=0;
					TStart=1;
					PStart=0;
					AStart=0;
				}break;
			}
		}break;
		case 2:
		{
			MkeyNum=0;
			if(MS==0)
			MS=1;
			else
			MS=0;
		}break;
	}
	
}

void main()
{
	Init();
	DS18B20_Read();
	Delay800ms();
	DS18B20_Read();
	if(T<TSZT)
	{
		DACData=255;
	}
	else
	{
		DACData=0;
	}
	DAC_Write(DACData);
	DAC=DACData/255.0*500;
	Timer0Init();
	while(1)
	{
		if(MkeyStart==1)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		Mkey_Select();
		LED_Loop();
		if(DS18B20Start==1)
		{
			DS18B20Start=0;
			DS18B20_Read();
		}
		if(DACStart==1)
		{
			DACStart=0;
			if(MS==0)
			{
				if(T<SZT)
				{
					DACData=255;
				}
				else
				{
					DACData=0;
				}
				DAC_Write(DACData);
				DAC=DACData/255.0*500;
			}
			if(MS==1)
			{
				if(T<=20)
				DAC=100;
				if(T>20&&T<40)
				DAC=T*15-200;
				if(T>=40)
				DAC=400;
				DACData=DAC/500.0*255;
				DAC_Write(DACData);
			}
		}
		if(TStart==1)
		T_Loop();
		if(PStart==1)
		P_Loop();
		if(AStart==1)
		A_Loop();
	}
}

