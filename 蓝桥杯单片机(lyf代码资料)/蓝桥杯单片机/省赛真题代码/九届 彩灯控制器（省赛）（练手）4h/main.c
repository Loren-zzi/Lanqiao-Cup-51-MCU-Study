#include "STC15F2K60S2.h"
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};
unsigned int SJ[]={0,0,0,0,0};
unsigned char TCC[]={0,0,0,0,0};
unsigned int TSJ[]={0,0,0,0,0};
code unsigned char MS[]={0,1,2,3,4};


sbit K1=P3^3;
sbit K2=P3^2;
sbit K3=P3^1;
sbit K4=P3^0;

unsigned char KeyNum,AT24C02Data,ADCData,Compare,LedState=0xff,m;

unsigned char CountL1,CountL2,CountL3,CountL4,LD,Count4,Count9,I=1,TempCC,MSSZ,SJSZ,CCTemp;

unsigned int Count5,Count6,Count7,Count8;

bit L1Start=1,L2Start,L3Start,L4Start,LedStart,LCount,LDStart,SZ,ADCStart;

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

unsigned char Key_State()
{
	unsigned char Key=0;
	if(K1==0)
	Key=1;
	if(K2==0)
	Key=2;
	if(K3==0)
	Key=3;
	if(K4==0)
	Key=4;
	return Key;
}

void Key_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Key_State();
	if(LastState==2&&NowState==0)
	KeyNum=2;
	if(LastState==3&&NowState==0)
	KeyNum=3;
	if(LastState==1&&NowState==0)
	KeyNum=1;
	if(LastState==0&&NowState==1)
	KeyNum=1;
	if(LastState==4&&NowState==0)
	KeyNum=4;
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

void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
}

void Led_1()
{
	CountL1++;
	P2=((P2&0x1f)|0x80);
	P0=~(0x01<<CountL1);
	LedState=P0;
	P2&=0x1f;
	if(CountL1==7)
	{
		L1Start=0;
		L2Start=1;
		CountL1=0;
	}
}

void Led_2()
{
	CountL2++;
	P2=((P2&0x1f)|0x80);
	P0=~(0x80>>CountL2);
	LedState=P0;
	P2&=0x1f;
	if(CountL2==7)
	{
		L2Start=0;
		L3Start=1;
		CountL2=0;
		P2=((P2&0x1f)|0x80);
		P0=0x7e;
		LedState=P0;
		P2&=0x1f;
	}
}

void Led_3()
{
	CountL3++;
	if(CountL3==1)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xbd;
		LedState=P0;
		P2&=0x1f;
	}
	if(CountL3==2)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xdb;
		LedState=P0;
		P2&=0x1f;
	}
	if(CountL3==3)
	{
		CountL3=0;
		L3Start=0;
		L4Start=1;
		P2=((P2&0x1f)|0x80);
		P0=0xe7;
		LedState=P0;
		P2&=0x1f;
	}
}

void Led_4()
{
	CountL4++;
	if(CountL4==1)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xdb;
		LedState=P0;
		P2&=0x1f;
	}
	if(CountL4==2)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xbd;
		LedState=P0;
		P2&=0x1f;
	}
	if(CountL4==3)
	{
		P2=((P2&0x1f)|0x80);
		P0=0x7e;
		LedState=P0;
		P2&=0x1f;
	}
	if(CountL4==4)
	{
		CountL4=0;
		L4Start=0;
		L1Start=1;
		P2=((P2&0x1f)|0x80);
		P0=0xfe;
		LedState=P0;
		P2&=0x1f;
	}
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count0,Count1,Count2;
	static unsigned int Count3;
	
	if(++Count0>=2)
	{
		Count0=0;
		Nixie();
	}
	if(++Count1>=20)
	{
		Count1=0;
		Key_Loop();
	}
	if(ADCStart==0)
	{
		if(++Count3>=400)
		{
			Count3=0;
			ADCData=ADC_Read();
			if(ADCData<=63)
			{
				Compare=2;
				LD=1;
			}
			if(ADCData>63&&ADCData<=127)
			{
				Compare=5;
				LD=2;
			}
			if(ADCData>127&&ADCData<=191)
			{
				Compare=7;
				LD=3;
			}
			if(ADCData>191&&ADCData<=255)
			{
				Compare=10;
				LD=4;
			}
		}
	}
	if(SZ==1)
	{
		if(++Count4>=100)
		{
			Count4=0;
			switch(MSSZ)
			{
				case 1:
				{
					Temp[0]=11;
					Temp[1]=11;
					Temp[2]=11;
					if(TSJ[I]<1000)
					{
						Temp[4]=11;
						Temp[5]=TSJ[I]/100;
						Temp[6]=TSJ[I]%100/10;
						Temp[7]=TSJ[I]%100%10;
					}
					if(TSJ[I]>=1000)
					{
						Temp[4]=TSJ[I]/1000;
						Temp[5]=TSJ[I]%1000%100;
						Temp[6]=TSJ[I]%1000%100/10;
						Temp[7]=TSJ[I]%1000%100%10;
					}
					if(++Count9>=8)
					{
						Count9=0;
						MSSZ=2;
					}
				}break;
				case 2:
				{
					Temp[0]=10;
					Temp[1]=MS[I];
					Temp[2]=10;
					if(TSJ[I]<1000)
					{
						Temp[4]=11;
						Temp[5]=TSJ[I]/100;
						Temp[6]=TSJ[I]%100/10;
						Temp[7]=TSJ[I]%100%10;
					}
					if(TSJ[I]>=1000)
					{
						Temp[4]=TSJ[I]/1000;
						Temp[5]=TSJ[I]%1000%100;
						Temp[6]=TSJ[I]%1000%100/10;
						Temp[7]=TSJ[I]%1000%100%10;
					}
					if(++Count9>=8)
					{
						Count9=0;
						MSSZ=1;
					}
				}break;
			}
			switch(SJSZ)
			{
				case 1:
				{
					Temp[0]=10;
					Temp[1]=MS[I];
					Temp[2]=10;
					Temp[4]=11;
					Temp[5]=11;
					Temp[6]=11;
					Temp[7]=11;
					if(++Count9>=8)
					{
						Count9=0;
						SJSZ=2;
					}
				}break;
				case 2:
				{
					Temp[0]=10;
					Temp[1]=MS[I];
					Temp[2]=10;
					if(TSJ[I]<1000)
					{
						Temp[4]=11;
						Temp[5]=TSJ[I]/100;
						Temp[6]=TSJ[I]%100/10;
						Temp[7]=TSJ[I]%100%10;
					}
					if(TSJ[I]>=1000)
					{
						Temp[4]=TSJ[I]/1000;
						Temp[5]=TSJ[I]%1000%100;
						Temp[6]=TSJ[I]%1000%100/10;
						Temp[7]=TSJ[I]%1000%100%10;
					}
					if(++Count9>=8)
					{
						Count9=0;
						SJSZ=1;
					}
				}break;
			}
		}
	}
	if(LedStart==1)
	{
		if(L1Start==1)
		{
			if(++Count5>=SJ[1])
			{
				Count5=0;
				Led_1();
			}
		}
		if(L2Start==1)
		{
			if(++Count6>=SJ[2])
			{
				Count6=0;
				Led_2();
			}
		}
		if(L3Start==1)
		{
			if(++Count7>=SJ[3])
			{
				Count7=0;
				Led_3();
			}
		}
		if(L4Start==1)
		{
			if(++Count8>=SJ[4])
			{
				Count8=0;
				Led_4();
			}
		}
	}
	
	Count2++;
	if(Count2<=Compare)
	{
		P2=((P2&0x1f)|0x80);
		P0=LedState;
		P2&=0x1f;
	}
	if(Count2>Compare)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xff;
		P2&=0x1f;
	}
	Count2%=10;
}

void LD_Loop()
{
	if(KeyNum==1)
	{
		KeyNum=0;
		LDStart=0;
	}
	Temp[6]=10;
	Temp[7]=LD;
}

void Key_Selete()
{
	switch(KeyNum)
	{
		case 4:
		{
			KeyNum=0;
			if(LCount==0)
			{
				LCount=1;
				LedStart=1;
				L1Start=1;
				P2=((P2&0x1f)|0x80);
				P0=0xfe;
				LedState=P0;
				P2&=0x1f;
			}
			else
			{
				LCount=0;
				LedStart=0;
				CountL1=0;
				CountL2=0;
				CountL3=0;
				CountL4=0;
				L2Start=0;
				L3Start=0;
				L4Start=0;
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				LedState=P0;
				P2&=0x1f;
			}
		}break;
		case 1:
		{
			KeyNum=0;
			LDStart=1;
		}break;
		case 3:
		{
			KeyNum=0;
			SZ=1;
			MSSZ=1;
			SJSZ=0;
			for(m=1;m<=4;m++)
			{
				TSJ[m]=SJ[m];
			}
			TSJ[I]=SJ[I];
			Temp[0]=11;
			Temp[1]=11;
			Temp[2]=11;
			if(TSJ[I]<1000)
			{
				Temp[4]=11;
				Temp[5]=TSJ[I]/100;
				Temp[6]=TSJ[I]%100/10;
				Temp[7]=TSJ[I]%100%10;
			}
			if(TSJ[I]>=1000)
			{
				Temp[4]=TSJ[I]/1000;
				Temp[5]=TSJ[I]%1000%100;
				Temp[6]=TSJ[I]%1000%100/10;
				Temp[7]=TSJ[I]%1000%100%10;
			}
		}break;
	}
}

void SZ_Loop()
{
	static unsigned char i,a,b;
	switch(KeyNum)
	{
		case 3:
		{
			KeyNum=0;
			switch(i)
			{
				case 0:
				{
					MSSZ=0;
					SJSZ=1;
					Count4=0;
					Count9=0;
					i=1;
				}break;
				case 1:
				{
					i=0;
					SZ=0;
					I=1;
					Count4=0;
					Count9=0;
					if(ADCStart==0)
					{
						ADCStart=1;
						for(b=1;b<=4;b++)
						{
							AT24C02_Write(b,TSJ[b]/100);
							Delay5ms();
							AT24C02Data=AT24C02_Read(b);
							SJ[b]=(unsigned int)(AT24C02Data)*100;
						}
						ADCStart=0;
					}
				}break;
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(MSSZ==1||MSSZ==2)
			{
				if(I==4)
				I=3;
				I++;
				TSJ[I]=SJ[I];
			}
			if(SJSZ==1||SJSZ==2)
			{
				if(TSJ[I]==1200)
				TSJ[I]=1100;
				CCTemp=TSJ[I]/100;
				CCTemp++;
				TSJ[I]=(unsigned int)(CCTemp)*100;
			}
		}break;		
		case 1:
		{
			KeyNum=0;
			switch(a)
			{
				case 0:
				{
					a=1;
				}break;
				case 1:
				{
					a=0;
					if(MSSZ==1||MSSZ==2)
					{
						if(I==1)
						I=2;
						I--;
						TSJ[I]=SJ[I];
					}
					if(SJSZ==1||SJSZ==2)
					{
						if(TSJ[I]==400)
						TSJ[I]=500;
						CCTemp=TSJ[I]/100;
						CCTemp--;
						TSJ[I]=(unsigned int)(CCTemp)*100;
					}
				}break;
			}
		}break;		
	}
}

void main()
{
	Init();
	AT24C02Data=AT24C02_Read(1);
	if(AT24C02Data<4||AT24C02Data>12)
	AT24C02Data=4;
	SJ[1]=(unsigned int)(AT24C02Data)*100;
	AT24C02Data=AT24C02_Read(2);
	if(AT24C02Data<4||AT24C02Data>12)
	AT24C02Data=4;
	SJ[2]=(unsigned int)(AT24C02Data)*100;
	AT24C02Data=AT24C02_Read(3);
	if(AT24C02Data<4||AT24C02Data>12)
	AT24C02Data=4;
	SJ[3]=(unsigned int)(AT24C02Data)*100;
	AT24C02Data=AT24C02_Read(4);
	if(AT24C02Data<4||AT24C02Data>12)
	AT24C02Data=4;
	SJ[4]=(unsigned int)(AT24C02Data)*100;
	Timer0Init();
	while(1)
	{
		Key_Selete();
		while(LDStart==1)
		LD_Loop();
		while(SZ==1)
		SZ_Loop();
		Temp[0]=11;
		Temp[1]=11;
		Temp[2]=11;
		Temp[3]=11;
		Temp[4]=11;
		Temp[5]=11;
		Temp[6]=11;
		Temp[7]=11;
	}
}
