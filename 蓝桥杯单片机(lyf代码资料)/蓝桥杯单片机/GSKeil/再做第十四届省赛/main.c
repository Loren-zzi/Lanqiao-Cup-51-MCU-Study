#include <STC15F2K60S2.H>
#include "ds1302.h"
#include "iic.h"
#include "onewire.h"
#include "intrins.h"

#define WP 0x8e

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,0xc6,0x89,0x8e,0x8c,0x86,0x88};
unsigned char Temp[]={21,21,21,21,21,21,21,21};
code unsigned char TimeWCommand[]={0x84,0x82,0x80};
code unsigned char TimeRCommand[]={0x85,0x83,0x81};
unsigned char Time[]={0x13,0x03,0x05};

unsigned char MkeyNum,GMData,LEDState=0xff,CFCS,WDMax=0,SDMax=0,Hour,Min,Count8;

char CST=30,TCST;

unsigned int PLData,i,Count7;

float T,SDData,PJSD,PJWD,ST,SSDData;

bit MkeyStart,SJJMStart=1,TimeStart,WXDataStart,WSDStart,DS18B20Start,PCF8591Start,CJStart;
bit CSJMStart,WDHXStart=1,HXJMStart,SDHXStart,SJHXStart;
bit Sec3Start,HSec10Start,Sec2Start,L3flag,L3Start=1,L3Over,WX,L6Start;
bit a=1,b=1,c=1,d=1,e=1,f=1,g=1,h=1,j=1,k=1,l=1,m=1;
void Delay800ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 37;
	j = 123;
	k = 92;
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

unsigned char Mkey_State()
{
	unsigned char Mkey=0;
	P44=0;
	P42=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=1;
	if(P32==0)
	Mkey=5;
	
	P44=1;
	P42=0;
	P3|=0x0f;
	if(P33==0)
	Mkey=2;
	if(P32==0)
	Mkey=6;
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
	if(NowState==5&&LastState==0)
	MkeyNum=5;
	if(NowState==6&&LastState==0)
	{
		if(SJHXStart)
		Sec2Start=1;
		MkeyNum=6;
	}
	if(SJHXStart)
	{
		if(NowState==0&&LastState==6)
		{
			if(Sec2Start)
			{
				Sec2Start=0;
				Count7=0;
			}
		}
	}
}

void Time_Write()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<3;i++)
	Write_Ds1302_Byte(TimeWCommand[i],Time[i]);
	Write_Ds1302_Byte(WP,0xff);
}

void Time_Read()
{
	unsigned char i;
	for(i=0;i<3;i++)
	Time[i]=Read_Ds1302_Byte(TimeRCommand[i]);
}

void DS18B20_Read()
{
	int TEMP;
	unsigned char TLSB,TMSB;
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

unsigned char PCF8591_Read()
{
	unsigned char Data;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x01);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0x90|0x01);
	I2CWaitAck();
	Data=I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	return Data;
}

void CJXY_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=GMData;
	if(NowState<20&&LastState>20)
	{
		if(Sec3Start==0)
		CFCS++;
		WDMax=0;
		SDMax=0;
		Sec2Start=0;
		Count7=0;
		CJStart=1;
		Sec3Start=1;
		Hour=Time[0]/16*10+Time[0]%16;
		Min=Time[1]/16*10+Time[1]%16;
		if(CFCS>99)
		CFCS=1;
	}
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


void Timer1_Loop() interrupt 3
{
	static unsigned char Count0,Count1,Count2,Count3,Count4,Count6=0;
	static unsigned int Count5;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		MkeyStart=1;
	}
	if(++Count3>=25)
	{
		Count3=0;
		DS18B20Start=1;
	}
	if(++Count4>=70)
	{
		Count4=0;
		PCF8591Start=1;
	}
	if(++Count1>=250)
	{
		Count1=0;
		PLData=TH0<<8|TL0;
		PLData*=2;
		if(PLData<200)
		WXDataStart=1;
		else if(PLData>2000)
		WXDataStart=1;
		else 
		{
			WXDataStart=0;
			SDData=PLData*0.0444+1.2;
		}
		TH0=0;
		TL0=0;
	}
	if(++Count2>=25)
	{
		Count2=0;
		TimeStart=1;
	}
	if(Sec3Start)  //取样
	{
		if(++Count6>=10)
		{
			Count6=0;
			HSec10Start=1;
		}
		if(++Count5>=1500)
		{
			Count5=0;
			Count6=0;
			Sec3Start=0;
			CJStart=0;
			PJWD=ST/i;
			PJSD=SSDData/i;
			i=0;
			ST=0;
			SSDData=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x08;
			P0=LEDState;
			P2&=0x1f;
			L3flag=0;
			L3Start=1;
			L3Over=0;
			g=1;
			h=1;
		}
	}
	if(Sec2Start)  //清除
	{
		if(++Count7>=1000)
		{
			Count7=0;
			CFCS=0;
		}
	}
	if(L3flag)
	{
		if(++Count8>=50)
		{
			Count8=0;
			if(L3Start)
			{
				L3Start=0;
				L3Over=1;
			}
			else
			{
				L3Start=1;
				L3Over=0;
			}
		}
	}
}

void SJJM_Loop()  //时间界面
{
	Temp[0]=Time[0]/16;
	Temp[1]=Time[0]%16;
	Temp[2]=20;
	Temp[3]=Time[1]/16;
	Temp[4]=Time[1]%16;
	Temp[5]=20;
	Temp[6]=Time[2]/16;
	Temp[7]=Time[2]%16;	
}

void WSD_Loop()   //温湿度界面*******************************
{
	static unsigned char WDNowState,WDLastState,SDNowState,SDLastState;
	if((int)(T)>CST)
	{
		L3flag=1;
	}
	else
	{
		if(L3flag)
		{
			P2=((P2&0x1f)|0x80);
			LEDState|=0x08;
			P0=LEDState;
			P2&=0x1f;
		}
		L3flag=0;
		L3Start=1;
		L3Over=0;
	}
	if(T>WDMax)
	WDMax=T;
	if(SDData>SDMax)
	SDMax=SDData;
	if(HSec10Start)
	{
		i++;
		HSec10Start=0;
		WDLastState=WDNowState;
		WDNowState=(int)(T);
		SDLastState=SDNowState;
		SDNowState=(int)(SDData);	
		if((int)(T))
		ST+=(int)(T);
		SSDData+=(int)(SDData);
	}
	if(WXDataStart)
	{
		WX=1;
		Temp[0]=26;
		Temp[1]=21;
		Temp[2]=21;
		Temp[3]=(int)(T)/10;
		Temp[4]=(int)(T)%10;
		Temp[5]=20;
		Temp[6]=27;
		Temp[7]=27;	
	}
	else
	{
		WX=0;
		Temp[0]=26;
		Temp[1]=21;
		Temp[2]=21;
		Temp[3]=(int)(T)/10;
		Temp[4]=(int)(T)%10;
		Temp[5]=20;
		Temp[6]=(int)(SDData)/10;
		Temp[7]=(int)(SDData)%10;	
	}
	if((WDNowState>WDLastState)&&(SDNowState>SDLastState))
	L6Start=1;
	else
	L6Start=0;
}

void CS_Loop()  //参数界面
{
		Temp[0]=25;
		Temp[1]=21;
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=21;
		Temp[5]=21;
		Temp[6]=TCST/10;
		Temp[7]=TCST%10;	
}

void WDHX_Loop()  //温度回显
{
	if(CFCS==0)   //触发次数
	{
		Temp[0]=22;
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
		Temp[0]=22;
		Temp[1]=21;
		Temp[2]=WDMax/10;
		Temp[3]=WDMax%10;
		Temp[4]=20;
		Temp[5]=(int)(PJWD*10)/100;
		Temp[6]=(int)(PJWD*10)%100/10+10;
		Temp[7]=(int)(PJWD*10)%10;	
	}
}

void SDHX_Loop() //湿度回显
{
	if(CFCS==0)
	{
		Temp[0]=23;
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
		Temp[0]=23;
		Temp[1]=21;
		Temp[2]=SDMax/10;
		Temp[3]=SDMax%10;
		Temp[4]=20;
		Temp[5]=(int)(PJSD*10)/100;
		Temp[6]=(int)(PJSD*10)%100/10+10;
		Temp[7]=(int)(PJSD*10)%10;	
	}
}

void SJHX_Loop() //时间回显
{
	if(CFCS==0)
	{
		Temp[0]=24;
		Temp[1]=0;
		Temp[2]=0;
		Temp[3]=21;
		Temp[4]=21;
		Temp[5]=21;
		Temp[6]=21;
		Temp[7]=21;	
	}
	else
	{
		Temp[0]=24;
		Temp[1]=CFCS/10;
		Temp[2]=CFCS%10;
		Temp[3]=Hour/10;
		Temp[4]=Hour%10;
		Temp[5]=20;
		Temp[6]=Min/10;
		Temp[7]=Min%10;	
	}
}

void LED_Loop()  //LED灯
{
	if(SJJMStart)
	{
		if(a)
		{
			a=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfe;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		b=1;
	}
	else
	{
		if(b)
		{
			b=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x01;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		a=1;
	}
	if(HXJMStart)
	{
		if(c)
		{
			c=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfd;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		d=1;
	}
	else
	{
		if(d)
		{
			d=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x02;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		c=1;
	}
	if(CSJMStart)
	{
		if(e)
		{
			e=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xfb;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		f=1;
	}
	else
	{
		if(f)
		{
			f=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x04;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		e=1;
	}
	if(CJStart)
	{
		if(L3flag)
		{
			if(L3Start)
			{
				if(g)
				{
					g=0;
					P2=((P2&0x1f)|0x80);
					LEDState&=0xf7;
					P0=LEDState;
					P2&=0x1f;
				}
				else
				h=1;
			}
			if(L3Over)
			{
				if(h)
				{
					h=0;
					P2=((P2&0x1f)|0x80);
					LEDState|=0x08;
					P0=LEDState;
					P2&=0x1f;
				}
				else
				g=1;
			}
		}
	}
	
	if(WX)
	{
		if(j)
		{
			j=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xef;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		k=1;
	}
	else
	{
		if(k)
		{
			k=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x10;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		j=1;
	}
	if(L6Start)
	{
		if(l)
		{
			l=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xdf;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		m=1;
	}
	else
	{
		if(m)
		{
			m=0;
			P2=((P2&0x1f)|0x80);
			LEDState|=0x20;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		l=1;
	}
}

void Mkey_Select()  //矩阵键盘
{
	switch(MkeyNum)
	{
		case 1:
		{
			MkeyNum=0;
			if(SJJMStart)
			{
				SJJMStart=0;
				HXJMStart=1;
			}else if(HXJMStart)
			{
				HXJMStart=0;
				CSJMStart=1;
				WDHXStart=1;
				SDHXStart=0;
				SJHXStart=0;
				TCST=CST;
			}else if(CSJMStart)
			{
				CSJMStart=0;
				SJJMStart=1;
				CST=TCST;
			}
		}break;
		case 2:
		{
			MkeyNum=0;
			if(CSJMStart)
			{
				TCST++;
				if(TCST>99)
				TCST=0;
			}
		}break;
		case 5:
		{
			MkeyNum=0;
			if(HXJMStart)
			{
				if(WDHXStart)
				{
					WDHXStart=0;
					SDHXStart=1;
				}else if(SDHXStart)
				{
					SDHXStart=0;
					SJHXStart=1;
				}else if(SJHXStart)
				{
					SJHXStart=0;
					WDHXStart=1;
				}
				
			}
		}break;
		case 6:
		{
			MkeyNum=0;
			if(CSJMStart)
			{
				TCST--;
				if(TCST<0)
				TCST=99;
			}
			if(SJHXStart)
			Sec2Start=1;
		}break;
	}
}

void main()
{
	Init();
	Timer0Init();
	Timer1Init();
	DS18B20_Read();
	GMData=PCF8591_Read();
	GMData=PCF8591_Read();
	GMData=PCF8591_Read();
	Delay800ms();
	Time_Write();
	while(1)
	{
		if(TimeStart)
		{
			TimeStart=0;
			Time_Read();
		}
		if(DS18B20Start)
		{
			DS18B20Start=0;
			DS18B20_Read();
		}
		if(PCF8591Start)
		{
			PCF8591Start=0;
			GMData=PCF8591_Read();
			CJXY_Loop();
		}
		if(MkeyStart)
		{
			MkeyStart=0;
			if(CJStart==0)
			Mkey_Loop();
		}
		Mkey_Select();
		LED_Loop();
		if(CJStart)
		WSD_Loop();
		else
		{
			if(SJJMStart)
			SJJM_Loop();
			if(CSJMStart)
			CS_Loop();
			if(HXJMStart)
			{
				if(WDHXStart)
				WDHX_Loop();
				if(SDHXStart)
				SDHX_Loop();
				if(SJHXStart)
				SJHX_Loop();
			}
		}
	}
}


