#include <STC15F2K60S2.H>
#include "iic.h"


code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xff,0xc1,0x8c,0xc8};
unsigned char Temp[]={21,21,21,21,21,21,21,21};

unsigned char MkeyNum,ADCData,AT24C02Data,JS,LEDState=0xff,Count3;

unsigned char b=1,c=1,d=1,e=1;

unsigned int UData,Count2=0;

int Vp,TVp;

bit MkeyStart,ADCStart,UStart=1,PStart,NStart,Count2Start,a;

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

void JS_Loop()
{
	static unsigned int NowState,LastStata;
	LastStata=NowState;
	NowState=UData;
	if(NowState<Vp&&LastStata>Vp)
	JS++;
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
		MkeyStart=1;
	}
	if(++Count1>=100)
	{
		Count1=0;
		ADCStart=1;
	}
	if(Count2Start)
	{
		if(++Count2>=2500)
		{
			Count2Start=0;
			Count2=0;
			a=1;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfe;
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
	Temp[7]=UData%10;
}

void P_Loop()
{
	Temp[0]=23;
	Temp[5]=TVp/100+10;
	Temp[6]=TVp%100/10;
	Temp[7]=TVp%10;
}

void N_Loop()
{
	if(JS>=100)
	JS=1;
	Temp[0]=24;
	Temp[5]=21;
	Temp[6]=JS/10;
	Temp[7]=JS%10;
}

void Mkey_Select()   //S12=3 S16=4 S13=7 S17=8
{
	switch(MkeyNum)
	{
		case 3:
		{
			MkeyNum=0;
			if(UStart)
			{
				UStart=0;
				PStart=1;
				TVp=Vp;
			}else if(PStart)
			{
				PStart=0;
				NStart=1;
				if(TVp!=Vp)
				{
					Count2Start=0;
					Count2=0;
					if(a)
					{
						a=0;
						P2=((P2&0x1f)|0x80);
						LEDState|=0x01;
						P0=LEDState;
						P2&=0x1f;
					}
				}
				Vp=TVp;
				AT24C02_Write(0,Vp/10);
				
			}else if(NStart)
			{
				NStart=0;
				UStart=1;
			}
			Count3=0;
			d=1;
			e=1;
		}break;
		case 7:
		{
			MkeyNum=0;
			if(NStart)
			{
				JS=0;
				Count3=0;
				d=1;
				e=1;
			}
			else
			{
				Count3++;
				if(Count3>=3)
				Count3=3;
			}
		}break;
		case 4:
		{
			MkeyNum=0;
			if(PStart)
			{
				TVp+=50;
				if(TVp>500)
				TVp=0;
				Count3=0;
				d=1;
				e=1;
			}
			else
			{
				Count3++;
				if(Count3>=3)
				Count3=3;
			}
		}break;
		case 8:
		{
			MkeyNum=0;
			if(PStart)
			{
				TVp-=50;
				if(TVp<0)
				TVp=500;
				Count3=0;
				d=1;
				e=1;
			}
			else
			{
				Count3++;
				if(Count3>=3)
				Count3=3;
			}
		}break;
		case 1:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 2:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 5:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 6:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 9:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 10:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 11:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 12:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 13:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 14:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 15:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		case 16:
		{
			MkeyNum=0;
			Count3++;
			if(Count3>=3)
			Count3=3;
		}break;
		
	}
}

void LED2_Loop()
{
	
	if(JS%2==1)
	{
		if(b)
		{
			b=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfd;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		c=1;
	}
	if(JS%2==0)
	{
		if(c)
		{
			c=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x02;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		b=1;
	}
}

void LED3_Loop()
{
	if(Count3>=3)
	{
		if(d)
		{
			d=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfb;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		e=1;
	}
	else
	{
		if(e)
		{
			e=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x04;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		d=1;
	}
}

void main()
{
	Init();
	Timer0Init();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	ADCData=ADC_Read();
	UData=ADCData/255.0*500;
	AT24C02Data=AT24C02_Read(0);
	if(AT24C02Data>50||AT24C02Data<0||AT24C02Data%5!=0)
	AT24C02Data=25;
	Vp=AT24C02Data*10;
	while(1)
	{
		if(MkeyStart)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		Mkey_Select();
		LED3_Loop();
		if(ADCStart)
		{
			ADCStart=0;
			ADCData=ADC_Read();
			UData=ADCData/255.0*500;
			JS_Loop();
			if(UData<Vp)
			Count2Start=1;
			else
			{
				Count2Start=0;
				Count2=0;
				if(a)
				{
					a=0;
					P2=((P2&0x1f)|0x80);
					LEDState|=0x01;
					P0=LEDState;
					P2&=0x1f;
				}
			}
			LED2_Loop();
		}
		if(UStart)
		U_Loop();
		if(PStart)
		P_Loop();
		if(NStart)
		N_Loop();
	}
}
