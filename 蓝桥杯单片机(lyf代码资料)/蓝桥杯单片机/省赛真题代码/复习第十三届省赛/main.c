#include <STC15F2K60S2.H>
#include "ds1302.h"
#include "onewire.h"
#include "intrins.h"

#define WP 0x8e

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,0xc1};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

code unsigned char DS1302WCommand[]={0x84,0x82,0x80};
code unsigned char DS1302RCommand[]={0x85,0x83,0x81};
unsigned char Time[]={0x17,0x59,0x50};

unsigned char MkeyNum,Hour,Min,Sec,CST=23,TCST,JDQState=0x00,LEDState=0xff;
		
unsigned int Count2,Count3;

float T;

bit MkeyStart,DS18B20Start,U1Start=1,U2Start,U2Start,U3Start,a=1,WDKZStart=1,SJKZStart;
bit b=1,c=1,JDQJSStart,d=1,e=1,JDQJSflag,JDQJSOver,f=1,g=1,L3Start,L3LStart,L3MStart,h;
bit TimeStart;
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
	P35=0;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=3;
	if(P32==0)
	Mkey=7;
	
	P35=1;
	P34=0;
	P3|=0x0f;
	if(P33==0)
	Mkey=4;
	if(P32==0)
	Mkey=8;
	
	return Mkey;
}

void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(NowState==3&&LastState==0)
	MkeyNum=3;
	if(NowState==4&&LastState==0)
	MkeyNum=4;
	if(NowState==7&&LastState==0)
	MkeyNum=7;
	if(NowState==8&&LastState==0)
	MkeyNum=8;
	if(U2Start)
	{
		if(NowState==0&&LastState==8)
		MkeyNum=8;
	}
}
			
void Time_Write()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<3;i++)
	Write_Ds1302_Byte(DS1302WCommand[i],Time[i]);
	Write_Ds1302_Byte(WP,0xff);
}

void Time_Read()
{
	unsigned char i;
	for(i=0;i<3;i++)
	Time[i]=Read_Ds1302_Byte(DS1302RCommand[i]);
	Hour=Time[0]/16*10+Time[0]%16;
	Min=Time[1]/16*10+Time[1]%16;
	Sec=Time[2]/16*10+Time[2]%16;
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
	static unsigned char Count0,Count1,Count4;
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
	if(++Count4>=25)
	{
		Count4=0;
		TimeStart=1;
	}
	if(JDQJSStart)
	{
		if(h==0)
		{
			h=1;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfe;
			P0=LEDState;
			P2&=0x1f;
			L3Start=1;
			L3LStart=1;
		}
		if(++Count2>=2500)
		{
			Count2=0;
			JDQJSStart=0;
			JDQJSOver=1;
			h=0;
		}
	}
	if(L3Start)
	{
		if(++Count3>=50)
		{
			Count3=0;
			if(L3LStart)
			{
				L3LStart=0;
				L3MStart=1;
			}
			else
			{
				L3LStart=1;
				L3MStart=0;
			}
		}
	}
	
}

void U1_Loop()
{
	Temp[0]=22;
	Temp[1]=1;
	Temp[3]=21;
	Temp[4]=21;
	Temp[5]=(int)(T)/10;
	Temp[6]=(int)(T)%10+10;
	Temp[7]=(int)(T*10)%10;
}

void U2_Loop()
{
	if(a)
	{
		Temp[0]=22;
		Temp[1]=2;
		Temp[3]=Time[0]/16;
		Temp[4]=Time[0]%16;
		Temp[5]=20;
		Temp[6]=Time[1]/16;
		Temp[7]=Time[1]%16;
	}
	else
	{
		Temp[0]=22;
		Temp[1]=2;
		Temp[3]=Time[1]/16;
		Temp[4]=Time[1]%16;
		Temp[5]=20;
		Temp[6]=Time[2]/16;
		Temp[7]=Time[2]%16;
	}
}

void U3_Loop()
{
	Temp[0]=22;
	Temp[1]=3;
	Temp[3]=21;
	Temp[4]=21;
	Temp[5]=21;
	Temp[6]=TCST/10;
	Temp[7]=TCST%10;
}

void WDKZ_Loop()
{
	if((int)(T*10)/10.0>CST)
	{
		if(b)
		{
			b=0;
			P2=((P2&0x1f)|0xa0);
			JDQState|=0x10;
			P0=JDQState;
			P2&=0x1f;
			L3Start=1;
			L3LStart=1;
		}
		else
		c=1;
	}
	else
	{
		if(c)
		{
			c=0;
			P2=((P2&0x1f)|0xa0);
			JDQState&=0x00;
			P0=JDQState;
			P2&=0x1f;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x04;
			P0=LEDState;
			P2&=0x1f;
			L3Start=0;
			L3LStart=0;
			L3MStart=0;
			Count3=0;
			f=1;
			g=1;
		}
		else
		b=1;
	}
}

void SJKZ_Loop()
{
	if(Min==0&&Sec==0)
	{
		JDQJSflag=1;
		P2=((P2&0x1f)|0xa0);
		JDQState|=0x10;
		P0=JDQState;
		P2&=0x1f;
	}
	if(JDQJSflag) //·ÀÖ¹¶à²é1S
	{
		JDQJSflag=0;
		JDQJSStart=1;
	}
	
	if(JDQJSOver)
	{
		JDQJSOver=0;
		P2=((P2&0x1f)|0xa0);
		JDQState&=0x00;
		P0=JDQState;
		P2&=0x1f;
		P2=((P2&0x1f)|0x80);
		LEDState|=0x01;
		P0=LEDState;
		P2&=0x1f;
		P2=((P2&0x1f)|0x80);
		LEDState|=0x04;
		P0=LEDState;
		P2&=0x1f;
		L3Start=0;
		L3LStart=0;
		L3MStart=0;
		Count3=0;
		f=1;
		g=1;
	}
}

void LED_Loop()
{
	if(WDKZStart)
	{
		if(d)
		{
			d=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfd;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		e=1;
	}
	if(SJKZStart)
	{
		if(e)
		{
			e=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x02;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		d=1;
	}
	if(L3LStart)
	{
		if(f)
		{
			f=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfb;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		g=1;
	}
	if(L3MStart)
	{
		if(g)
		{
			g=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x04;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		f=1;
	}
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 3:
		{
			MkeyNum=0;
			if(U1Start)
			{
				U1Start=0;
				U2Start=1;
			}else if(U2Start)
			{
				U3Start=1;
				U2Start=0;
				TCST=CST;
			}else if(U3Start)
			{
				U3Start=0;
				U1Start=1;
				CST=TCST;
			}
		}break;
		case 4:
		{
			MkeyNum=0;
			if(U3Start)
			{
				TCST++;
				if(TCST>99)
				TCST=10;
			}
		}break;
		case 7:
		{
			MkeyNum=0;
			if(WDKZStart)
			{
				WDKZStart=0;
				SJKZStart=1;
				P2=((P2&0x1f)|0xa0);
				JDQState&=0x00;
				P0=JDQState;
				P2&=0x1f;
				JDQJSflag=0;
				JDQJSStart=0;
				Count2=0;
				JDQJSOver=0;
				b=1;
				c=1;
				P2=((P2&0x1f)|0x80);
				LEDState|=0x04;
				P0=LEDState;
				P2&=0x1f;
				L3Start=0;
				L3LStart=0;
				L3MStart=0;
				Count3=0;
				f=1;
				g=1;
				h=0;
			}
			else
			{
				WDKZStart=1;
				SJKZStart=0;
				P2=((P2&0x1f)|0xa0);
				JDQState&=0x00;
				P0=JDQState;
				P2&=0x1f;
				JDQJSflag=0;
				JDQJSStart=0;
				Count2=0;
				JDQJSOver=0;
				b=1;
				c=1;
				P2=((P2&0x1f)|0x80);
				LEDState|=0x01;
				P0=LEDState;
				P2&=0x1f;
				P2=((P2&0x1f)|0x80);
				LEDState|=0x04;
				P0=LEDState;
				P2&=0x1f;
				L3Start=0;
				L3LStart=0;
				L3MStart=0;
				Count3=0;
				f=1;
				g=1;
				h=0;
			}
		}break;
		case 8:
		{
			MkeyNum=0;
			if(U3Start)
			{
				TCST--;
				if(TCST<10)
				TCST=99;
			}
			if(U2Start)
			{
				if(a)
				a=0;
				else
				a=1;
			}
		}break;
	}
}

void main()
{
	Init();
	Timer0Init();
	Time_Write();
	DS18B20_Read();
	Delay800ms();
	while(1)
	{
		if(TimeStart)
		{
			TimeStart=0;
			Time_Read();
		}
		
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
		if(WDKZStart)
		WDKZ_Loop();
		if(SJKZStart)
		SJKZ_Loop();
		if(U1Start)
		U1_Loop();
		if(U2Start)
		U2_Loop();
		if(U3Start)
		U3_Loop();
	}
}
