#include <STC15F2K60S2.H>
#include "intrins.h"
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xc1,0x8c,0xc8};
unsigned char Temp[]={20,20,20,20,20,20,20,20};

unsigned char MkeyNum,ADCData,AT24C02Data,JM,L1Count,LState=0xff,L2Count,L3Count,Count;

int SZU,TSZU;

unsigned int UData;

bit UStart=1,PStart,NStart,ADC=1;

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

void Timer0Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x40;		//设置定时初值
	TH0 = 0xA2;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
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

void SZU_Loop()
{
	static unsigned int NowState,LastState;
	LastState=NowState;
	NowState=UData;
	if(LastState>SZU&&NowState<SZU)
	Count++;
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count0;
	static unsigned int Count1;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		Mkey_Loop();
		if(L3Count>=3)
		{
			P2=((P2&0x1f)|0x80);
			LState&=0xfb;
			P0=LState;
			P2&=0x1f;
		}
		if(L3Count<3)
		{
			P2=((P2&0x1f)|0x80);
			LState|=0x04;
			P0=LState;
			P2&=0x1f;
		}
	}
	if(ADC==1)
	{
		if(++Count1>=50)
		{
			Count1=0;
			ADCData=ADC_Read();
			UData=ADCData/255.0*500;
			SZU_Loop();
			if(Count%2!=0)			
			{
				P2=((P2&0x1f)|0x80);
				LState&=0xfd;
				P0=LState;
				P2&=0x1f;
			}
			if(Count%2==0)
			{
				P2=((P2&0x1f)|0x80);
				LState|=0x02;
				P0=LState;
				P2&=0x1f;
			}
			if(UData<SZU)
			{
				L1Count++;
			}
			if(UData>=SZU)
			{
				L1Count=0;
				P2=((P2&0x1f)|0x80);
				LState|=0x01;
				P0=LState;
				P2&=0x1f;
			}
			if(L1Count>=50)
			{
				L1Count=50;
				P2=((P2&0x1f)|0x80);
				LState&=0xfe;
				P0=LState;
				P2&=0x1f;
			}
		}
	}
}

void U_Loop()
{
	Temp[0]=21;
	Temp[5]=UData/100+10;
	Temp[6]=UData%100/10;
	Temp[7]=UData%100%10;
}

void P_Loop()
{
	Temp[0]=22;
	if(TSZU>500)
	{
		TSZU=0;
	}
	if(TSZU<0)
	{
		TSZU=500;
	}
	Temp[5]=TSZU/100+10;
	Temp[6]=TSZU%100/10;
	Temp[7]=TSZU%100%10;
}

void N_Loop()
{
	Temp[0]=23;
	if(Count<100&&Count>=10)
	{
		Temp[5]=20;
		Temp[6]=Count/10;
		Temp[7]=Count%10;
	}
	if(Count<10)
	{
		Temp[5]=20;
		Temp[6]=20;
		Temp[7]=Count%10;
	}
	if(Count>=100)
	Count=1;
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
				case 0:
				{
					TSZU=SZU;
					UStart=0;
					PStart=1;
					NStart=0;
					JM=1;
					L3Count=0;
				}break;
				case 1:
				{
					UStart=0;
					PStart=0;
					NStart=1;
					L3Count=0;
					if(TSZU!=SZU)
					{
						Count=0;
						L1Count=0;
						P2=((P2&0x1f)|0x80);
						LState|=0x01;
						P0=LState;
						P2&=0x1f;
					}
					if(ADC==1)
					{
						ADC=0;
						AT24C02_Write(0,TSZU/10);
						Delay5ms();
						AT24C02Data=AT24C02_Read(0);
						SZU=(int)(AT24C02Data)*10;
						ADC=1;
					}
					JM=2;
				}break;
				case 2:
				{
					UStart=1;
					PStart=0;
					NStart=0;
					L3Count=0;
					JM=0;
				}break;
			}
		}break;
		case 4:
		{
			MkeyNum=0;
			if(PStart==1)
			{
				TSZU+=50;
				L3Count=0;
			}
			else
			{
				L3Count++;
				if(L3Count>=3)
				{
					L3Count=3;
				}
			}
		}break;
		case 8:
		{
			MkeyNum=0;
			if(PStart==1)
			{
				TSZU-=50;
				L3Count=0;
			}
			else
			{
				L3Count++;
				if(L3Count>=3)
				{
					L3Count=3;
				}
			}
		}break;
		case 7:
		{
			MkeyNum=0;
			if(NStart==1)
			{
				Count=0;
				L3Count=0;
			}
			else
			{
				L3Count++;
				if(L3Count>=3)
				{
					L3Count=3;
				}
			}
		}break;
		case 1:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 2:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 5:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 6:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 9:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 10:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 11:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 12:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 13:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 14:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 15:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
		}break;
		case 16:
		{
			MkeyNum=0;
			L3Count++;
			if(L3Count>=3)
			{
				L3Count=3;
			}
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
	if(ADC==1)
	{
		ADC=0;
		AT24C02Data=AT24C02_Read(0);
		if(AT24C02Data<0||AT24C02Data>50||AT24C02Data%5!=0)
		AT24C02Data=25;
		SZU=(int)(AT24C02Data)*10;
		ADC=1;
	}
	while(1)
	{
		Mkey_Select();
		if(UStart==1)
		U_Loop();
		if(PStart==1)
		P_Loop();
		if(NStart==1)
		N_Loop();
	}
}

