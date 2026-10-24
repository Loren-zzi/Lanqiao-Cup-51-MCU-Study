#include "STC15F2K60S2.H"
#include "onewire.h"
#include "ds1302.h"
#include "intrins.h"

#define WP 0x8e
code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0xc6};
unsigned char Temp[]={11,11,11,11,11,11,11,11,11};
code unsigned char TimeWCommand[]={0x84,0x82,0x80};
code unsigned char TimeRCommand[]={0x85,0x83,0x81};
unsigned char Time[]={0x23,0x59,0x50};
char Hour,Min,Sec,NHour=0,NMin=0,NSec=0,TNHour,TNMin,TNSec,THour,TMin,TSec;

unsigned int Count4=0;

unsigned char KeyNum=0,XSSZ=0,FZSZ=0,MSZ=0,NXSSZ,NFZSZ,NMSZ;

unsigned char Count1=0,Count2=0,Count3=0;

float T;

bit SJSZ=0,NZ=0,LedState=0,NZSZ=0,KG=1,WD=0;

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

unsigned char Key_State()
{
	unsigned char Key=0;
	if(P33==0)
	Key=1;
	if(P32==0)
	Key=2;
	if(P31==0)
	Key=3;
	if(P30==0)
	Key=4;
	return Key;
}

void Key_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Key_State();
	if(NowState==1&&LastState==0)
	KeyNum=1;
	if(SJSZ==0&&NZSZ==0)
	{
		if(NowState==0&&LastState==1)
		KeyNum=1;
	}
	if(NowState==2&&LastState==0)
	KeyNum=2;
	if(NowState==3&&LastState==0)
	KeyNum=3;
	if(NowState==4&&LastState==0)
	KeyNum=4;
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
}

void DS1302_Set()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<3;i++)
	Write_Ds1302_Byte(TimeWCommand[i],Time[i]);
	Write_Ds1302_Byte(WP,0x80);
}

void DS1302_Read()
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

void Time0_Loop() interrupt 1
{
	static unsigned char Count0;
	static unsigned int Count5;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		Key_Loop();
	}
	if(KG)
	{
		if(NHour==Hour&&NMin==Min&&NSec==Sec)
		{
			NZ=1;
			KG=0;
			P2=((P2&0x1f)|0x80);
			P0=0xfe;
			P2&=0x1f;
		}	
	}
	else
	{
		if(++Count5>=500)
		{
			Count5=0;
			KG=1;
		}
	}
	if(NZ)
	{
		if(++Count3>=100)
		{
			Count3=0;
			if(LedState==0)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				LedState=1;
			}
			else
			{
				P2=((P2&0x1f)|0x80);
				P0=0xfe;
				P2&=0x1f;
				LedState=0;
			}
		}
		if(++Count4>=3030)
		{
			Count4=0;
			NZ=0;
			P2=((P2&0x1f)|0x80);
			P0=0xff;
			P2&=0x1f;
			Count3=0;
			LedState=0;
		}
	}
	if(SJSZ)
	{
		if(++Count1>=50)
		{
			Count1=0;
			switch(XSSZ)
			{
				case 1:
				{
					Temp[0]=11;
					Temp[1]=11;
					Temp[2]=10;
					Temp[3]=TMin/10;
					Temp[4]=TMin%10;
					Temp[5]=10;
					Temp[6]=TSec/10;
					Temp[7]=TSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						XSSZ=2;
					}
				}break;
				case 2:
				{
					Temp[0]=THour/10;
					Temp[1]=THour%10;
					Temp[2]=10;
					Temp[3]=TMin/10;
					Temp[4]=TMin%10;
					Temp[5]=10;
					Temp[6]=TSec/10;
					Temp[7]=TSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						XSSZ=1;
					}
				}break;
			}
			switch(FZSZ)
			{
				case 1:
				{
					Temp[0]=THour/10;
					Temp[1]=THour%10;
					Temp[2]=10;
					Temp[3]=11;
					Temp[4]=11;
					Temp[5]=10;
					Temp[6]=TSec/10;
					Temp[7]=TSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						FZSZ=2;
					}
				}break;
				case 2:
				{
					Temp[0]=THour/10;
					Temp[1]=THour%10;
					Temp[2]=10;
					Temp[3]=TMin/10;
					Temp[4]=TMin%10;
					Temp[5]=10;
					Temp[6]=TSec/10;
					Temp[7]=TSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						FZSZ=1;
					}
				}break;
			}
			switch(MSZ)
			{
				case 1:
				{
					Temp[0]=THour/10;
					Temp[1]=THour%10;
					Temp[2]=10;
					Temp[3]=TMin/10;
					Temp[4]=TMin%10;
					Temp[5]=10;
					Temp[6]=11;
					Temp[7]=11;
					if(++Count2>=10)
					{
						Count2=0;
						MSZ=2;
					}
				}break;
				case 2:
				{
					Temp[0]=THour/10;
					Temp[1]=THour%10;
					Temp[2]=10;
					Temp[3]=TMin/10;
					Temp[4]=TMin%10;
					Temp[5]=10;
					Temp[6]=TSec/10;
					Temp[7]=TSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						MSZ=1;
					}
				}break;
			}
		}
	}
	if(NZSZ)
	{
		if(++Count1>=50)
		{
			Count1=0;
			switch(NXSSZ)
			{
				case 1:
				{
					Temp[0]=11;
					Temp[1]=11;
					Temp[2]=10;
					Temp[3]=TNMin/10;
					Temp[4]=TNMin%10;
					Temp[5]=10;
					Temp[6]=TNSec/10;
					Temp[7]=TNSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						NXSSZ=2;
					}
				}break;
				case 2:
				{
					Temp[0]=TNHour/10;
					Temp[1]=TNHour%10;
					Temp[2]=10;
					Temp[3]=TNMin/10;
					Temp[4]=TNMin%10;
					Temp[5]=10;
					Temp[6]=TNSec/10;
					Temp[7]=TNSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						NXSSZ=1;
					}
				}break;
			}
			switch(NFZSZ)
			{
				case 1:
				{
					Temp[0]=TNHour/10;
					Temp[1]=TNHour%10;
					Temp[2]=10;
					Temp[3]=11;
					Temp[4]=11;
					Temp[5]=10;
					Temp[6]=TNSec/10;
					Temp[7]=TNSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						NFZSZ=2;
					}
				}break;
				case 2:
				{
					Temp[0]=TNHour/10;
					Temp[1]=TNHour%10;
					Temp[2]=10;
					Temp[3]=TNMin/10;
					Temp[4]=TNMin%10;
					Temp[5]=10;
					Temp[6]=TNSec/10;
					Temp[7]=TNSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						NFZSZ=1;
					}
				}break;
			}
			switch(NMSZ)
			{
				case 1:
				{
					Temp[0]=TNHour/10;
					Temp[1]=TNHour%10;
					Temp[2]=10;
					Temp[3]=TNMin/10;
					Temp[4]=TNMin%10;
					Temp[5]=10;
					Temp[6]=11;
					Temp[7]=11;
					if(++Count2>=10)
					{
						Count2=0;
						NMSZ=2;
					}
				}break;
				case 2:
				{
					Temp[0]=TNHour/10;
					Temp[1]=TNHour%10;
					Temp[2]=10;
					Temp[3]=TNMin/10;
					Temp[4]=TNMin%10;
					Temp[5]=10;
					Temp[6]=TNSec/10;
					Temp[7]=TNSec%10;
					if(++Count2>=10)
					{
						Count2=0;
						NMSZ=1;
					}
				}break;
			}
		}
	}
}

void WD_Loop()
{
	if(KeyNum==1)
	{
		KeyNum=0;
		WD=0;
	}
	DS18B20_Read();
	DS1302_Read();
	Temp[0]=11;
	Temp[1]=11;
	Temp[2]=11;
	Temp[3]=11;
	Temp[4]=11;
	Temp[5]=(int)(T)/10;
	Temp[6]=(int)(T)%10;
	Temp[7]=12;
}

void SJ_Loop()
{
	DS1302_Read();
	Temp[0]=Time[0]/16;
	Temp[1]=Time[0]%16;
	Temp[2]=10;
	Temp[3]=Time[1]/16;
	Temp[4]=Time[1]%16;
	Temp[5]=10;
	Temp[6]=Time[2]/16;
	Temp[7]=Time[2]%16;
}

void SJSZ_Loop()
{
	DS1302_Read();
	switch(KeyNum)
	{
		case 4:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(XSSZ)
				{
					XSSZ=0;
					FZSZ=1;
					Count1=0;
					Count2=0;
					Temp[0]=THour/10;
					Temp[1]=THour%10;
					Temp[2]=10;
					Temp[3]=11;
					Temp[4]=11;
					Temp[5]=10;
					Temp[6]=TSec/10;
					Temp[7]=TSec%10;
					break;
				}
			}
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(FZSZ)
				{
					FZSZ=0;
					MSZ=1;
					Count1=0;
					Count2=0;
					Temp[0]=THour/10;
					Temp[1]=THour%10;
					Temp[2]=10;
					Temp[3]=TMin/10;
					Temp[4]=TMin%10;
					Temp[5]=10;
					Temp[6]=11;
					Temp[7]=11;
					break;
				}
			}
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(MSZ)
				{
					MSZ=0;
					SJSZ=0;
					Count1=0;
					Count2=0;
					Time[0]=THour/10*16+THour%10;
					Time[1]=TMin/10*16+TMin%10;
					Time[2]=TSec/10*16+TSec%10;
					DS1302_Set();
					break;
				}
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(XSSZ==1||XSSZ==2)
				{
					THour++;
					if(THour>23)
					THour=0;
				}
				if(FZSZ==1||FZSZ==2)
				{
					TMin++;
					if(TMin>59)
					TMin=0;
				}if(MSZ==1||MSZ==2)
				{
					TSec++;
					if(TSec>59)
					TSec=0;
				}
			}
		}break;
		case 1:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(XSSZ==1||XSSZ==2)
				{
					THour--;
					if(THour<0)
					THour=23;
				}
				if(FZSZ==1||FZSZ==2)
				{
					TMin--;
					if(TMin<0)
					TMin=59;
				}
				if(MSZ==1||MSZ==2)
				{
					TSec--;
					if(TSec<0)
					TSec=59;
				}
			}
		}break;
	}
}

void NZSZ_Loop()
{
	DS1302_Read();
	switch(KeyNum)
	{
		case 3:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(NXSSZ)
				{
					NXSSZ=0;
					NFZSZ=1;
					Count1=0;
					Count2=0;
					Temp[0]=TNHour/10;
					Temp[1]=TNHour%10;
					Temp[2]=10;
					Temp[3]=11;
					Temp[4]=11;
					Temp[5]=10;
					Temp[6]=TNSec/10;
					Temp[7]=TNSec%10;
					break;
				}
			}
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(NFZSZ)
				{
					NFZSZ=0;
					NMSZ=1;
					Count1=0;
					Count2=0;
					Temp[0]=TNHour/10;
					Temp[1]=TNHour%10;
					Temp[2]=10;
					Temp[3]=TNMin/10;
					Temp[4]=TNMin%10;
					Temp[5]=10;
					Temp[6]=11;
					Temp[7]=11;
					break;
				}
			}
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(NMSZ)
				{
					NMSZ=0;
					NZSZ=0;
					Count1=0;
					Count2=0;
					NHour=TNHour;
					NMin=TNMin;
					NSec=TNSec;
					break;
				}
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(NXSSZ==1||NXSSZ==2)
				{
					TNHour++;
					if(TNHour>23)
					TNHour=0;
				}
				if(NFZSZ==1||NFZSZ==2)
				{
					TNMin++;
					if(TNMin>59)
					TNMin=0;
				}
				if(NMSZ==1||NMSZ==2)
				{
					TNSec++;
					if(TNSec>59)
					TNSec=0;
				}
			}
		}break;
		case 1:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
				break;
			}
			else
			{
				if(NXSSZ==1||NXSSZ==2)
				{
					TNHour--;
					if(TNHour<0)
					TNHour=23;
				}
				if(NFZSZ==1||NFZSZ==2)
				{
					TNMin--;
					if(TNMin<0)
					TNMin=59;
				}
				if(NMSZ==1||NMSZ==2)
				{
					TNSec--;
					if(TNSec<0)
					TNSec=59;
				}
			}
		}break;
	}
}

void Key_Select()
{
	switch(KeyNum)
	{
		case 4:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
			}
			else
			{
				SJSZ=1;
				XSSZ=1;
				THour=Time[0]/16*10+Time[0]%16;
				TMin=Time[1]/16*10+Time[1]%16;
				TSec=Time[2]/16*10+Time[2]%16;
				Temp[0]=11;
				Temp[1]=11;
				Temp[2]=10;
				Temp[3]=TMin/10;
				Temp[4]=TMin%10;
				Temp[5]=10;
				Temp[6]=TSec/10;
				Temp[7]=TSec%10;
			}
		}break;
		case 3:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
			}
			else
			{
				NZSZ=1;
				NXSSZ=1;
				TNHour=NHour;
				TNMin=NMin;
				TNSec=NSec;
				Temp[0]=11;
				Temp[1]=11;
				Temp[2]=10;
				Temp[3]=TNMin/10;
				Temp[4]=TNMin%10;
				Temp[5]=10;
				Temp[6]=TNSec/10;
				Temp[7]=TNSec%10;
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
			}
		}break;
		case 1:
		{
			KeyNum=0;
			if(NZ)
			{
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
				NZ=0;
				Count3=0;
				Count4=0;
				LedState=0;
			}
			else
			{
				WD=1;
			}
		}break;
	}
}

void main()
{
	Init();
	DS18B20_Read();
	Delay800ms();
	DS1302_Set();
	Timer0Init();
	while(1)
	{
		SJ_Loop();
		Key_Select();
		while(NZSZ)
		NZSZ_Loop();
		while(SJSZ)
		SJSZ_Loop();
		while(WD)
		WD_Loop();
	}
}

