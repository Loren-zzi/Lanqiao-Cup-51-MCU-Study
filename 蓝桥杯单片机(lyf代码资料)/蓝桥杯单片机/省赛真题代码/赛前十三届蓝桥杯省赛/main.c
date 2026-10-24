#include <STC15F2K60S2.H>
#include "onewire.h"
#include "ds1302.h"
#include "intrins.h"

#define WP 0x8e
code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,0xc1};
unsigned char Temp[]={21,21,21,21,21,21,21,21};
code unsigned char TimeWCommand[]={0x84,0x82,0x80};
code unsigned char TimeRCommand[]={0x85,0x83,0x81};

unsigned char Time[]={0x07,0x59,0x50};

unsigned char MkeyNum,Hour,Min,Sec,CST=23,TT,JDQState=0xaf,LEDState=0xff,Count5=0;
float T;

unsigned int Count2=0,Count3=0,Count6=0;
bit L3L=0;L3M=0,SJStart=0,c=0,d=0;
bit MkeyStart=0,WD=1,SF=0,SZ=0,FM=0,WDKZ=1,SJKZ=0,JSKG=0,SKG=1,L3Start=0,L3State=0,JDKG=0;

void L3_Loop();
void SJKZ_Loop();
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
	P44=1;
	P42=1;
	P35=0;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=3;
	if(P32==0)
	Mkey=7;
	
	P44=1;
	P42=1;
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
	if(SF)
	{
		if(NowState==0&&LastState==8)
		MkeyNum=8;
	}
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
}

void Time_Set()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<3;i++)
	Write_Ds1302_Byte(TimeWCommand[i],Time[i]);
	Write_Ds1302_Byte(WP,0x80);
}

void Time_Read()
{
	unsigned char i;
	for(i=0;i<3;i++)
	Time[i]=Read_Ds1302_Byte(TimeRCommand[i]);
	Hour=Time[0]/16*10+Time[0]%16;
	Min=Time[1]/16*10+Time[1]%16;
	Sec=Time[2]/16*10+Time[2]%16;
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
	if(++Count1>=25)
	{
		Count1=0;
		SJStart=1;
	}
	if(SKG==0)
	{
		if(++Count3>=500)
		{
			Count3=0;
			SKG=1;
		}
	}
	if(JSKG)
	{
		if(++Count2>=3000)
		{
			Count2=0;
			JDKG=1;
			JSKG=0;
		}
	}
	if(L3Start)
	{
		if(++Count5>=50)
		{
			Count5=0;
			L3_Loop();
		}
	}
}

void WD_Loop()
{
	Temp[0]=22;
	Temp[1]=1;
	Temp[3]=21;
	Temp[4]=21;
	Temp[5]=(int)(T)/10;
	Temp[6]=(int)(T)%10+10;
	Temp[7]=(int)(T*10)%10;
}

void SF_Loop() //Ê±·Ö
{
	//Time_Read();
	Temp[0]=22;
	Temp[1]=2;
	Temp[3]=Hour/10;
	Temp[4]=Hour%10;
	Temp[5]=20;
	Temp[6]=Min/10;
	Temp[7]=Min%10;
}

void FM_Loop() //·ÖÃë
{
	if(MkeyStart)
	{
		MkeyStart=0;
		Mkey_Loop();
	}
	if(MkeyNum==8)
	{
		MkeyNum=0;
		FM=0;
	}
	SJKZ_Loop();
	if(SJStart)
	{
		SJStart=0;
		Time_Read();
	}	Temp[0]=22;
	Temp[1]=2;
	Temp[3]=Min/10;
	Temp[4]=Min%10;
	Temp[5]=20;
	Temp[6]=Sec/10;
	Temp[7]=Sec%10;
}

void SZ_Loop()
{
	Temp[0]=22;
	Temp[1]=3;
	Temp[3]=21;
	Temp[4]=21;
	Temp[5]=21;
	Temp[6]=TT/10;
	Temp[7]=TT%10;
}

void WDKZ_Loop()
{
	if(T>CST)
	{
		if(c==0)
		{
			c=1;
			d=0;
			Delay2ms();
			P2=((P2&0x1f)|0xa0);
			JDQState|=0x10;
			P0=JDQState;
			P2&=0x1f;
			Delay2ms();
			L3Start=1;
			Delay2ms();
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfb;
			P0=LEDState;
			P2&=0x1f;
			Delay2ms();
		}
		else
		{
			c=1;
		}
	}
	else
	{
		if(d==0)
		{
			d=1;
			c=0;
			Delay2ms();
			P2=((P2&0x1f)|0xa0);
			JDQState&=0xef;
			P0=JDQState;
			P2&=0x1f;
			Delay2ms();
			L3Start=0;
			L3State=0;
			Count5=0;
			Delay2ms();
			P2=((P2&0x1f)|0x80);
			LEDState|=0x04;
			P0=LEDState;
			P2&=0x1f;
			Delay2ms();
		}
		else
		{
			d=1;
		}
	}
}

void SJKZ_Loop()
{

		if(SKG)
		{
			if(Min==0&&Sec==0)
			{
				Delay2ms();
				P2=((P2&0x1f)|0xa0);
				JDQState|=0x10;
				P0=JDQState;
				P2&=0x1f;
				Delay2ms();
				Delay2ms();
				P2=((P2&0x1f)|0x80);
				LEDState&=0xfe;
				P0=LEDState;
				P2&=0x1f;
				Delay2ms();
				JSKG=1;
				SKG=0;
				L3Start=1;
				Delay2ms();
				P2=((P2&0x1f)|0x80);
				LEDState&=0xfb;
				P0=LEDState;
				P2&=0x1f;
				Delay2ms();
				
			}
		}
	
		if(JDKG)
		{
			Delay2ms();
			JDKG=0;
			P2=((P2&0x1f)|0xa0);
			JDQState&=0xef;
			P0=JDQState;
			P2&=0x1f;
			Delay2ms();
			Delay2ms();
			P2=((P2&0x1f)|0x80);
			LEDState|=0x01;
			P0=LEDState;
			P2&=0x1f;
			Delay2ms();
			JSKG=0;
			L3Start=0;
			L3State=0;
			Count5=0;
			Delay2ms();
			P2=((P2&0x1f)|0x80);
			LEDState|=0x04;
			P0=LEDState;
			P2&=0x1f;
			Delay2ms();
		}
	
}

void LED_Loop()
{
	static unsigned char a=0,b=0;
	if(WDKZ)
	{
		if(a==0)
		{
			a=1;
			b=0;
			Delay2ms();
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfd;
			P0=LEDState;
			P2&=0x1f;
			Delay2ms();
		}
		else
		{
			a=1;
		}
	}
	else
	{
		if(b==0)
		{
			a=0;
			b=1;
			Delay2ms();
			P2=((P2&0x1f)|0x80);
			LEDState|=0x02;
			P0=LEDState;
			P2&=0x1f;
			Delay2ms();
		}
		else
		{
			b=1;
		}
	}
}

void L3_Loop()
{
	if(L3State==0)
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x04;
		P0=LEDState;
		P2&=0x1f;
		L3State=1;
	}
	else
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xfb;
		P0=LEDState;
		P2&=0x1f;
		L3State=0;
	}
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 3:
		{
			MkeyNum=0;
			if(WD)
			{
				WD=0;
				SF=1;
				break;
			}
			if(SF)
			{
				SF=0;
				SZ=1;
				TT=CST;
				break;
			}
			if(SZ)
			{
				SZ=0;
				WD=1;
				CST=TT;
				c=0;
				d=0;
				break;
			}
		}break;
		case 4:
		{
			MkeyNum=0;
			if(SZ)
			{
				TT++;
				if(TT>99)
				TT=10;
			}
		}break;
		case 7:
		{
			MkeyNum=0;
			if(WDKZ)
			{
				WDKZ=0;
				SJKZ=1;

				Delay2ms();
				P2=((P2&0x1f)|0xa0);
				JDQState=0x00;
				P0=JDQState;
				P2&=0x1f;
				Delay2ms();
				L3Start=0;
				L3State=0;
				Count5=0;
				P2=((P2&0x1f)|0x80);
				LEDState|=0x04;
				P0=LEDState;
				P2&=0x1f;
				Delay2ms();
				c=0;
				d=0;
				break;
			}
			if(SJKZ)
			{
				WDKZ=1;
				SJKZ=0;
				Count2=0;
				Count3=0;
				JSKG=0;
				P2=((P2&0x1f)|0xa0);
				JDQState=0x00;
				P0=JDQState;
				P2&=0x1f;
				Delay2ms();
				L3Start=0;
				L3State=0;
				Count5=0;
				P2=((P2&0x1f)|0x80);
				LEDState|=0x04;
				P0=LEDState;
				P2&=0x1f;
				Delay2ms();
				Delay2ms();
				P2=((P2&0x1f)|0x80);
				LEDState|=0x01;
				P0=LEDState;
				P2&=0x1f;
				Delay2ms();
				break;
			}
		}break;
		case 8:
		{
			MkeyNum=0;
			if(SF)
			FM=1;
			if(SZ)
			{
				TT--;
				if(TT<10)
				TT=99;
			}
		}break;
	}
}

void main()
{
	Init();
	Time_Set();
	Time_Read();
	DS18B20_Read();
	Delay800ms();
	Timer0Init();
	P2=((P2&0x1f)|0x80);
	LEDState&=0xfd;
	P0=LEDState;
	P2&=0x1f;
	while(1)
	{
		DS18B20_Read();
		if(SJStart)
		{
			SJStart=0;
			Time_Read();
		}
		if(MkeyStart)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		Mkey_Select();
		if(WDKZ)
		WDKZ_Loop();
		if(SJKZ)
		SJKZ_Loop();
		LED_Loop();
		if(WD)
		WD_Loop();
		if(SF)
		SF_Loop();
		if(SZ)
		SZ_Loop();
		while(FM)
		FM_Loop();
	}
}
