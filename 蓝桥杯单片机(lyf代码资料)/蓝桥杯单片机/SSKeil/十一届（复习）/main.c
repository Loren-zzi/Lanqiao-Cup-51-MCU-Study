#include <STC15F2K60S2.H>
#include "iic.h"


code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x08,0x00,0x10,0xBF,0xff,0xc1,0x8c,0xc8};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned int UData;

int TU,TTU;

unsigned char MkeyNum,ADCData,AT24C02Data,JM=1,JS,LEDState=0xff,Count3,Count4;

bit UState=1,PState,NState,L1State;

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

void AT24C02_Write(unsigned char Command,Data)
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(Command);
	IIC_WaitAck();
	IIC_SendByte(Data);
	IIC_WaitAck();
	IIC_Stop();
}

unsigned char AT24C02_Read(unsigned char Command)
{
	unsigned char Data;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(Command);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0xa0|0x01);
	IIC_WaitAck();
	Data=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return Data;
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

void U_State()
{
	static unsigned int NowState,LastState;
	LastState=NowState;
	NowState=UData;
	if(LastState>TU&&NowState<TU)
	JS++;
	if(UData<TU)
	Count3++;
	else
	Count3=0;
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count0,Count1,Count2;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		Mkey_Loop();
	}
	if(++Count1>=100)
	{
		Count1=0;
		ADCData=ADC_Read();
		UData=ADCData/255.0*500;
	}
	if(++Count2>=20)
	{
		Count2=0;
		U_State();
		if(Count3>=125)
		{
			Count3=125;
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
	}
}

void U_Loop()
{
	Temp[0]=22;
	Temp[5]=UData/100+10;
	Temp[6]=UData%100/10;
	Temp[7]=UData%100%10;
}

void P_Loop()
{
	Temp[0]=23;
	if(TTU>500)
	TTU=0;
	if(TTU<0)
	TTU=500;
	Temp[5]=TTU/100+10;
	Temp[6]=TTU%100/10;
	Temp[7]=TTU%100%10;
}

void N_Loop()
{
	Temp[0]=24;
	Temp[5]=21;
	Temp[6]=JS/10;
	Temp[7]=JS%10;
}

void LED_Loop()
{
	if(JS%2==0)
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x02;
		P0=LEDState;
		P2&=0x1f;
	}
	else
	{
		P2=((P2&0x1f)|0x80);	
		LEDState&=0xfd;
		P0=LEDState;
		P2&=0x1f;
	}
	if(Count4>=3)
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
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 3:
		{
			MkeyNum=0;
			switch(JM)
			{
				case 1:
				{
					UState=0;
					PState=1;
					NState=0;
					TTU=TU;
					Count4=0;
					JM=2;
				}break;
				case 2:
				{
					UState=0;
					PState=0;
					NState=1;
					Count4=0;
					if(TU!=TTU)
					JS=0;
					TU=TTU;
					AT24C02_Write(0,TU/10);
					Delay5ms();
					JM=3;
				}break;
				case 3:
				{
					UState=1;
					PState=0;
					NState=0;
					Count4=0;
					JM=1;
				}break;
			}
		}break;
		case 4:
		{
			MkeyNum=0;
			if(PState)
			{
				TTU+=50;
				Count4=0;
			}
			else
			{
				Count4++;
				if(Count4>=3)
				Count4=3;
			}
		}break;
		case 8:
		{
			MkeyNum=0;
			if(PState)
			{
				TTU-=50;
				Count4=0;
			}
			else
			{
				Count4++;
				if(Count4>=3)
				Count4=3;
			}
		}break;
		case 7:
		{
			MkeyNum=0;
			if(NState)
			{
				JS=0;
				Count4=0;
			}
			else
			{
				Count4++;
				if(Count4>=3)
				Count4=3;
			}
		}break;
		case 1:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 2:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 5:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 6:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 9:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 10:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 11:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 12:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 13:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 14:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 15:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
		case 16:
		{
			MkeyNum=0;
			Count4++;
			if(Count4>=3)
			Count4=3;
		}break;
	}
}

void main()
{
	Init();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	UData=ADCData/255.0*500;	
	Timer0Init();
	AT24C02Data=AT24C02_Read(0);
	if(AT24C02Data<0||AT24C02Data>50||AT24C02Data%5!=0)
	AT24C02Data=0;
	TU=AT24C02Data*10;
	while(1)
	{
		Mkey_Select();
		LED_Loop();
		if(UState)
		U_Loop();
		if(PState)
		P_Loop();
		if(NState)
		N_Loop();
	}
}
