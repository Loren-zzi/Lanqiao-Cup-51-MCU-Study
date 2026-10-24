#include <STC15F2K60S2.H>
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11,11};

code unsigned char MS[]={0,1,2,3,4};

unsigned int SJ[]={0,0,0,0,0};

unsigned int TSJ[]={0,0,0,0,0};


unsigned char KeyNum,AT24C02Data,ADCData,LEDState=0xff,Compare,MSSZ,SJSZ,Count4,Count5;

unsigned char CountL1,CountL2,CountL3,CountL4,I=1,SZCount=1,TTSJ,a;

unsigned int L1Count,L2Count,L3Count,L4Count;

bit LD,L1State,L2State,L3State,L4State,LEDStart,CLED,SZ,ADC=1;

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
	P2=((P2&0x1f)|0xA0);
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
	if(LastState==1&&NowState==0)
	KeyNum=1;
	if(LastState==0&&NowState==1)
	KeyNum=1;
	if(LastState==2&&NowState==0)
	KeyNum=2;
	if(LastState==3&&NowState==0)
	KeyNum=3;
	if(LastState==4&&NowState==0)
	KeyNum=4;
}

void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时初值
	TH0 = 0xD1;		//设置定时初值
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

void L1_Loop()
{
	CountL1++;
	P2=((P2&0x1f)|0x80);
	LEDState=~(0x01<<CountL1);
	P0=LEDState;
	P2&=0x1f;
	if(CountL1>=7)
	{
		L1State=0;
		L2State=1;
		CountL1=0;
	}
}

void L2_Loop()
{
	CountL2++;
	P2=((P2&0x1f)|0x80);
	LEDState=~(0x80>>CountL2);
	P0=LEDState;
	P2&=0x1f;
	if(CountL2>=7)
	{
		L2State=0;
		L3State=1;
		P2=((P2&0x1f)|0x80);
		LEDState=0x7e;
		P0=LEDState;
		P2&=0x1f;
		CountL2=0;
	}
}

void L3_Loop()
{
	CountL3++;
	if(CountL3==1)
	{
		P2=((P2&0x1f)|0x80);
		LEDState=0xbd;
		P0=LEDState;
		P2&=0x1f;
	}
	if(CountL3==2)
	{
		P2=((P2&0x1f)|0x80);
		LEDState=0xdb;
		P0=LEDState;
		P2&=0x1f;
	}
	if(CountL3==3)
	{
		P2=((P2&0x1f)|0x80);
		LEDState=0xe7;
		P0=LEDState;
		P2&=0x1f;
		L3State=0;
		L4State=1;
		CountL3=0;
	}
}

void L4_Loop()
{
	CountL4++;
	if(CountL4==1)
	{
		P2=((P2&0x1f)|0x80);
		LEDState=0xdb;
		P0=LEDState;
		P2&=0x1f;
	}
	if(CountL4==2)
	{
		P2=((P2&0x1f)|0x80);
		LEDState=0xbd;
		P0=LEDState;
		P2&=0x1f;
	}
	if(CountL4==3)
	{
		P2=((P2&0x1f)|0x80);
		LEDState=0x7e;
		P0=LEDState;
		P2&=0x1f;
	}
	if(CountL4==4)
	{
		L4State=0;
		L1State=1;
		P2=((P2&0x1f)|0x80);
		LEDState=0xfe;
		P0=LEDState;
		CountL4=0;
	}
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count0,Count1,Count3;
	static unsigned int Count2;
	if(++Count0>=2)
	{
		Count0=0;
		Nixie();
	}
	if(ADC==1)
	{
		if(++Count2>=300)
		{
			Count2=0;
			ADCData=ADC_Read();
			if(ADCData<64)
			Compare=2;
			if(ADCData>=64&&ADCData<128)
			Compare=5;
			if(ADCData>=128&&ADCData<192)
			Compare=7;
			if(ADCData>=192&&ADCData<=255)
			Compare=10;
		}
	}
	if(++Count1>=20)
	{
		Count1=0;
		Key_Loop();
	}
	if(LEDStart==1)
	{
		if(L1State==1)
		{
			if(++L1Count>=SJ[1])
			{
				L1Count=0;
				L1_Loop();
			}
		}
		if(L2State==1)
		{
			if(++L2Count>=SJ[2])
			{
				L2Count=0;
				L2_Loop();
			}
		}
		if(L3State==1)
		{
			if(++L3Count>=SJ[3])
			{
				L3Count=0;
				L3_Loop();
			}
		}
		if(L4State==1)
		{
			if(++L4Count>=SJ[4])
			{
				L4Count=0;
				L4_Loop();
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
				case 2:
				{
					Temp[0]=10;
					Temp[1]=MS[I];
					Temp[2]=10;
					if(TSJ[I]>=1000)
					{
						Temp[4]=TSJ[I]/1000;
						Temp[5]=TSJ[I]%1000/100;
						Temp[6]=TSJ[I]%1000%100/10;
						Temp[7]=TSJ[I]%1000%100%10;
					}
					else
					{
						Temp[4]=11;
						Temp[5]=TSJ[I]/100;
						Temp[6]=TSJ[I]%100/10;
						Temp[7]=TSJ[I]%100%10;
					}
					if(++Count5>=8)
					{
						Count5=0;
						MSSZ=1;
					}
				}break;
				case 1:
				{
					Temp[0]=11;
					Temp[1]=11;
					Temp[2]=11;
					if(TSJ[I]>=1000)
					{
						Temp[4]=TSJ[I]/1000;
						Temp[5]=TSJ[I]%1000/100;
						Temp[6]=TSJ[I]%1000%100/10;
						Temp[7]=TSJ[I]%1000%100%10;
					}
					else
					{
						Temp[4]=11;
						Temp[5]=TSJ[I]/100;
						Temp[6]=TSJ[I]%100/10;
						Temp[7]=TSJ[I]%100%10;
					}
					if(++Count5>=8)
					{
						Count5=0;
						MSSZ=2;
					}
				}break;
			}
			switch(SJSZ)
			{
				case 2:
				{
					Temp[0]=10;
					Temp[1]=MS[I];
					Temp[2]=10;
					if(TSJ[I]>=1000)
					{
						Temp[4]=TSJ[I]/1000;
						Temp[5]=TSJ[I]%1000/100;
						Temp[6]=TSJ[I]%1000%100/10;
						Temp[7]=TSJ[I]%1000%100%10;
					}
					else
					{
						Temp[4]=11;
						Temp[5]=TSJ[I]/100;
						Temp[6]=TSJ[I]%100/10;
						Temp[7]=TSJ[I]%100%10;
					}
					if(++Count5>=8)
					{
						Count5=0;
						SJSZ=1;
					}
				}break;
				case 1:
				{
					Temp[0]=10;
					Temp[1]=MS[I];
					Temp[2]=10;
					Temp[4]=11;
					Temp[5]=11;
					Temp[6]=11;
					Temp[7]=11;
					if(++Count5>=8)
					{
						Count5=0;
						SJSZ=2;
					}
				}break;
			}
		}
	}
	
	Count3++;
	if(Count3<=Compare)
	{
		P2=((P2&0x1f)|0x80);
		P0=LEDState;
		P2&=0x1f;
	}
	if(Count3>Compare)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xff;
		P2&=0x1f;
	}
	Count3%=10;
}


void Nixie_Loop()
{
	Temp[0]=11;
	Temp[1]=11;
	Temp[2]=11;
	Temp[3]=11;
	Temp[4]=11;
	Temp[5]=11;
	Temp[6]=11;
	Temp[7]=11;
}

void LD_Loop()
{
	if(KeyNum==1)
	{
		KeyNum=0;
		LD=0;
	}
	if(Compare==2)
	{
		Temp[7]=1;
		Temp[6]=10;
	}
	if(Compare==5)
	{
		Temp[7]=2;
		Temp[6]=10;
	}
	if(Compare==7)
	{
		Temp[7]=3;
		Temp[6]=10;
	}
	if(Compare==10)
	{
		Temp[7]=4;
		Temp[6]=10;
	}
}

void SZ_Loop()
{
	static unsigned char i=1;
	unsigned char b;
	switch(KeyNum)
	{
		case 4:
		{
			KeyNum=0;
			if(CLED==0)
			{
				LEDStart=1;
				L1State=1;
				P2=((P2&0x1f)|0x80);
				LEDState=0xfe;
				P0=LEDState;
				CLED=1;
			}
			else
			{
				LEDStart=0;
				L2State=0;
				L3State=0;
				L4State=0;
				P2=((P2&0x1f)|0x80);
				LEDState=0xff;
				P0=LEDState;
				CountL1=0;
				CountL2=0;
				CountL3=0;
				CountL4=0;
				CLED=0;
			}
		}break;
		case 3:
		{
			KeyNum=0;
			switch(SZCount)
			{
				case 1:
				{
					MSSZ=0;
					SJSZ=1;
					Count4=0;
					Count5=0;
					Temp[4]=11;
					Temp[5]=11;
					Temp[6]=11;
					Temp[7]=11;
					TTSJ=TSJ[I]/100;
					SZCount=2;
				}break;
				case 2:
				{
					SZ=0;
					SJSZ=0;
					I=1;
					Count4=0;
					Count5=0;
					SZCount=1;
					for(b=1;b<=4;b++)
					{
						ADC=0;
						AT24C02_Write(b,TSJ[b]/100);
						Delay5ms();
						AT24C02Data=AT24C02_Read(b);
						SJ[b]=(unsigned int)(AT24C02Data)*100;
						ADC=1;
					}
				}break;
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(MSSZ==1||MSSZ==2)
			{
				I++;
				if(I>=5)
				I=4;
			}
			else
			{
				TTSJ++;
				if(TTSJ>=13)
				TTSJ=12;
				TSJ[I]=(unsigned int)(TTSJ)*100;			
			}
		}break;
		case 1:
		{
			KeyNum=0;
			switch(i)
			{
				case 1:
				{
					i=2;
				}break;
				case 2:
				{
					i=1;
					if(MSSZ==1||MSSZ==2)
					{
						I--;
						if(I<=0)
						I=1;
					}
					else
					{
						TTSJ--;
						if(TTSJ<=3)
						TTSJ=4;
						TSJ[I]=(unsigned int)(TTSJ)*100;
					}
				}break;
			}
		}break;
	}
}

void Key_Select()
{
	switch(KeyNum)
	{
		case 1:
		{
			KeyNum=0;
			LD=1;
		}break;
		case 3:
		{
			KeyNum=0;
			SZ=1;
			MSSZ=1;
			for(a=1;a<=4;a++)
			TSJ[a]=SJ[a];
			if(SJ[1]>=1000)
			{
				Temp[4]=TSJ[I]/1000;
				Temp[5]=TSJ[I]%1000/100;
				Temp[6]=TSJ[I]%1000%100/10;
				Temp[7]=TSJ[I]%1000%100%10;
			}
			else
			{
				Temp[4]=11;
				Temp[5]=TSJ[I]/100;
				Temp[6]=TSJ[I]%100/10;
				Temp[7]=TSJ[I]%100%10;
			}
		}break;
		case 4:
		{
			KeyNum=0;
			if(CLED==0)
			{
				LEDStart=1;
				L1State=1;
				P2=((P2&0x1f)|0x80);
				LEDState=0xfe;
				P0=LEDState;
				CLED=1;
			}
			else
			{
				LEDStart=0;
				L2State=0;
				L3State=0;
				L4State=0;
				P2=((P2&0x1f)|0x80);
				LEDState=0xff;
				P0=LEDState;
				CountL1=0;
				CountL2=0;
				CountL3=0;
				CountL4=0;
				CLED=0;
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
		Key_Select();
		while(LD==1)
		LD_Loop();
		while(SZ==1)
		SZ_Loop();
		Nixie_Loop();
	}
}
