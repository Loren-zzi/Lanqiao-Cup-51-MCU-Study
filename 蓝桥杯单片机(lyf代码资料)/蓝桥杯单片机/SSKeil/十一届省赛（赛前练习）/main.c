#include <STC15F2K60S2.H>
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,0xc1,0x8c,0xc8};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned int UData,L1Count=0;

unsigned char MkeyNum,AT24C02Data,ADCData,JS=0,LEDState=0xff,CountL3=0;

int CSU,SZU=0,TU;

bit UStart=1,PStart=0,JSStart=0,AT24C02Start=0,L1Start=0,a=0;

void Delay5ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 59;
	j = 90;
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

void AT24C02_Write(unsigned char Command,Data)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(Command);
	I2CWaitAck();
	I2CSendByte(Data);
	I2CWaitAck();
	I2CStop();
}

unsigned char AT24C02_Read(unsigned char Command)
{
	unsigned char Data;
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(Command);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0xa0|0x01);
	I2CWaitAck();
	Data=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return Data;
}

unsigned char ADC_Read()
{
	unsigned char Data;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x03);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0x90|0x01);
	I2CWaitAck();
	Data=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return Data;
}

void UJS_Loop()
{
	static unsigned int ULastState,UNowState;
	ULastState=UNowState;
	UNowState=UData;
	if(UNowState<CSU&&ULastState>CSU)
	JS++;
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
		Mkey_Loop();
	}
	if(++Count1>=50)
	{
		Count1=0;
		ADCData=ADC_Read();
		UData=ADCData/255.0*500;
		UJS_Loop();
		if(UData<CSU)
		{
			L1Start=1;
		}
		else
		{
			L1Start=0;
			L1Count=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x01;
			P0=LEDState;
			P2&=0x1f;
		}
		if(JS%2!=0)
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
		if(JS>99)
		JS=0;
	}
	if(AT24C02Start)
	{
		AT24C02Start=0;
		AT24C02_Write(0,CSU/10);
	}
	if(L1Start)
	{
		if(++L1Count>=2500)
		{
			L1Count=0;
			L1Start=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfe;
			P0=LEDState;
			P2&=0x1f;
		}
	}
	if(CountL3>=3&&a==0)
	{
		a=1;
		P2=((P2&0x1f)|0x80);
		LEDState&=0xfb;
		P0=LEDState;
		P2&=0x1f;
	}
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 3:
		{
			MkeyNum=0;
			CountL3=0;
			a=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x04;
			P0=LEDState;
			P2&=0x1f;
			if(UStart)
			{
				UStart=0;
				PStart=1;
				SZU=CSU;
				TU=SZU;
				break;
			}
			if(PStart)
			{
				PStart=0;
				JSStart=1;
				CSU=SZU;
				AT24C02Start=1;
				if(TU!=CSU)
				{
					L1Count=0;
					L1Start=0;
					P2=((P2&0x1f)|0x80);
					LEDState|=0x01;
					P0=LEDState;
					P2&=0x1f;
				}
				break;
			}
			if(JSStart)
			{
				JSStart=0;
				UStart=1;
				break;
			}
		}break;
		case 7:
		{
			MkeyNum=0;
			if(JSStart)
			{
				JS=0;
				CountL3=0;
				a=0;
				P2=((P2&0x1f)|0x80);
				LEDState|=0x04;
				P0=LEDState;
				P2&=0x1f;
			}
			else
			{
				CountL3++;
				if(CountL3>=3)
				CountL3=3;
			}
		}break;
		case 4:
		{
			MkeyNum=0;
			if(PStart)
			{
				SZU+=50;
				if(SZU>500)
				SZU=0;
				CountL3=0;
				a=0;
				P2=((P2&0x1f)|0x80);
				LEDState|=0x04;
				P0=LEDState;
				P2&=0x1f;
			}
			else
			{
				CountL3++;
				if(CountL3>=3)
				CountL3=3;
			}
		}break;
		case 8:
		{
			MkeyNum=0;
			if(PStart)
			{
				SZU-=50;
				if(SZU<0)
				SZU=500;
				CountL3=0;
				a=0;
				P2=((P2&0x1f)|0x80);
				LEDState|=0x04;
				P0=LEDState;
				P2&=0x1f;
			}
			else
			{
				CountL3++;
				if(CountL3>=3)
				CountL3=3;
			}
		}break;
		case 1:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 2:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 5:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 6:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 9:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 10:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 11:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 12:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 13:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 14:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 15:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
		case 16:
		{
			MkeyNum=0;
			CountL3++;
			if(CountL3>=3)
			CountL3=3;
		}break;
	}
}

void U_Loop()
{
	Temp[0]=22;
	Temp[5]=UData/100+10;
	Temp[6]=UData%100/10;
	Temp[7]=UData%10;
}

void P_Loop()
{
	Temp[0]=23;
	Temp[5]=SZU/100+10;
	Temp[6]=SZU%100/10;
	Temp[7]=SZU%10;
}

void JS_Loop()
{
	Temp[0]=24;
	Temp[5]=21;
	Temp[6]=JS/10;
	Temp[7]=JS%10;
}

void main()
{
	Init();
	AT24C02Data=AT24C02_Read(0);
	if(AT24C02Data>=0&&AT24C02Data<=50&&AT24C02Data%5==0)
	CSU=AT24C02Data*10;
	else
	CSU=250;
	ADC_Read();
	ADC_Read();
	ADCData=ADC_Read();
	UData=ADCData/255.0*500;
	Timer0Init();
	while(1)
	{
		Mkey_Select();
		if(UStart)
		U_Loop();
		if(PStart)
		P_Loop();
		if(JSStart)
		JS_Loop();
	}
}


