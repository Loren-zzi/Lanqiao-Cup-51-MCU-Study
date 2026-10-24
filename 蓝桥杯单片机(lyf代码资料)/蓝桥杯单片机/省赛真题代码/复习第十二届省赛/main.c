#include <STC15F2K60S2.H>
#include "iic.h"
#include "onewire.h"
#include "intrins.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
												0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,0xc6,0x8c,0x88};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char MkeyNum,DACData,LEDStart=0xff;

char CST=25,TCST;

float T;

unsigned int DACUData;

bit MkeyStart,DS18B20Start,DACStart,CStart=1,PStart,AStart,MS1Start=1,MS2Start;

bit a=1,b=1,c=1,d=1,e=1;

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

unsigned char Mkey_State()
{
	unsigned char Mkey=0;
	P44=0;
	P42=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=1;
	if(P32==0)
	Mkey=5;
	
	P44=1;
	P42=0;
	P3|=0x0f;
	if(P33==0)
	Mkey=2;
	if(P32==0)
	Mkey=6;
	
	return Mkey;
}

void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(NowState==1&&LastState==0)
	MkeyNum=1;
	if(NowState==5&&LastState==0)
	MkeyNum=5;
	if(NowState==2&&LastState==0)
	MkeyNum=2;
	if(NowState==6&&LastState==0)
	MkeyNum=6;
}

void DAC_Write(unsigned char Data)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40);
	I2CWaitAck();
	I2CSendByte(Data);
	I2CWaitAck();
	I2CStop();
}

void DS18B20_Read()
{
	int TEMP;
	unsigned char TLSB,TMSB;
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
	if(++Count2>=50)
	{
		Count2=0;
		DACStart=1;
	}
}

void C_Loop()
{
	Temp[0]=22;
	Temp[4]=(int)(T)/10;
	Temp[5]=(int)(T)%10+10;
	Temp[6]=(int)(T*100)%1000%100/10;
	Temp[7]=(int)(T*100)%10;
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
	Temp[0]=23;
	Temp[4]=21;
	Temp[5]=DACUData/100+10;
	Temp[6]=DACUData%100/10;
	Temp[7]=DACUData%10;
}

void MS_Loop()
{
	if(MS1Start)
	{
		if((int)(T*100)/100.0<CST)
		{
			DACData=0;
			DACUData=0;
		}
		else
		{
			DACData=255;
			DACUData=500;
		}
	}
	if(MS2Start)
	{
		if((int)(T*100)/100.0<=20)
		{
			DACData=51;
			DACUData=100;
		}else	if((int)(T*100)/100.0>=40)
		{
			DACData=204;
			DACUData=400;
		}else
		{
			DACUData=(int)(T*100)/100.0*15-200;
			DACData=((int)(T*100)/100.0*15-200)/500.0*255;
		}
	}
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 1:
		{
			MkeyNum=0;
			if(CStart)
			{
				CStart=0;
				PStart=1;
				TCST=CST;
			}else if(PStart)
			{
				PStart=0;
				AStart=1;
				CST=TCST;
			}else if(AStart)
			{
				AStart=0;
				CStart=1;
			}
		}break;
		case 5:
		{
			MkeyNum=0;
			if(MS1Start)
			{
				MS1Start=0;
				MS2Start=1;
			}
			else
			{
				MS1Start=1;
				MS2Start=0;
			}
		}break;
		case 2:
		{
			MkeyNum=0;
			if(PStart)
			{
				TCST--;
				if(TCST<0)
				TCST=99;
			}
		}break;
		case 6:
		{
			MkeyNum=0;
			if(PStart)
			{
				TCST++;
				if(TCST>99)
				TCST=0;
			}
		}break;
	}
}	

void LED_Loop()
{
	if(MS1Start)
	{
		if(a)
		{
			a=0;
			P2=((P2&0x1f)|0x80);
			LEDStart&=0xfe;
			P0=LEDStart;
			P2&=0x1f;
		}
		else
		b=1;
	}
	if(MS2Start)
	{
		if(b)
		{
			b=0;
			P2=((P2&0x1f)|0x80);
			LEDStart|=0x01;
			P0=LEDStart;
			P2&=0x1f;
		}
		else
		a=1;
	}
	if(CStart)
	{
		if(c)
		{
			c=0;
			P2=((P2&0x1f)|0x80);
			LEDStart|=0x0e;
			LEDStart&=0xfd;
			P0=LEDStart;
			P2&=0x1f;
		}
		else
		d=1;
	}
	if(PStart)
	{
		if(d)
		{
			d=0;
			P2=((P2&0x1f)|0x80);
			LEDStart|=0x0e;
			LEDStart&=0xfb;
			P0=LEDStart;
			P2&=0x1f;
		}
		else
		e=1;
	}
	if(AStart)
	{
		if(e)
		{
			e=0;
			P2=((P2&0x1f)|0x80);
			LEDStart|=0x0e;
			LEDStart&=0xf7;
			P0=LEDStart;
			P2&=0x1f;
		}
		else
		c=1;
	}
}

void main()
{
	Init();
	Timer0Init();
	DS18B20_Read();
	Delay800ms();
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
		MS_Loop();
		if(DACStart)
		{
			DACStart=0;
			DAC_Write(DACData);
		}
		Mkey_Select();
		LED_Loop();
		if(CStart)
		C_Loop();
		if(PStart)
		P_Loop();
		if(AStart)
		A_Loop();
	}
}

