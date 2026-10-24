#include <STC15F2K60S2.H>
#include "iic.h"
#include "intrins.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,0xc1,0x8e};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char KeyNum;

unsigned int FData,UData;

unsigned char ADCData,LEDState=0xff;

unsigned char a=1,b=1,c=1,d=1,e=1,f=1,g=1,h=1;

bit KeyStart,ADCStart,FStart,UStart=1,DAC1Start=1,DAC2Start,LEDStart=1,SMGStart=1;

void Delay500ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 23;
	j = 205;
	k = 120;
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
	if(NowState==2&&LastState==0)
	KeyNum=2;
	if(NowState==3&&LastState==0)
	KeyNum=3;
	if(NowState==4&&LastState==0)
	KeyNum=4;
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

void DAC_Write(unsigned char Data)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40);
	I2CWaitAck();
	I2CSendByte(Data);
	I2CWaitAck();
	I2CStop();
}

void Timer0Init(void)		//65536??@12.000MHz
{
	AUXR &= 0x7F;		//?????12T??
	TMOD &= 0xF0;		//???????
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

void Timer1_Loop() interrupt 3
{
	static unsigned char Count0,Count1,Count2;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		KeyStart=1;
	}
	if(++Count1>=250)
	{
		Count1=0;
		FData=TH0<<8|TL0;
		FData*=2;
		TH0=0;
		TL0=0;
	}
	if(++Count2>=50)
	{
		Count2=0;
		ADCStart=1;
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
	Temp[7]=UData%10;
}

void F_Loop()
{
	Temp[0]=23;
	if(FData>=10000)
	{
		Temp[2]=21;
		Temp[3]=FData/10000;
		Temp[4]=FData%10000/1000;
		Temp[5]=FData%10000%1000/100;
		Temp[6]=FData%10000%1000%100/10;
		Temp[7]=FData%10;
	}else if(FData>=1000)
	{
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=FData/1000;
		Temp[5]=FData%1000/100;
		Temp[6]=FData%1000%100/10;
		Temp[7]=FData%10;
	}else if(FData>=100)
	{
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=21;
		Temp[5]=FData/100;
		Temp[6]=FData%100/10;
		Temp[7]=FData%10;
	}else if(FData>=10)
	{
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=21;
		Temp[5]=21;
		Temp[6]=FData/10;
		Temp[7]=FData%10;
	}
	else
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

	if(UStart)
	{
		if(a)
		{
			P2=((P2&0x1f)|0x80);
			LEDState|=0x02;
			LEDState&=0xfe;
			P0=LEDState;
			P2&=0x1f;
			a=0;
		}
		else
		b=1;
	}
	if(FStart)
	{
		if(b)
		{
			P2=((P2&0x1f)|0x80);
			LEDState|=0x01;
			LEDState&=0xfd;
			P0=LEDState;
			P2&=0x1f;
			b=0;
		}
		else
		a=1;
	}
	if(UData<150||(UData>=250&&UData<350))
	{
		if(c)
		{
			P2=((P2&0x1f)|0x80);
			LEDState|=0x04;
			P0=LEDState;
			P2&=0x1f;
			c=0;
		}
		else
		d=1;
	}
	
	if((UData>=150&&UData<250)||(UData>=350))
	{
		if(d)
		{
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfb;
			P0=LEDState;
			P2&=0x1f;
			d=0;
		}
		else
		c=1;
	}
	if(FData<1000||(FData>=5000&&FData<10000))
	{
		if(e)
		{
			P2=((P2&0x1f)|0x80);
			LEDState|=0x08;
			P0=LEDState;
			P2&=0x1f;
			e=0;
		}
		else
		f=1;
	}
	if((FData>=1000&&FData<5000)||FData>=10000)
	{
		if(f)
		{
			P2=((P2&0x1f)|0x80);
			LEDState&=0xf7;
			P0=LEDState;
			P2&=0x1f;
			f=0;
		}
		else
		e=1;
	}
	if(DAC1Start)
	{
		if(g)
		{
			P2=((P2&0x1f)|0x80);
			LEDState|=0x10;
			P0=LEDState;
			P2&=0x1f;
			g=0;
		}
		else
		h=1;
	}
	if(DAC2Start)
	{
		if(h)
		{
			P2=((P2&0x1f)|0x80);
			LEDState&=0xef;
			P0=LEDState;
			P2&=0x1f;
			h=0;
		}
		else
		g=1;
	}
}

void Key_Select()
{
	switch(KeyNum)
	{
		case 1:
		{
			KeyNum=0;
			if(UStart)
			{
				UStart=0;
				FStart=1;
			}
			else
			{
				UStart=1;
				FStart=0;
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(DAC1Start)
			{
				DAC1Start=0;
				DAC2Start=1;
			}
			else
			{
				DAC1Start=1;
				DAC2Start=0;
			}
		}break;
		case 3:
		{
			KeyNum=0;
			if(LEDStart)
			{
				LEDStart=0;
				P2=((P2&0x1f)|0x80);
				LEDState=0xff;
				P0=LEDState;
				P2&=0x1f;
				a=1;
				b=1;
				c=1;
				d=1;
				e=1;
				f=1;
				g=1;
				h=1;
			}
			else
			LEDStart=1;
		}break;
		case 4:
		{
			KeyNum=0;
			if(SMGStart)
			SMGStart=0;
			else
			SMGStart=1;
		}break;
	}
}

void main()
{
	Init();
	Timer1Init();
	Timer0Init();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	UData=ADCData/255.0*500;
	Delay500ms();
	while(1)
	{
		if(KeyStart)
		{
			KeyStart=0;
			Key_Loop();
		}
		Key_Select();
		if(ADCStart)
		{
			ADCStart=0;
			ADCData=ADC_Read();
			UData=ADCData/255.0*500;
			if(DAC1Start)
			DAC_Write(102);
			if(DAC2Start)
			DAC_Write(ADCData);
		}
		if(LEDStart)
		LED_Loop();
		if(SMGStart)
		{
			if(UStart)
			U_Loop();
			if(FStart)
			F_Loop();
		}
		else
		{
			Temp[0]=21;
			Temp[1]=21;
			Temp[2]=21;
			Temp[3]=21;
			Temp[4]=21;
			Temp[5]=21;
			Temp[6]=21;
			Temp[7]=21;
		}
	}
}

