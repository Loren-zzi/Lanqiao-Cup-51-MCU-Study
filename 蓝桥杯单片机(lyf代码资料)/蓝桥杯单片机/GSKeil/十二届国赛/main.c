#include <STC15F2K60S2.H>
#include "iic.h"
#include "ds1302.h"
#include "intrins.h"

#define WP 0x8e

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xbf,0xff,
													0xc7,0xc6,0x8e,0x89,0xfe,0xbf,0xf7,0x8c};
code unsigned char TimeWCommand[]={0x84,0x82,0x80};
code unsigned char TimeRCommand[]={0x85,0x83,0x81};
unsigned char Time[]={0x20,0x20,0x01};

unsigned char Temp[]={21,21,21,21,21,21,21,21};

sbit TX=P1^0;
sbit RX=P1^1;

unsigned char MkeyNum,Sec,CJSJ=2,TCJSJ=2,JLCS=20,TJLCS=20,GMData,LEDState=0xff,CS;

float CSBPJ,SCSB=0;

unsigned int x,t,CSBMax=0,CSBMin=0,i,Count3;

bit MkeyStart,SJSJXSJMStart=1,TimeStart,CSBStart,JLCJXSJMStart,SJXSJM=1,SJJLXSJM1Start;
bit SJJLXSJM2Start,SJJLXSJM3Start,CSSZJM,CJSJSZJMStart=1,JLCSSZJMStart,GMStart,CFMSStart=1,DSMSStart;
bit S1Start,L5Start;
bit a=1,b=1,c=1,d=1,e=1,f=1,g=1,h=1,j=1,k=1,l=1,m=1;
void Delay12us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
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
	MkeyNum=6;
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
	Sec=Time[2]/16*10+Time[2]%16;
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

unsigned char GM_Read()
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

void CFCJ_Loop()   //触发采集
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=GMData;
	if(NowState<20&&LastState>20)
	{
		CSBStart=1;
		i++;
	}
}

void Timer1Init(void)		//65536??@12.000MHz
{
	AUXR &= 0xBF;		//?????12T??
	TMOD &= 0x0F;		//???????
	TL1 = 0x00;		//??????
	TH1 = 0x00;		//??????
	TF1 = 0;		//??TF1??
	TR1 = 0;		//???1????
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
	static unsigned char Count0,Count1,Count2;
	Nixie();
	if(++Count0>=10)
	{
		Count0=0;
		MkeyStart=1;
	}
	if(++Count1>=25)
	{
		Count1=0;
		TimeStart=1;
	}
	if(++Count2>=70)
	{
		Count2=0;
		GMStart=1;
	}
	if(S1Start)
	{
		if(++Count3>=500)
		{
			Count3=0;
			S1Start=0;
		}
	}
}

void CSBCL_Loop()  //采集数据处理
{
	if(CSBMax<x)
	CSBMax=x;
	if(CSBMin>x)
	CSBMin=x;
	SCSB+=x;
	CSBPJ=SCSB/i;
}

void DSCJ_Loop() //定时采集
{
	if(S1Start==0)
	{
		if(Sec%CJSJ==0)
		{
			CSBStart=1;
			S1Start=1;
			i++;
		}
	}
}

void CSB_Loop()
{
	unsigned char i;
	for(i=0;i<9;i++)
	{
		TX=1;
		Delay12us();
		TX=0;
		Delay12us();
	}
	TR1=1;
	while(RX==1&&TF1==0);
	TR1=0;
	if(TF1)
	{
		TF1=0;
		x=999;
	}
	else
	{
		RX=1;
		t=TH1<<8|TL1;
		x=t*0.017;
	}
	TH1=0;
	TL1=0;
}

void SJSJXSJM_Loop()   //时间数据显示界面
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

void JLCJXSJM_Loop()  //距离测距显示界面
{
	if(CFMSStart)
	{
		Temp[0]=22;
		Temp[1]=23;
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=21;
		if(x<10)
		{
			Temp[5]=21;
			Temp[6]=21;
			Temp[7]=x%10;
		}
		if(x>=10&&x<100)
		{
			Temp[5]=21;
			Temp[6]=x/10;
			Temp[7]=x%10;
		}
		if(x>=100&&x<1000)
		{
			Temp[5]=x/100;
			Temp[6]=x%100/10;
			Temp[7]=x%10;
		}
	}
	if(DSMSStart)
	{
		Temp[0]=22;
		Temp[1]=24;
		Temp[2]=21;
		Temp[3]=21;
		Temp[4]=21;
		if(x<10)
		{
			Temp[5]=21;
			Temp[6]=21;
			Temp[7]=x%10;
		}
		if(x>=10&&x<100)
		{
			Temp[5]=21;
			Temp[6]=x/10;
			Temp[7]=x%10;
		}
		if(x>=100&&x<1000)
		{
			Temp[5]=x/100;
			Temp[6]=x%100/10;
			Temp[7]=x%10;
		}
	}
}

void SJJLXSJM1_Loop()  //数据记录显示界面最大值
{
	Temp[0]=25;
	Temp[1]=26;
	Temp[2]=21;
	Temp[3]=21;
	Temp[4]=21;
	if(CSBMax<10)
	{
		Temp[5]=21;
		Temp[6]=21;
		Temp[7]=CSBMax%10;
	}
	if(CSBMax>=10&&CSBMax<100)
	{
		Temp[5]=21;
		Temp[6]=CSBMax/10;
		Temp[7]=CSBMax%10;
	}
	if(CSBMax>=100&&CSBMax<1000)
	{
		Temp[5]=CSBMax/100;
		Temp[6]=CSBMax%100/10;
		Temp[7]=CSBMax%10;
	}
}

void SJJLXSJM2_Loop()   //数据记录显示界面平均值
{
	Temp[0]=25;
	Temp[1]=27;
	Temp[2]=21;
	Temp[3]=21;
	if((int)(CSBPJ*10)<10)
	{
		Temp[4]=21;
		Temp[5]=21;
		Temp[6]=0+10;
		Temp[7]=(int)(CSBPJ*10)%10;
	}
	if((int)(CSBPJ*10)>=10&&(int)(CSBPJ*10)<100)
	{
		Temp[4]=21;
		Temp[5]=21;
		Temp[6]=(int)(CSBPJ*10)/10+10;
		Temp[7]=(int)(CSBPJ*10)%10;
	}
	if((int)(CSBPJ*10)>=100&&(int)(CSBPJ*10)<1000)
	{
		Temp[4]=21;
		Temp[5]=(int)(CSBPJ*10)/100;
		Temp[6]=(int)(CSBPJ*10)%100/10+10;
		Temp[7]=(int)(CSBPJ*10)%10;
	}
	if((int)(CSBPJ*10)>=1000)
	{
		Temp[4]=(int)(CSBPJ*10)/1000;
		Temp[5]=(int)(CSBPJ*10)%1000/100;
		Temp[6]=(int)(CSBPJ*10)%1000%100/10+10;
		Temp[7]=(int)(CSBPJ*10)%10;
	}
}

void SJJLXSJM3_Loop()   //数据记录显示界面最小值
{
	Temp[0]=25;
	Temp[1]=28;
	Temp[2]=21;
	Temp[3]=21;
	Temp[4]=21;
	if(CSBMin<10)
	{
		Temp[5]=21;
		Temp[6]=21;
		Temp[7]=CSBMin%10;
	}
	if(CSBMin>=10&&CSBMin<100)
	{
		Temp[5]=21;
		Temp[6]=CSBMin/10;
		Temp[7]=CSBMin%10;
	}
	if(CSBMin>=100&&CSBMin<1000)
	{
		Temp[5]=CSBMin/100;
		Temp[6]=CSBMin%100/10;
		Temp[7]=CSBMin%10;
	}
}

void CJSJSZJM_Loop()  //采集时间设置界面
{
	Temp[0]=29;
	Temp[1]=1;
	Temp[2]=21;
	Temp[3]=21;
	Temp[4]=21;
	Temp[5]=21;
	Temp[6]=TCJSJ/10;
	Temp[7]=TCJSJ%10;
}

void JLCSSZJM_Loop() //距离参数设置界面
{
	Temp[0]=29;
	Temp[1]=2;
	Temp[2]=21;
	Temp[3]=21;
	Temp[4]=21;
	Temp[5]=21;
	Temp[6]=TJLCS/10;
	Temp[7]=TJLCS%10;
}

void DAC_Loop()
{
	if(x<=10)
	DAC_Write(51);
	else if(x>=80)
	DAC_Write(255);
	else
	DAC_Write((x*0.0571+0.429)/5.0*255);
}

void LED_State()
{
	if(SJSJXSJMStart&&SJXSJM)
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
	if(JLCJXSJMStart)
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
	if(SJJLXSJM1Start||SJJLXSJM2Start||SJJLXSJM3Start)
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
	if(CFMSStart)
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
	else
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
	if(GMData>20)
	{
		if(j)
		{
			j=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xdf;
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
			LEDState|=0x20;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		j=1;
	}
	if(L5Start)
	{
		if(l)
		{
			l=0;
			P2=((P2&0x1f)|0x80);
			LEDState&=0xef;
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
			LEDState|=0x10;
			P0=LEDState;
			P2&=0x1f;
		}
		else
		l=1;
	}
}

void Mkey_Select()
{
	switch(MkeyNum)
	{
		case 1:
		{
			MkeyNum=0;
			if(SJXSJM)
			{
				SJXSJM=0;
				CSSZJM=1;
				SJSJXSJMStart=1;
				JLCJXSJMStart=0;
				SJJLXSJM1Start=0;
				SJJLXSJM2Start=0;
				SJJLXSJM3Start=0;
				TCJSJ=CJSJ;
			}
			else
			{
				SJXSJM=1;
				CSSZJM=0;
				CJSJSZJMStart=1;
				JLCSSZJMStart=0;
				CJSJ=TCJSJ;
				JLCS=TJLCS;
			}
		}break;
		case 2:
		{
			MkeyNum=0;
			if(SJJLXSJM1Start)
			{
				SJJLXSJM1Start=0;
				SJJLXSJM2Start=1;
			}else if(SJJLXSJM2Start)
			{
				SJJLXSJM2Start=0;
				SJJLXSJM3Start=1;
			}
			else if(SJJLXSJM1Start||SJJLXSJM3Start||SJJLXSJM3Start)
			{
				SJJLXSJM3Start=0;
				SJJLXSJM1Start=1;
			}
			if(JLCJXSJMStart)
			{
				if(CFMSStart)
				{
					CFMSStart=0;
					DSMSStart=1;
				}else if(DSMSStart)
				{
					CFMSStart=1;
					DSMSStart=0;
				}
			}
		}break;
		case 5:
		{
			MkeyNum=0;
			if(SJXSJM)
			{
				if(SJSJXSJMStart)
				{
					SJSJXSJMStart=0;
					JLCJXSJMStart=1;
				}else if(JLCJXSJMStart)
				{
					JLCJXSJMStart=0;
					SJJLXSJM1Start=1;
				}
				else
				{
					SJJLXSJM1Start=0;
					SJJLXSJM2Start=0;
					SJJLXSJM3Start=0;
					SJSJXSJMStart=1;
				}
			}
			if(CSSZJM)
			{
				if(CJSJSZJMStart)
				{
					CJSJSZJMStart=0;
					JLCSSZJMStart=1;
					TJLCS=JLCS;
					CJSJ=TCJSJ;
				}
				else
				{
					CJSJSZJMStart=1;
					JLCSSZJMStart=0;
					JLCS=TJLCS;
					TCJSJ=CJSJ;
				}
			}
		}break;
		case 6:
		{
			MkeyNum=0;
			if(CJSJSZJMStart)
			{
				if(TCJSJ==2)
				TCJSJ=3;
				else if(TCJSJ==3)
				TCJSJ=5;
				else if(TCJSJ==5)
				TCJSJ=7;
				else if(TCJSJ==7)
				TCJSJ=9;
				else if(TCJSJ==9)
				TCJSJ=2;
			}
			if(JLCSSZJMStart)
			{
				if(TJLCS==10)
				TJLCS=20;
				else if(TJLCS==20)
				TJLCS=30;
				else if(TJLCS==20)
				TJLCS=30;
				else if(TJLCS==30)
				TJLCS=40;
				else if(TJLCS==40)
				TJLCS=50;
				else if(TJLCS==50)
				TJLCS=60;
				else if(TJLCS==60)
				TJLCS=70;
				else if(TJLCS==70)
				TJLCS=80;
				else if(TJLCS==80)
				TJLCS=10;
			}
		}break;
	}
}

void main()
{
	Init();
	Time_Write();
	GMData=GM_Read();
	GMData=GM_Read();
	GMData=GM_Read();
	Timer0Init();
	Timer1Init();
	while(1)
	{
		if(TimeStart)
		{
			TimeStart=0;
			Time_Read();
			if(DSMSStart)
			DSCJ_Loop();
		}
		if(GMStart)
		{
			GMStart=0;
			GMData=GM_Read();
			if(CFMSStart)
			CFCJ_Loop();
		}
		if(CSBStart)
		{
			CSBStart=0;
			if(x==0)
			{
				CSB_Loop();
				CSBMax=x;
				CSBMin=x;
				CSBPJ=x;
			}
			else
			CSB_Loop();
			CSBCL_Loop();
			DAC_Loop();
			if(x>(JLCS-5)&&x<(JLCS+5))
			CS++;
			else
			{
				CS=0;
				L5Start=0;
			}
			if(CS>=3)
			{
				CS=3;
				L5Start=1;
			}
		}
		if(MkeyStart)
		{
			MkeyStart=0;
			Mkey_Loop();
		}
		Mkey_Select();
		LED_State();
		if(SJXSJM)
		{
			if(SJSJXSJMStart)
			SJSJXSJM_Loop();
			if(JLCJXSJMStart)
			JLCJXSJM_Loop();
			if(SJJLXSJM1Start)
			SJJLXSJM1_Loop();
			if(SJJLXSJM2Start)
			SJJLXSJM2_Loop();
			if(SJJLXSJM3Start)
			SJJLXSJM3_Loop();
		}
		if(CSSZJM)
		{
			if(CJSJSZJMStart)
			CJSJSZJM_Loop();
			if(JLCSSZJMStart)
			JLCSSZJM_Loop();
		}
	}
}

