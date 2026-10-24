#include <STC15F2K60S2.H>
#include "CLED.h"
#include "CBuzz.h"
#include "AT24C02.h"
#include "Nixie.h"
#include "Delay.h"
#include "Timer0.h"
#include "DS1302.h"

sbit List_1=P4^4;
sbit List_2=P4^2;
sbit List_3=P3^5;
sbit List_4=P3^4;
sbit Line_1=P3^3;
sbit Line_2=P3^2;
sbit Line_3=P3^1;
sbit Line_4=P3^0;

unsigned char Data,MKeyNum,Min,Sec,MiniSec,Start;

unsigned char Bit1,Bit2,Bit4,Bit5,Bit7,Bit8,Bit3,Bit6;

unsigned char ShiJian=1,RiQi,XingQi;

unsigned char ShiZhong=1,MiaoBiao=0,NaoZhong=0,Switch_NaoZhong=0,Timer=0,Show_Timer=0;

signed char  NState=0,NHour,NMin,TempNHour,TempNMin,State_Timer=0;

signed char THour,TMin,TSec,TMiniSec,Timer_Control=0,TempTHour,TempTMin,TempTSec,P_Buzzer=0;

signed char Move=0;

unsigned char Matrixkey_State()
{
	unsigned char Key=0;
	List_1=0;
	List_2=1;
	List_3=1;
	List_4=1;
	Line_1=1;
	Line_2=1;
	Line_3=1;
	Line_4=1;
	if(Line_1==0)
	Key=1;		

	if(Line_2==0)
	Key=5;
	
	if(Line_3==0)
	Key=9;
	
	if(Line_4==0)
	Key=13;
	
	List_1=1;
	List_2=0;
	List_3=1;
	List_4=1;
	Line_1=1;
	Line_2=1;
	Line_3=1;
	Line_4=1;

	if(Line_1==0)
	Key=2;		

	if(Line_2==0)
	Key=6;

	if(Line_3==0)
	Key=10;
	
	if(Line_4==0)
	Key=14;
	
	List_1=1;
	List_2=1;
	List_3=0;
	List_4=1;
	Line_1=1;
	Line_2=1;
	Line_3=1;
	Line_4=1;
	if(Line_1==0)
	Key=3;		
	
	if(Line_2==0)
	Key=7;
	
	if(Line_3==0)
	Key=11;

	if(Line_4==0)
	Key=15;
	
	List_1=1;
	List_2=1;
	List_3=1;
	List_4=0;
	Line_1=1;
	Line_2=1;
	Line_3=1;
	Line_4=1;
	if(Line_1==0)
	Key=4;		
	
	if(Line_2==0)
	Key=8;
	
	if(Line_3==0)
	Key=12;
	
	if(Line_4==0)
	Key=16;
	
	return Key;
}

void MKey_Loop(void)
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Matrixkey_State();
	if(NowState==0 && LastState==1)
	MKeyNum=1;
	if(NowState==0 && LastState==2)
	MKeyNum=2;
	if(NowState==0 && LastState==3)
	MKeyNum=3;
	if(NowState==0 && LastState==4)
	MKeyNum=4;
	if(NowState==0 && LastState==5)
	MKeyNum=5;
	if(NowState==0 && LastState==6)
	MKeyNum=6;
	if(NowState==0 && LastState==7)
	MKeyNum=7;
	if(NowState==0 && LastState==8)
	MKeyNum=8;
	if(NowState==0 && LastState==9)
	MKeyNum=9;
	if(NowState==0 && LastState==10)
	MKeyNum=10;
	if(NowState==0 && LastState==11)
	MKeyNum=11;
	if(NowState==0 && LastState==12)
	MKeyNum=12;
	if(NowState==0 && LastState==13)
	MKeyNum=13;
	if(NowState==0 && LastState==14)
	MKeyNum=14;
	if(NowState==0 && LastState==15)
	MKeyNum=15;
	if(NowState==0 && LastState==16)
	MKeyNum=16;
}

void Sec_Loop()
{
	if(Start)
	{
		MiniSec++;
		if(MiniSec==100)
		{
			MiniSec=0;
			Sec++;
			if(Sec==60)
			{
				Sec=0;
				Min++;
				if(Min==100)
				{
					Min=0;
				}
			}
		}
	}
}

void LED_Loop()
{
	P2=((P2&0x1f)|0x80);
	P0=~(0x01<<Move);
	P2&=0x1f;
	Move++;
	if(Move==8)
	Move=0;
}

void NaoZhong_Loop()
{	
	unsigned char NowHour,NowMin;
	DS1302_ReadTime();
	NowHour=DS1302_Time[3];
	NowMin=DS1302_Time[4];
	if(NowHour>=NHour&&NowMin>=NMin&&NState==1)
	{
		P2&=0x1F;
		P2|=0xA0;
		P0=0x40;//P0^6记好了
		P2&=0x1F;
	}
}


void Timer_Loop()
{
	if(State_Timer==1)
	{
		TMiniSec++;//想更准确，加个Mini秒
		if(TMiniSec==100)
		{
			TMiniSec=0;
			TSec--;
			Timer_Control++;
		}
			
		if(TSec<0)
		{
			TSec=59;
			TMin--;
		}
			
		if(TMin<0)
		{
			TMin=59;
			THour--;			
		}	
			
		if(THour<0)
		{
			THour=0;
			TMin=0;
			TSec=0;
			TMiniSec=0;
			P2&=0x1f;
			P2|=0xA0;
			P0=0x40;
			P2&=0x1f;
		}	
	}
	if(P_Buzzer==1)
	Timer_Control=0;
}

void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count1=0,T0Count2=0,T0Count3=0,T0Count4=0,T0Count5=0,T0Count6=0;
	TH0=(65536-917)/256;		//重装初值
	TL0=(65536-917)%256;	
	if(++T0Count6>=1000)
	{
		T0Count6=0;
		LED_Loop();
	}
	if(++T0Count1>=21)
	{
		T0Count1=0;
		MKey_Loop();
	}
	if(++T0Count2>=2)
	{
		T0Count2=0;
		Nixie_Loop();
	}
	if(++T0Count3>=10)
	{
		T0Count3=0;
		Sec_Loop();
	}
	if(++T0Count4>=100)
	{
		T0Count4=0;
		NaoZhong_Loop();
	}
	if(++T0Count5>=10)
	{
		T0Count5=0;
		Timer_Loop();
	}
}

void GongNeng_Select()
{
			if(MKeyNum==1)
		{
			MKeyNum=0;
			ShiZhong=1;
			MiaoBiao=0;
			NaoZhong=0;
			Switch_NaoZhong=0;
			Timer=0;
			Show_Timer=0;
		}		
		
		if(MKeyNum==2)
		{
			MKeyNum=0;
			ShiZhong=0;
			MiaoBiao=1;
			NaoZhong=0;
			Switch_NaoZhong=0;
			Timer=0;
			Show_Timer=0;
		}	
		
		if(MKeyNum==3)
		{
			MKeyNum=0;
			ShiZhong=0;
			MiaoBiao=0;
			NaoZhong=1;	
			Switch_NaoZhong=0;
			Timer=0;
			Show_Timer=0;
		}	
		
		if(MKeyNum==4)
		{
			MKeyNum=0;
			ShiZhong=0;
			MiaoBiao=0;
			NaoZhong=0;
			Switch_NaoZhong=1;
			Timer=0;
			Show_Timer=0;
		}
		
		if(MKeyNum==5)
		{
			MKeyNum=0;
			ShiZhong=0;
			MiaoBiao=0;
			NaoZhong=0;
			Switch_NaoZhong=0;
			Timer=1;
			Show_Timer=0;
			if(Timer_Control==0)
			{
				THour=AT24C02_ReadByte(5);
				TMin=AT24C02_ReadByte(6);
				TSec=AT24C02_ReadByte(7);
				TempTHour=THour;
				TempTMin=TMin;
				TempTSec=TSec;
			}
		}
		
		if(MKeyNum==6)
		{
			MKeyNum=0;
			ShiZhong=0;
			MiaoBiao=0;
			NaoZhong=0;
			Switch_NaoZhong=0;
			Timer=0;
			Show_Timer=1;
			THour=AT24C02_ReadByte(5);
			TMin=AT24C02_ReadByte(6);
			TSec=AT24C02_ReadByte(7);
		}
		
		if(MKeyNum==7)
		{
			MKeyNum=0;
			State_Timer=0;
			P_Buzzer=1;
			TMiniSec=0;
			P2&=0x1F;
			P2|=0xA0;
			P0=0x00;//P0^6记好了
			P2&=0x1F;
		}

}

void MB()
{
		while(MiaoBiao)
		{
			if(MKeyNum==2)
			{
				MKeyNum=0;
				MiaoBiao=0;//试一下
			}
			if(MKeyNum==5)
			{
				MKeyNum=0;
				Start=!Start;
			}
			if(MKeyNum==6)
			{
				MKeyNum=0;
				Min=0;
				Sec=0;
				MiniSec=0;
			}
			if(MKeyNum==7)
			{
				MKeyNum=0;
				AT24C02_WriteByte(0,Min);
				Delay(5);//写周期最长5ms，要给芯片时间先擦除再写入，写完之后立刻读会出错
				AT24C02_WriteByte(1,Sec);
				Delay(5);//写周期最长5ms，要给芯片时间先擦除再写入
				AT24C02_WriteByte(2,MiniSec);
				Delay(5);//写周期最长5ms，要给芯片时间先擦除再写入
			}
			if(MKeyNum==8)
			{
				MKeyNum=0;
				Data=AT24C02_ReadByte(0);
				Min=Data;
				Data=AT24C02_ReadByte(1);
				Sec=Data;
				Data=AT24C02_ReadByte(2);
				MiniSec=Data;
			}
			Nixie(1,Min/10);
			Nixie(2,Min%10);
			Nixie(3,11);
			Nixie(4,Sec/10);
			Nixie(5,Sec%10);
			Nixie(6,11);
			Nixie(7,MiniSec/10);
			Nixie(8,MiniSec%10);
		}
}

void SZ()
{
		while(ShiZhong)
		{
			if(MKeyNum==1)
			{
				MKeyNum=0;
				ShiZhong=0;
			}	
			if(MKeyNum==2)
			{
				MKeyNum=0;
				ShiJian=1;
				RiQi=0;
				XingQi=0;
			}
			if(MKeyNum==3)
			{
				MKeyNum=0;
				RiQi=1;
				ShiJian=0;
				XingQi=0;
			}
			if(MKeyNum==4)
			{
				MKeyNum=0;
				XingQi=1;
				ShiJian=0;
				RiQi=0;
			}
			if(ShiJian)
			{
				DS1302_ReadTime();
				Bit3=11;
				Bit6=11;
				Bit8=DS1302_Time[5]%10;
				Bit7=DS1302_Time[5]/10;
				Bit5=DS1302_Time[4]%10;
				Bit4=DS1302_Time[4]/10;
				Bit2=DS1302_Time[3]%10;
				Bit1=DS1302_Time[3]/10;
			}
			if(RiQi)
			{
				Bit3=11;
				Bit6=11;
				Bit8=DS1302_Time[2]%10;
				Bit7=DS1302_Time[2]/10;
				Bit5=DS1302_Time[1]%10;
				Bit4=DS1302_Time[1]/10;
				Bit2=DS1302_Time[0]%10;
				Bit1=DS1302_Time[0]/10;
			}
			if(XingQi)
			{
				Bit3=10;
				Bit6=0;
				Bit8=DS1302_Time[6]%10;
				Bit7=0;
				Bit5=10;
				Bit4=10;
				Bit2=10;
				Bit1=10;
			}
			Nixie(1,Bit1);
			Nixie(2,Bit2);
			Nixie(3,Bit3);
			Nixie(4,Bit4);
			Nixie(5,Bit5);
			Nixie(6,Bit6);
			Nixie(7,Bit7);
			Nixie(8,Bit8);
		}	
}

void TNZ()
{
		while(NaoZhong)
		{
			Nixie(1,TempNHour/10);
			Nixie(2,TempNHour%10);
			Nixie(3,11);
			Nixie(4,TempNMin/10);
			Nixie(5,TempNMin%10);
			Nixie(6,11);
			Nixie(7,0);
			Nixie(8,0);
			if(MKeyNum==3)
			{
				MKeyNum=0;
				NaoZhong=0;
			}
			if(MKeyNum==5)
			{
				MKeyNum=0;
				TempNMin++;
				if(TempNMin==60)
				{
					TempNMin=0;
				}
			}
			if(MKeyNum==6)
			{
				MKeyNum=0;
				TempNMin--;
				if(TempNMin<0)
				{
					TempNMin=59;
				}
			}
			if(MKeyNum==7)
			{
				MKeyNum=0;
				TempNHour++;
				if(TempNHour==24)
				{
					TempNHour=0;
				}
			}
			if(MKeyNum==8)
			{
				MKeyNum=0;
				TempNHour--;
				if(TempNHour<0)
				{
					TempNHour=23;
				}
			}
			if(MKeyNum==9)
			{
				MKeyNum=0;
				AT24C02_WriteByte(3,TempNHour);
				NHour=TempNHour;
				Delay(5);//写周期最长5ms，要给芯片时间先擦除再写入，写完之后立刻读会出错
				AT24C02_WriteByte(4,TempNMin);
				NMin=TempNMin;
				Delay(5);//写周期最长5ms，要给芯片时间先擦除再写入
			}
			if(MKeyNum==10)
			{
				MKeyNum=0;
				Data=AT24C02_ReadByte(3);
				NHour=Data;
				TempNHour=NHour;
				Data=AT24C02_ReadByte(4);
				NMin=Data;
				TempNMin=NMin;
			}
		}
}

void SNZ()
{
		while(Switch_NaoZhong)
		{
			Nixie(1,10);
			Nixie(2,10);
			Nixie(3,10);
			Nixie(4,10);
			Nixie(5,10);
			Nixie(6,10);
			Nixie(7,10);
			Nixie(8,NState);
			if(NState==2)
			{
				NState=0;
				P2&=0x1F;
				P2|=0xA0;
				P0=0x00;//P0^6记好了
				P2&=0x1F;
			}
			if(MKeyNum==4)
			{
				MKeyNum=0;
				Switch_NaoZhong=0;
			}
			
			if(MKeyNum==1)
			{
				MKeyNum=0;
				NState++;
			}
		}
}

void SZJSQ()
{
		while(Timer==1&&Timer_Control==0)
		{
			if(MKeyNum==5)
			{
				MKeyNum=0;
				Timer=0;
			}
			
			if(MKeyNum==9)
			{
				MKeyNum=0;
				TempTHour++;
				if(TempTHour==24)
				{
					TempTHour=0;
				}
			}
			if(MKeyNum==10)
			{
				MKeyNum=0;
				TempTHour--;
				if(TempTHour<0)
				{
					TempTHour=23;
				}
			}
			if(MKeyNum==11)
			{
				MKeyNum=0;
				TempTMin++;
				if(TempTMin==60)
				{
					TempTMin=0;
				}
			}
			if(MKeyNum==12)
			{
				MKeyNum=0;
				TempTMin--;
				if(TempTMin<0)
				{
					TempTMin=59;
				}
			}
			if(MKeyNum==13)
			{
				MKeyNum=0;
				TempTSec++;
				if(TempTSec==60)
				{
					TempTSec=0;
				}
			}
			if(MKeyNum==14)
			{
				MKeyNum=0;
				TempTSec--;
				if(TSec<0)
				{
					TempTSec=59;
				}
			}
			if(MKeyNum==15)
			{
				MKeyNum=0;
				AT24C02_WriteByte(5,TempTHour);				
				Delay(5);
				THour=TempTHour;
				AT24C02_WriteByte(6,TempTMin);				
				Delay(5);
				TMin=TempTMin;
				AT24C02_WriteByte(7,TempTSec);
				Delay(5);
				TSec=TempTSec;
			}
			if(MKeyNum==16)
			{
				MKeyNum=0;
				Data=AT24C02_ReadByte(5);
				THour=Data;
				TempTHour=THour;
				Data=AT24C02_ReadByte(6);
				TMin=Data;
				TempTMin=TMin;
				Data=AT24C02_ReadByte(7);
				TSec=Data;
				TempTSec=TSec;
			}
			Nixie(1,TempTHour/10);
			Nixie(2,TempTHour%10);
			Nixie(3,11);
			Nixie(4,TempTMin/10);
			Nixie(5,TempTMin%10);
			Nixie(6,11);
			Nixie(7,TempTSec/10);
			Nixie(8,TempTSec%10);			
		}
}

void JSQXS()
{
		while(Show_Timer)
		{
			if(MKeyNum==6)
			{
				MKeyNum=0;
				Show_Timer=0;
			}
			if(MKeyNum==1)//开始/暂停
			{
				MKeyNum=0;
				P_Buzzer=0;
				State_Timer++;
				Timer_Control++;			
				if(State_Timer==2)
				{
					State_Timer=0;
					Timer_Control=0;
					P2&=0x1F;
					P2|=0xA0;
					P0=0x00;//P0^6记好了
					P2&=0x1F;
				}			
			}
		
			if(MKeyNum==2)
			{
				MKeyNum=0;
				THour=AT24C02_ReadByte(5);
				TMin=AT24C02_ReadByte(6);
				TSec=AT24C02_ReadByte(7);
				TMiniSec=0;
				Timer_Control=0;
				P2&=0x1F;
				P2|=0xA0;
				P0=0x00;//P0^6记好了
				P2&=0x1F;
			}
			
			if(MKeyNum==3)
			{
				MKeyNum=0;
				State_Timer=0;
				P_Buzzer=1;
				THour=AT24C02_ReadByte(5);
				TMin=AT24C02_ReadByte(6);
				TSec=AT24C02_ReadByte(7);
				TMiniSec=0;
				P2&=0x1F;
				P2|=0xA0;
				P0=0x00;//P0^6记好了
				P2&=0x1F;
			}
			Nixie(1,THour/10);
			Nixie(2,THour%10);
			Nixie(3,11);
			Nixie(4,TMin/10);
			Nixie(5,TMin%10);
			Nixie(6,11);
			Nixie(7,TSec/10);
			Nixie(8,TSec%10);	
		}
}

void main()
{
	Timer0_Init();
	LED_Close();
	Buzz_Close();
	DS1302_Init();
	DS1302_SetTime();
	NHour=AT24C02_ReadByte(3);
	NMin=AT24C02_ReadByte(4);
	THour=AT24C02_ReadByte(5);
	TMin=AT24C02_ReadByte(6);
	TSec=AT24C02_ReadByte(7);
	TempNHour=NHour;
	TempNMin=NMin;
	while(1)
	{
		Nixie(1,10);
		Nixie(2,10);
		Nixie(3,10);
		Nixie(4,10);
		Nixie(5,10);
		Nixie(6,10);
		Nixie(7,10);
		Nixie(8,10);
		GongNeng_Select();
		//秒表
		MB();
		//时钟
		SZ();
		//调闹钟
		TNZ();
		//开关闹钟
		SNZ();
		//设置计时器
		SZJSQ();
		//计时器显示
		JSQXS();
	}
}	
