#include <STC15F2K60S2.H>
#include "onewire.h"
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0xc6};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

unsigned char KeyNum=0,Compare,Time=0,LEDState=0xff;

unsigned char a=0,b=0,c=0,d=0;
	
unsigned int CountTime=0;

float T;

bit KeyStart=0,DS18B20Start=0,DAC5Start=0,DAC0Start=0,MS1=1,MS2=0,MS3=0,WD=0,Time0=1,Time1=0,Time2=0;

bit TimeKG1=0;

void LED_Loop();

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
	static unsigned char Bit=0;
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

void DAC(unsigned char Data)
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

void Timer0Init(void)		//100??@12.000MHz
{
	AUXR |= 0x80;		//?????1T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x50;		//??????
	TH0 = 0xFB;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	EA=1;
	ET0=1;
}

void Timer0_Loop() interrupt 1
{
	static unsigned char Count0,Count1,Count3;
	static unsigned int Count2;
	if(++Count0>=20)
	{
		Count0=0;
		Nixie();
	}
	if(++Count1>=200)
	{
		Count1=0;
		KeyStart=1;
	}
	if(WD)
	{
		if(++Count2>=2000)
		{
			Count2=0;
			DS18B20Start=1;
		}
	}
	if(Time)
	{
		if(++Count3<=Compare)
		{
			DAC5Start=1;
		}
		else
		{
			DAC0Start=1;
		}
		Count3%=10;
	}
	else
	{
		if(TimeKG1==0)
		{
			TimeKG1=1;
			DAC0Start=1;
		}
		else
		{
			TimeKG1=1;
		}
	}
	if(Time)
	{
		if(++CountTime>=10000)
		{
			CountTime=0;
			Time--;
		}
	}
	
}

void WD_Loop()
{
	if(KeyStart)
	{
		KeyStart=0;
		Key_Loop();
	}
	switch(KeyNum)
	{
		case 1:
		{
			KeyNum=0;
			if(MS1)
			{
				MS1=0;
				MS2=1;
				Compare=3;
				break;
			}
			if(MS2)
			{
				MS2=0;
				MS3=1;
				Compare=7;
				break;
			}
			if(MS3)
			{
				MS3=0;
				MS1=1;
				Compare=2;
				break;
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(Time0)
			{
				Time0=0;
				Time1=1;
				Time=5;
				TimeKG1=0;
				CountTime=0;
				break;
			}
			if(Time1)
			{
				Time1=0;
				Time2=1;
				Time=10;
				TimeKG1=0;
				CountTime=0;
				break;
			}
			if(Time2)
			{
				d=0;
				Time2=0;
				Time0=1;
				Time=0;
				TimeKG1=0;
				CountTime=0;
				break;
			}
		}break;
		case 3:
		{
			KeyNum=0;
			d=0;
			Time=0;
			Time0=1;
			Time1=0;
			Time2=0;
			TimeKG1=0;
			CountTime=0;
		}break;
		case 4:
		{
			KeyNum=0;
			WD=0;
		}break;
	}
	LED_Loop();
	if(DAC5Start)
	{
		DAC5Start=0;
		DAC(255);
	}
	if(DAC0Start)
	{
		DAC0Start=0;
		DAC(0);
	}
	if(DS18B20Start)
	{
		DS18B20Start=0;
		DS18B20_Read();
	}
	Temp[0]=10;
	Temp[1]=4;
	Temp[2]=10;
	Temp[4]=11;
	Temp[5]=(int)(T)/10;
	Temp[6]=(int)(T)%10;
	Temp[7]=12;
}

void MS1_Loop()
{
	Compare=2;
	Temp[0]=10;
	Temp[1]=1;
	Temp[2]=10;
	Temp[4]=Time/1000;
	Temp[5]=Time%1000/100;
	Temp[6]=Time%1000%100/10;
	Temp[7]=Time%10;
}

void MS2_Loop()
{
	Compare=3;
	Temp[0]=10;
	Temp[1]=2;
	Temp[2]=10;
	Temp[4]=Time/1000;
	Temp[5]=Time%1000/100;
	Temp[6]=Time%1000%100/10;
	Temp[7]=Time%10;
}

void MS3_Loop()
{
	Compare=7;
	Temp[0]=10;
	Temp[1]=3;
	Temp[2]=10;
	Temp[4]=Time/1000;
	Temp[5]=Time%1000/100;
	Temp[6]=Time%1000%100/10;
	Temp[7]=Time%10;
}

void LED_Loop()
{
	if(Time)
	{
		if(MS1)
		{
			if(a==0)
			{
				a=1;
				b=0;
				c=0;
				d=0;
				P2=((P2&0x1f)|0x80);
				LEDState=0xff;
				LEDState&=0xfe;
				P0=LEDState;
				P2&=0x1f;
			}
		}
		if(MS2)
		{
			if(b==0)
			{
				a=0;
				b=1;
				c=0;
				d=0;
				P2=((P2&0x1f)|0x80);
				LEDState=0xff;
				LEDState&=0xfd;
				P0=LEDState;
				P2&=0x1f;
			}
		}
		if(MS3)
		{
			if(c==0)
			{
				a=0;
				b=0;
				c=1;
				d=0;
				P2=((P2&0x1f)|0x80);
				LEDState=0xff;
				LEDState&=0xfb;
				P0=LEDState;
				P2&=0x1f;
			}
		}
	}
	else
	{
		if(d==0)
		{
			d=1;
			a=0;
			b=0;
			c=0;
			P2=((P2&0x1f)|0x80);
			LEDState=0xff;
			P0=LEDState;
			P2&=0x1f;
		}
	}
}

void Key_Select()
{
	switch(KeyNum)
	{
		case 1:
		{
			KeyNum=0;
			if(MS1)
			{
				MS1=0;
				MS2=1;
				break;
			}
			if(MS2)
			{
				MS2=0;
				MS3=1;
				break;
			}
			if(MS3)
			{
				MS3=0;
				MS1=1;
				break;
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(Time0)
			{
				Time0=0;
				Time1=1;
				Time=5;
				TimeKG1=0;
				CountTime=0;
				break;
			}
			if(Time1)
			{
				Time1=0;
				Time2=1;
				Time=10;
				TimeKG1=0;
				CountTime=0;
				break;
			}
			if(Time2)
			{
				d=0;
				Time2=0;
				Time0=1;
				Time=0;
				TimeKG1=0;
				CountTime=0;
				break;
			}
		}break;
		case 3:
		{
			KeyNum=0;
			d=0;
			Time=0;
			Time0=1;
			Time1=0;
			Time2=0;
			TimeKG1=0;
			CountTime=0;
		}break;
		case 4:
		{
			KeyNum=0;
			WD=1;
			DS18B20_Read();
		}break;
	}
}

void main()
{
	Init();
	DS18B20_Read();
	Timer0Init();
	while(1)
	{
		if(DAC5Start)
		{
			DAC5Start=0;
			DAC(255);
		}
		if(DAC0Start)
		{
			DAC0Start=0;
			DAC(0);
		}
		if(KeyStart)
		{
			KeyStart=0;
			Key_Loop();
		}
		Key_Select();
		LED_Loop();
		if(MS1)
		MS1_Loop();
		if(MS2)
		MS2_Loop();
		if(MS3)
		MS3_Loop();
		while(WD)
		WD_Loop();
	}
}

