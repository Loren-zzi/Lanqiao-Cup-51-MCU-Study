#include <STC15F2K60S2.H>
#include "iic.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

unsigned char MkeyNum,LedState=0xff,ADCData,Compare,CountL1,CountL2,CountL3,CountL4,AT24C02Data;

unsigned int Count5,Count6,Count7,Count8,Count9,TSTemp;

unsigned char I=1,SJSZ,MSSZ,LiangDu,CCTemp,S8,Count4;

unsigned int TS[]={0,0,0,0,0};

unsigned char CC[]={0,0,0,0,0};

code unsigned char MS[]={0,1,2,3,4};

bit LedStart,LedCount,Led1Start,Led2Start,Led3Start,Led4Start,SZ,LDStart,XianZhi,ADC;

sbit List1=P4^4;
sbit List2=P4^2;
sbit List3=P3^5;
sbit List4=P3^4;
sbit Line1=P3^3;
sbit Line2=P3^2;
sbit Line3=P3^1;
sbit Line4=P3^0;

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


unsigned char Mkey_State()
{
	unsigned char Mkey=0;
	List1=0;
	List2=1;
	List3=1;
	List4=1;
	P3|=0x0f;
	if(Line3==0)
	Mkey=9;
	if(Line4==0)
	Mkey=13;
	
	List1=1;
	List2=0;
	List3=1;
	List4=1;
	P3|=0x0f;
	if(Line1==0)
	Mkey=2;
	if(Line2==0)
	Mkey=6;
	
	return Mkey;
}

void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(LastState==0&NowState==2)
	MkeyNum=2;
	if(LastState==2&NowState==0)
	MkeyNum=2;
	if(LastState==6&NowState==0)
	MkeyNum=6;
	if(LastState==9&NowState==0)
	MkeyNum=9;
	if(LastState==13&NowState==0)
	MkeyNum=13;
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

void Init()
{
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
}

void Timer0Init(void)		//100微秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFc;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
}

void AT24C02_Write(unsigned char Command,Data)
{
	IIC_Start();
	IIC_SendByte(0xA0);
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
	IIC_SendByte(0xA0);
	IIC_WaitAck();
	IIC_SendByte(Command);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0xA0|0x01);
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

void Led_1()
{
	if(++CountL1==7)
	{
		Led2Start=1;
		Led1Start=0;
		CountL4=0;
	}
	P2=((P2&0x1f)|0x80);
	P0=~(0x01<<CountL1);
	LedState=P0;
	P2&=0x1f;
}

void Led_2()
{
	if(++CountL2==7)
	{
		Led3Start=1;
		Led2Start=0;
	}
	P2=((P2&0x1f)|0x80);
	P0=~(0x80>>CountL2);
	LedState=P0;
	P2&=0x1f;
	if(Led3Start==1)
	{
		CountL1=0;
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
		CountL2=0;
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
		Led3Start=0;
		Led4Start=1;
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
		CountL3=0;
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
		Led4Start=0;
		Led1Start=1;
		P2=((P2&0x1f)|0x80);
		P0=0xfe;
		LedState=P0;
		P2&=0x1f;
	}
}

void Timer0_Loop() interrupt 1
{
	static unsigned int Count3;
	static unsigned char Count0,Count1,Count2;
	if(++Count0==2)
	{
		Count0=0;
		Nixie();
	}
	if(++Count1==20)
	{
		Count1=0;
		Mkey_Loop();
	}
	if(ADC==0)
	{
		if(++Count3==400)
		{
			Count3=0;
			ADCData=ADC_Read();
			if(ADCData<=63)
			{
				Compare=2;
				LiangDu=1;
			}
			if(ADCData>63&&ADCData<=127)
			{
				Compare=5;
				LiangDu=2;
			}
			if(ADCData>127&&ADCData<=191)
			{
				Compare=7;
				LiangDu=3;
			}
			if(ADCData>191&&ADCData<=255)
			{
				Compare=10;
				LiangDu=4;
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
					if(CCTemp<10)
					{
						Temp[4]=11;
						Temp[5]=TSTemp/100;
						Temp[6]=TSTemp%100/10;
						Temp[7]=TSTemp%100%10;
					}
					if(CCTemp>=10)
					{
						Temp[4]=TSTemp/1000;
						Temp[5]=TSTemp%1000/100;
						Temp[6]=TSTemp%1000%100/10;
						Temp[7]=TSTemp%1000%100%10;
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
					if(CCTemp<10)
					{
						Temp[4]=11;
						Temp[5]=TSTemp/100;
						Temp[6]=TSTemp%100/10;
						Temp[7]=TSTemp%100%10;
					}
					if(CCTemp>=10)
					{
						Temp[4]=TSTemp/1000;
						Temp[5]=TSTemp%1000/100;
						Temp[6]=TSTemp%1000%100/10;
						Temp[7]=TSTemp%1000%100%10;
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
					if(CCTemp<10)
					{
						Temp[4]=11;
						Temp[5]=TSTemp/100;
						Temp[6]=TSTemp%100/10;
						Temp[7]=TSTemp%100%10;
					}
					if(CCTemp>=10)
					{
						Temp[4]=TSTemp/1000;
						Temp[5]=TSTemp%1000/100;
						Temp[6]=TSTemp%1000%100/10;
						Temp[7]=TSTemp%1000%100%10;
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
	if(LedStart)
	{
		if(Led1Start==1)
		{
			if(++Count5>=TS[1])
			{
				Count5=0;
				Led_1();
			}
		}
		if(Led2Start==1)
		{
			if(++Count6>=TS[2])
			{
				Count6=0;
				Led_2();
			}
		}
		if(Led3Start==1)
		{
			if(++Count7>=TS[3])
			{
				Count7=0;
				Led_3();
			}
		}
		if(Led4Start==1)
		{
			if(++Count8>=TS[4])
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

void SheZhi()
{
	static unsigned char i;
	switch(MkeyNum)
	{
		case 13:
		{
			SZ=0;
		}break;
		case 9:
		{
			MkeyNum=0;
			switch(i)
			{
				case 0:
				{
					MSSZ=0;
					SJSZ=1;
					i=1;
					Count4=0;
					Count9=0;
				}break;
				case 1:
				{
					MSSZ=1;
					SJSZ=0;
					i=0;
					Count4=0;
					Count9=0;
				}break;
			}
			
		}break;
		case 6:
		{
			MkeyNum=0;
			if(MSSZ==1|MSSZ==2)
			{
				if(I==4)
				I=3;
				I++;
				TSTemp=TS[I];
				CCTemp=CC[I];
			}
			else
			{
				if(CCTemp==12)
				CCTemp=11;
				CCTemp++;
				TSTemp=(int)(CCTemp)*100;
				if(ADC==0)
				{
					ADC=1;
					AT24C02_Write(I,CCTemp);
					Delay5ms();
					ADC=0;
				}
				if(ADC==0)
				{
					ADC=1;
					AT24C02Data=AT24C02_Read(I);
					CC[I]=AT24C02Data;
					TS[I]=(unsigned int)(AT24C02Data)*100;
					ADC=0;
				}
			}
		}break;
		case 2:
		{
			MkeyNum=0;
			switch(S8)
			{
				case 0:
				{
					S8=1;
				}break;
				case 1:
				{
					S8=0;
					if(MSSZ==1|MSSZ==2)
					{
						if(I==1)
						I=2;
						I--;
						TSTemp=TS[I];
						CCTemp=CC[I];
					}
					else
					{
						if(CCTemp==4)
						CCTemp=5;
						CCTemp--;
						TSTemp=(unsigned int)(CCTemp)*100;
						if(ADC==0)
						{
							ADC=1;
							AT24C02_Write(I,CCTemp);
							Delay5ms();
							ADC=0;
						}
						if(ADC==0)
						{
							ADC=1;
							AT24C02Data=AT24C02_Read(I);
							CC[I]=AT24C02Data;
							TS[I]=(unsigned int)(AT24C02Data)*100;
							ADC=0;
						}
					}
				}break;
			}
		}break;
	}
}

void LiangDu_Loop()
{
	if(MkeyNum==2)
	{
		MkeyNum=0;
		LDStart=0;
	}
	Temp[6]=10;
	Temp[7]=LiangDu;
}

void Mkey_Select()
{
		switch(MkeyNum)
		{
			case 13:
			{
				MkeyNum=0;
				I=1;
//				if(ADC==0)
//				{
//					ADC=1;
//					AT24C02Data=AT24C02_Read(1);
//					CC[1]=AT24C02Data;
//					TS[1]=(unsigned int)(AT24C02Data)*100;
//					AT24C02Data=AT24C02_Read(2);
//					CC[2]=AT24C02Data;
//					TS[2]=(unsigned int)(AT24C02Data)*100;
//					AT24C02Data=AT24C02_Read(3);
//					CC[3]=AT24C02Data;
//					TS[3]=(unsigned int)(AT24C02Data)*100;
//					AT24C02Data=AT24C02_Read(4);
//					CC[4]=AT24C02Data;
//					TS[4]=(unsigned int)(AT24C02Data)*100;
//					ADC=0;
//				}
				if(LedCount==0)
				{
					LedCount=1;
					LedStart=1;
					Led1Start=1;
					P2=((P2&0x1f)|0x80);
					P0=0xfe;
					LedState=P0;
					P2&=0x1f;
				}
				else
				{
					LedCount=0;
					LedStart=0;
					Led2Start=0;
					Led3Start=0;
					Led4Start=0;
					P2=((P2&0x1f)|0x80);
					P0=0xff;
					LedState=P0;
					P2&=0x1f;
					Count5=0;
					Count6=0;
					Count7=0;
					Count8=0;
					CountL1=0;
					CountL2=0;
					CountL3=0;
					CountL4=0;
				}
			}break;
			case 9:
			{
				MkeyNum=0;
				if(SZ==0)
				{
					SZ=1;
					MSSZ=1;
					SJSZ=0;
					Count4=0;
					Count9=0;
//					if(ADC==0)
//					{
//						ADC=1;
//						AT24C02Data=AT24C02_Read(1);
//						CC[1]=AT24C02Data;
//						TS[1]=(unsigned int)(AT24C02Data)*100;
//						AT24C02Data=AT24C02_Read(2);
//						CC[2]=AT24C02Data;
//						TS[2]=(unsigned int)(AT24C02Data)*100;
//						AT24C02Data=AT24C02_Read(3);
//						CC[3]=AT24C02Data;
//						TS[3]=(unsigned int)(AT24C02Data)*100;
//						AT24C02Data=AT24C02_Read(4);
//						CC[4]=AT24C02Data;
//						TS[4]=(unsigned int)(AT24C02Data)*100;
//						ADC=0;
//					}
					TSTemp=TS[1];
					CCTemp=CC[1];
					Temp[0]=11;
					Temp[1]=11;
					Temp[2]=11;
					if(CCTemp<10)
					{
						Temp[4]=11;
						Temp[5]=TSTemp/100;
						Temp[6]=TSTemp%100/10;
						Temp[7]=TSTemp%100%10;
					}
					if(CCTemp>=10)
					{
						Temp[4]=TSTemp/1000;
						Temp[5]=TSTemp%1000/100;
						Temp[6]=TSTemp%1000%100/10;
						Temp[7]=TSTemp%1000%100%10;
					}
				}
			}break;
			case 2:
			{
				MkeyNum=0;
				if(LDStart==0)
				LDStart=1;
			}break;
		}
}

void main()
{
	Init();	
	AT24C02Data=AT24C02_Read(1);
	if(AT24C02Data<4||AT24C02Data>12)
	AT24C02Data=4;//防止用上一个人的错误数据
	CC[1]=AT24C02Data;
	TS[1]=(unsigned int)(AT24C02Data)*100;
	AT24C02Data=AT24C02_Read(2);
	if(AT24C02Data<4||AT24C02Data>12)
	AT24C02Data=4;
	CC[2]=AT24C02Data;
	TS[2]=(unsigned int)(AT24C02Data)*100;
	AT24C02Data=AT24C02_Read(3);
	if(AT24C02Data<4||AT24C02Data>12)
	AT24C02Data=4;
	CC[3]=AT24C02Data;
	TS[3]=(unsigned int)(AT24C02Data)*100;
	AT24C02Data=AT24C02_Read(4);
	if(AT24C02Data<4||AT24C02Data>12)
	AT24C02Data=4;
	CC[4]=AT24C02Data;
	TS[4]=(unsigned int)(AT24C02Data)*100;
	Timer0Init();
	while(1)
	{
		Mkey_Select();
		while(SZ==1)
		SheZhi();
		while(LDStart==1)
		LiangDu_Loop();
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