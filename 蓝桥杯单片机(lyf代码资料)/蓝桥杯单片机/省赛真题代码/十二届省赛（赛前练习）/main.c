#include <STC15F2K60S2.H>
#include "onewire.h"
#include "intrins.h"
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,0xc6,0x8c,0x88};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char MkeyNum,CST=25,LEDState=0xff;
char SZT=0;

unsigned int DACData;

float T,TT;

bit MkeyStart=0,DS18B20Start=0,WD=1,CSSZ=0,DAC=0,MS1=1,MS2=0,a=0,b=0,c=0,d=0,e=0,f=0,g=0;

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
	
	P44=1;
	P42=0;
	P35=1;
	P34=1;
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
	if(LastState==0&&NowState==1)
	MkeyNum=1;
	if(LastState==0&&NowState==2)
	MkeyNum=2;
	if(LastState==0&&NowState==5)
	MkeyNum=5;
	if(LastState==0&&NowState==6)
	MkeyNum=6;
}

void DS18B20_Read()
{
	unsigned char TMSB,TLSB;
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
	TT=(int)(T*1000)/100.0;
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
	static unsigned char Count0,Count1;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		MkeyStart=1;
	}
	if(++Count1>=50)
	{
		Count1=0;
		DS18B20Start=1;
	}
}

void WD_Loop()
{
	Temp[0]=22;
	Temp[4]=(int)(T)/10;
	Temp[5]=(int)(T)%10+10;
	Temp[6]=(int)(T*100)%100/10;
	Temp[7]=(int)(T*100)%10;
}

void CSSZ_Loop()
{
	Temp[0]=23;
	Temp[4]=21;
	Temp[5]=21;
	Temp[6]=SZT/10;
	Temp[7]=SZT%10;
}

void DAC_Loop()
{
	Temp[0]=24;
	Temp[4]=21;
	Temp[5]=DACData/100+10;
	Temp[6]=DACData%100/10;
	Temp[7]=DACData%10;
}

void DACMS1_Loop()
{
	if(T<CST)
	{
		if(a==0)
		{
			a=1;
			b=0;
			DAC_Write(0);
			DACData=0;
		}
	}
	else
	{
		if(b==0)
		{
			a=0;
			b=1;
			DAC_Write(255);
			DACData=500;
		}
	}
}

void LED_Loop()
{
	if(WD)
	{
		if(c==0)
		{
			c=1;
			d=0;
			e=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x07;
			LEDState&=0xfe;
			P0=LEDState;
			P2&=0x1f;
		}
	}
	if(CSSZ)
	{
		if(d==0)
		{
			c=0;
			d=1;
			e=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x07;
			LEDState&=0xfd;
			P0=LEDState;
			P2&=0x1f;
		}
	}
	if(DAC)
	{
		if(e==0)
		{
			c=0;
			d=0;
			e=1;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x07;
			LEDState&=0xfb;
			P0=LEDState;
			P2&=0x1f;
		}
	}
	if(MS1)
	{
		if(f==0)
		{
			f=1;
			g=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x08;
			P0=LEDState;
			P2&=0x1f;
		}
	}
	if(MS2)
	{
		if(g==0)
		{
			g=1;
			f=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xf7;
			P0=LEDState;
			P2&=0x1f;
		}
	}
}

void DACMS2_Loop()
{
	if(T<=20)
	{
		DAC_Write(51);
		DACData=100;
	}
	if(T>=40)
	{
		DAC_Write(51);
		DACData=100;
	}
	if(T>20&&T<40)
	{
		DACData=(int)(15*T)-200;
		DAC_Write((int)(DACData/500.0*255));
	}
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 1:
		{
			MkeyNum=0;
			if(WD)
			{
				WD=0;
				CSSZ=1;
				SZT=CST;
				c=0;
				d=0;
				e=0;
				break;
			}
			if(CSSZ)
			{
				CSSZ=0;
				DAC=1;
				CST=SZT;
				c=0;
				d=0;
				e=0;
				break;
			}
			if(DAC)
			{
				DAC=0;
				WD=1;
				c=0;
				d=0;
				e=0;
				break;
			}
		}break;
		case 2:
		{
			MkeyNum=0;
			if(CSSZ)
			{
				SZT--;
				if(SZT<0)
				SZT=99;
			}
		}break;
		case 5:
		{
			MkeyNum=0;
			if(MS1)
			{
				MS1=0;
				MS2=1;
				a=0;
				b=0;
				g=0;
				f=0;
				break;
			}
			if(MS2)
			{
				MS1=1;
				MS2=0;
				g=0;
				f=0;
				break;
			}
		}break;
		case 6:
		{
			MkeyNum=0;
			if(CSSZ)
			{
				SZT++;
				if(SZT>99)
				SZT=0;
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
	Timer0Init();
	while(1)
	{
		if(DS18B20Start)
		{
			DS18B20Start=0;
			DS18B20_Read();
		}
		
		if(MkeyStart)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		Mkey_Select();
		LED_Loop();
		if(MS1)
		DACMS1_Loop();
		if(MS2)
		DACMS2_Loop();
		if(WD)
		WD_Loop();
		if(CSSZ)
		CSSZ_Loop();
		if(DAC)
		DAC_Loop();
	}
}
