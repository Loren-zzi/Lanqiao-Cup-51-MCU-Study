#include <STC15F2K60S2.H>
#include "intrins.h"
#include "onewire.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff};

unsigned char Temp[]={20,20,20,20,20,20,20,20};

bit MkeyStart,DS18B20Start=1;
float T;

sbit List_1=P4^4;
sbit List_2=P4^2;
sbit List_3=P3^5;
sbit List_4=P3^4;
sbit Line_1=P3^3;
sbit Line_2=P3^2;
sbit Line_3=P3^1;
sbit Line_4=P3^0;

unsigned char MkeyNum,High=15,Low=11;


void Delay1000ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 46;
	j = 153;
	k = 245;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
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
	List_1=0;
	List_2=1;
	List_3=1;
	List_4=1;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=1;
	if(Line_2==0)
	Mkey=5;
	if(Line_3==0)
	Mkey=9;
	if(Line_4==0)
	Mkey=13;

	List_1=1;
	List_2=0;
	List_3=1;
	List_4=1;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=2;
	if(Line_2==0)
	Mkey=6;
	if(Line_3==0)
	Mkey=10;
	if(Line_4==0)
	Mkey=14;
	
	List_1=1;
	List_2=1;
	List_3=0;
	List_4=1;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=3;
	if(Line_2==0)
	Mkey=7;
	if(Line_3==0)
	Mkey=11;
	if(Line_4==0)
	Mkey=15;
	
	List_1=1;
	List_2=1;
	List_3=1;
	List_4=0;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=4;
	if(Line_2==0)
	Mkey=8;
	if(Line_3==0)
	Mkey=12;
	if(Line_4==0)
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


void Timer0Init(void)		//2毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x30;		//设置定时初值
	TH0 = 0xF8;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
}

void Timer0_Loop() interrupt 1
{
	static unsigned int Count0;
	Nixie();
	if(++Count0==10)
	{
		Count0=0;
		MkeyStart=1;
	}
}

void DS18B20_ReadByte()
{
	unsigned char TLSB,TMSB;
	int Tem;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	//Delay1000ms();
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	TLSB=Read_DS18B20();
	TMSB=Read_DS18B20();
	Tem=(TMSB<<8)|TLSB;
	T=Tem/16.0;
	if(T>0)
	{
		Temp[0]=1;
	}
	else
	Temp[0]=0;
	Temp[2]=(unsigned int)(T)/10;
	Temp[3]=(unsigned int)(T)%10+10;
	Temp[4]=(unsigned long)(T*10000)%10000/1000;
	Temp[5]=(unsigned long)(T*10000)%10000/100%10;
	Temp[6]=(unsigned long)(T*10000)%10000/10%100%10;
	Temp[7]=(unsigned long)(T*10000)%10000%1000%100%10;
}	

void DS18B20_Init()
{
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	Delay1000ms();
}

void TBJ()
{
	unsigned int Temp;
	Temp=(unsigned int)(T);
	if(Temp>=Low&&Temp<High)
	{
		P2=((P2&0x1f)|0xA0);
		P0=0x00;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0xA0);
		P0=0x40;
		P2&=0x1f;
	}
}

void main()
{
	Timer0Init();
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	DS18B20_Init();
	while(1)
	{
		if(MkeyStart)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		switch(MkeyNum)
		{
			case 1:
			{
				MkeyNum=0;
				DS18B20Start=1;
			}break;
			case 2:
			{
				MkeyNum=0;
				Temp[0]=1;
				Temp[7]=High%10;
				Temp[6]=High/10;
				Temp[5]=20;
				Temp[4]=20;
				Temp[3]=20;
				Temp[2]=20;
			}break;
			case 3:
			{
				MkeyNum=0;
				Temp[0]=1;
				Temp[7]=Low%10;
				Temp[6]=Low/10;
				Temp[5]=20;
				Temp[4]=20;
				Temp[3]=20;
				Temp[2]=20;
			}break;
		}
		while(DS18B20Start)
		{
			if(MkeyStart)
			{
				MkeyStart=0;
				Mkey_Loop();
			}
			TBJ();
			if(MkeyNum!=1&&MkeyNum!=0)
			DS18B20Start=0;
			DS18B20_ReadByte();
		}
	}
}