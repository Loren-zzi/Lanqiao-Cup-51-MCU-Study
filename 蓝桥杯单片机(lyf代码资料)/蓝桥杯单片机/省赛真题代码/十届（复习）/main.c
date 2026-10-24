#include <STC15F2K60S2.H>
#include "intrins.h"
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,0xc1,0x8e};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char KeyNum,ADCData,LEDState=0xff;

unsigned int FData,UData;

bit UState=1,FState,DAC1=1,DAC2,LED=1,NState,TUState,TFState;

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
	if(LastState==0&&NowState==1)
	KeyNum=1;
	if(LastState==0&&NowState==2)
	KeyNum=2;
	if(LastState==0&&NowState==3)
	KeyNum=3;
	if(LastState==0&&NowState==4)
	KeyNum=4;
}

void Timer0Init(void)		//0??@12.000MHz
{
	TMOD |= 0x05;		//???????
	TL0 = 0x00;		//??????
	TH0 = 0x00;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
}

void Timer1Init(void)		//2??@12.000MHz
{
	AUXR |= 0x40;		//?????1T??
	TMOD &= 0x0F;		//???????
	TL1 = 0x40;		//??????
	TH1 = 0xA2;		//??????
	TF1 = 0;		//??TF1??
	TR1 = 1;		//???1????
	EA=1;
	ET1=1;
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

void ADC_Write(unsigned char Data)
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x40);
	IIC_WaitAck();
	IIC_SendByte(Data);
	IIC_WaitAck();
	IIC_Stop();
}

void Timer1_Loop() interrupt 3
{
	static unsigned char Count0,Count1,Count2,Count3;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		Key_Loop();
	}
	if(++Count1>=250)
	{
		Count1=0;
		FData=(TH0<<8)|TL0;
		FData*=2;
		TH0=0;
		TL0=0;
	}
	if(++Count2>=150)
	{
		Count2=0;
		ADCData=ADC_Read();
		UData=ADCData/255.0*500;
	}
	if(++Count3>=50)
	{
		Count3=0;
		if(DAC1)
		ADC_Write(102);
		if(DAC2)
		ADC_Write(ADCData);
	}
}

void U_Loop()
{
	Temp[0]=22;
	Temp[2]=21;
	Temp[3]=21;
	Temp[4]=21;
	Temp[5]=UData/100+10;
	Temp[6]=UData%100/10;
	Temp[7]=UData%100%10;
}

void F_Loop()
{
	Temp[0]=23;
	if(FData>=100000)
	{
		Temp[2]=FData/100000;
		Temp[3]=FData%100000/10000;
		Temp[4]=FData%100000%10000/1000;
		Temp[5]=FData%100000%10000%1000/100;
		Temp[6]=FData%100000%10000%1000%100/10;
		Temp[7]=FData%100000%10000%1000%100%10;
	}
	if(FData<100000&&FData>=10000)
	{
		Temp[2]=21;
		Temp[3]=FData/10000;
		Temp[4]=FData%10000/1000;
		Temp[5]=FData%10000%1000/100;
		Temp[6]=FData%10000%1000%100/10;
		Temp[7]=FData%10000%1000%100%10;
	}
	if(FData<10000&&FData>=1000)
	{
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=FData/1000;
		Temp[5]=FData%1000/100;
		Temp[6]=FData%1000%100/10;
		Temp[7]=FData%1000%100%10;
	}
	if(FData<1000&&FData>=100)
	{
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=21;
		Temp[5]=FData/100;
		Temp[6]=FData%100/10;
		Temp[7]=FData%100%10;
	}
	if(FData<100&&FData>=10)
	{
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=21;
		Temp[5]=21;
		Temp[6]=FData/10;
		Temp[7]=FData%10;
	}
	if(FData<10)
	{
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=21;
		Temp[5]=21;
		Temp[6]=21;
		Temp[7]=FData%10;
	}
}

void LED_Loop()
{
	if(NState==0)
	{
		if(UState==1)
		{
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfe;
			LEDState|=0x02;
			P0=LEDState;
			P2&=0x1f;
		}
		if(FState==1)
		{
			P2=((P2&0x1f)|0x80);
			LEDState|=0x01;
			LEDState&=0xfd;
			P0=LEDState;
			P2&=0x1f;
		}
	}
	if(NState==1)
	{
		if(TUState==1)
		{
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfe;
			LEDState|=0x02;
			P0=LEDState;
			P2&=0x1f;
		}
		if(TFState==1)
		{
			P2=((P2&0x1f)|0x80);
			LEDState|=0x01;
			LEDState&=0xfd;
			P0=LEDState;
			P2&=0x1f;
		}
	}
	if(UData<150)
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x04;
		P0=LEDState;
		P2&=0x1f;
	}
	if(UData>=150&&UData<250)
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xfb;
		P0=LEDState;
		P2&=0x1f;
	}
	if(UData>=250&&UData<350)
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x04;
		P0=LEDState;
		P2&=0x1f;
	}
	if(UData>=350)
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xfb;
		P0=LEDState;
		P2&=0x1f;
	}
	if(FData<1000)
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x08;
		P0=LEDState;
		P2&=0x1f;
	}
	if(FData>=1000&&FData<5000)
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xf7;
		P0=LEDState;
		P2&=0x1f;
	}
	if(FData>=5000&&FData<10000)
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x08;
		P0=LEDState;
		P2&=0x1f;
	}
	if(FData>=10000)
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xf7;
		P0=LEDState;
		P2&=0x1f;
	}
	if(DAC1==1)
	{
		P2=((P2&0x1f)|0x80);
		LEDState|=0x10;
		P0=LEDState;
		P2&=0x1f;
	}
	if(DAC2==1)
	{
		P2=((P2&0x1f)|0x80);
		LEDState&=0xef;
		P0=LEDState;
		P2&=0x1f;
	}
}

void Key_Select()
{
	switch(KeyNum)
	{
		case 1:
		{
			KeyNum=0;
			if(NState==0)
			{
				if(UState==1)
				{
					UState=0;
					FState=1;
				}
				else
				{
					UState=1;
					FState=0;
				}
			}
			if(NState==1)
			{
				if(TUState==1)
				{
					TUState=0;
					TFState=1;
				}
				else
				{
					TUState=1;
					TFState=0;
				}
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(DAC1==1)
			{
				DAC1=0;
				DAC2=1;
			}
			else
			{
				DAC1=1;
				DAC2=0;
			}
		}break;
		case 3:
		{
			KeyNum=0;
			if(LED==1)
			{
				LED=0;
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				P2&=0x1f;
			}
			else
			{
				LED=1;
			}
		}break;
		case 4:
		{
			KeyNum=0;
			if(NState==0)
			{
				NState=1;
				TUState=UState;
				TFState=FState;
				UState=0;
				FState=0;
				Temp[0]=21;
				Temp[1]=21;
				Temp[2]=21;
				Temp[3]=21;
				Temp[4]=21;
				Temp[5]=21;
				Temp[6]=21;
				Temp[7]=21;
			}
			else
			{
				NState=0;
				UState=TUState;
				FState=TFState;
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
	Timer1Init();
	while(1)
	{
		Key_Select();
		if(UState)
		U_Loop();
		if(FState)
		F_Loop();
		if(LED)
		LED_Loop();
	}
}