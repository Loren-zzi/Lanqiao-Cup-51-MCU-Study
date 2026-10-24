#include <STC15F2K60S2.H>
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xc1,0x8e};
unsigned char Temp[]={20,20,20,20,20,20,20,20};

unsigned char KeyNum,ADCData,DACData,LState=0xff;
	
unsigned int UData;

unsigned long Count,FData;

bit UStart=1,FStart,DACState,LStart=1,NixieStart=1,TUStart,TFStart;


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
	if(LastState==2&&NowState==0)
	KeyNum=2;
	if(LastState==3&&NowState==0)
	KeyNum=3;
	if(LastState==4&&NowState==0)
	KeyNum=4;
}

void Timer1Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x40;		//设置定时初值
	TH1 = 0xA2;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1=1;
	EA=1;
}

void Timer0Init(void)		//1个@12.000MHz
{
	TMOD |= 0x05;		//设置计数器模式
	TL0 = 0x00;		//设置计数初值
	TH0 = 0x00;		//设置计数重载值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
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


void DAC_Write(unsigned char Data)
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
	static unsigned char Count1;
	static unsigned char Count0,Count2;
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
	if(++Count2>=50)	
	{
		Count2=0;
		ADCData=ADC_Read();
		UData=ADCData/255.0*500;
	}
	if(DACState==0)
	DAC_Write(102);
	if(DACState==1)
	DAC_Write(ADCData);
}

void U_Loop()
{
	Temp[0]=21;
	Temp[2]=20;
	Temp[3]=20;
	Temp[4]=20;
	Temp[5]=UData/100+10;
	Temp[6]=UData%100/10;
	Temp[7]=UData%100%10;
}

void F_Loop()
{
	Temp[0]=22;
		if(FData<10)
		{
			Temp[2]=20;
			Temp[3]=20;
			Temp[4]=20;
			Temp[5]=20;
			Temp[6]=20;
			Temp[7]=FData%10;
		}
		if(FData>=10&&FData<100)
		{
			Temp[2]=20;
			Temp[3]=20;
			Temp[4]=20;
			Temp[5]=20;
			Temp[6]=FData/10;
			Temp[7]=FData%10;
		}
		if(FData>=100&&FData<1000)
		{
			Temp[2]=20;
			Temp[3]=20;
			Temp[4]=20;
			Temp[5]=FData/100;
			Temp[6]=FData%100/10;
			Temp[7]=FData%100%10;
		}
		if(FData>=1000&&FData<10000)
		{
			Temp[2]=20;
			Temp[3]=20;
			Temp[4]=FData/1000;
			Temp[5]=FData%1000/100;
			Temp[6]=FData%1000%100/10;
			Temp[7]=FData%1000%100%10;
		}
		if(FData>=10000&&FData<100000)
		{
			Temp[2]=20;
			Temp[3]=FData/10000;
			Temp[4]=FData%10000/1000;
			Temp[5]=FData%10000%1000/100;
			Temp[6]=FData%10000%1000%100/10;
			Temp[7]=FData%10000%1000%100%10;
		}
		if(FData>=100000)
		{
			Temp[2]=FData/100000;
			Temp[3]=FData%100000/10000;
			Temp[4]=FData%100000%10000/1000;
			Temp[5]=FData%100000%10000%1000/100;
			Temp[6]=FData%100000%10000%1000%100/10;
			Temp[7]=FData%100000%10000%1000%100%10;
		}
}

void LED_Loop()
{
	if(UStart==1)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xfe;
		LState|=0x02;
		P0=LState;
		P2&=0x1f;
	}
	if(FStart==1)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xfd;
		LState|=0x01;
		P0=LState;
		P2&=0x1f;
	}
	if(TUStart==1)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xfe;
		LState|=0x02;
		P0=LState;
		P2&=0x1f;
	}
	if(TFStart==1)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xfd;
		LState|=0x01;
		P0=LState;
		P2&=0x1f;
	}
	if(UData<150)
	{
		P2=((P2&0x1f)|0x80);
		LState|=0x04;
		P0=LState;
		P2&=0x1f;
	}
	if(UData>=150&&UData<250)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xfb;
		P0=LState;
		P2&=0x1f;
	}
	if(UData>=250&&UData<350)
	{
		P2=((P2&0x1f)|0x80);
		LState|=0x04;
		P0=LState;
		P2&=0x1f;
	}
	if(UData>=350)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xfb;
		P0=LState;
		P2&=0x1f;
	}
	if(FData<1000)
	{
		P2=((P2&0x1f)|0x80);
		LState|=0x08;
		P0=LState;
		P2&=0x1f;
	}
	if(FData>=1000&&FData<5000)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xf7;
		P0=LState;
		P2&=0x1f;
	}
	if(FData>=5000&&FData<10000)
	{
		P2=((P2&0x1f)|0x80);
		LState|=0x08;
		P0=LState;
		P2&=0x1f;
	}
	if(FData>=10000)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xf7;
		P0=LState;
		P2&=0x1f;
	}
	if(DACState==0)
	{
		P2=((P2&0x1f)|0x80);
		LState|=0x10;
		P0=LState;
		P2&=0x1f;
	}
	if(DACState==1)
	{
		P2=((P2&0x1f)|0x80);
		LState&=0xef;
		P0=LState;
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
			if(NixieStart==1)
			{
				if(UStart==1)
				{
					FStart=1;
					UStart=0;
				}
				else
				{
					FStart=0;
					UStart=1;
				}
			}
			else
			{
				if(TUStart==1)
				{
					TFStart=1;
					TUStart=0;
				}
				else
				{
					TFStart=0;
					TUStart=1;
				}
			}
		}break;
		case 2:
		{	
			KeyNum=0;
			if(DACState==1)
			{
				DACState=0;
			}
			else
			{
				DACState=1;
			}
		}break;
		case 3:
		{
			KeyNum=0;
			if(LStart==1)
			{
				LStart=0;
				P2=((P2&0x1f)|0x80);
				P0=0xff;
				LState=P0;
				P2&=0x1f;
			}
			else
			{
				LStart=1;
			}
		}break;
		case 4:
		{
			KeyNum=0;
			if(NixieStart==1)
			{
				NixieStart=0;
				TUStart=UStart;
				TFStart=FStart;
				UStart=0;
				FStart=0;
			}
			else
			{
				NixieStart=1;
				UStart=TUStart;
				FStart=TFStart;
				TUStart=0;
				TFStart=0;
			}
		}break;
	}
}

void Nixie_Loop()
{
	Temp[0]=20;
	Temp[1]=20;
	Temp[2]=20;
	Temp[3]=20;
	Temp[4]=20;
	Temp[5]=20;
	Temp[6]=20;
	Temp[7]=20;
}

void main()
{
	Init();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	UData=ADCData/255.0*500;
	Timer1Init();
	Timer0Init();
	while(1)
	{
		Key_Select();
		if(UStart==1)
		U_Loop();
		if(FStart==1)
		F_Loop();
		if(LStart==1)
		LED_Loop();
		if(NixieStart==0)
		Nixie_Loop();
	}
}