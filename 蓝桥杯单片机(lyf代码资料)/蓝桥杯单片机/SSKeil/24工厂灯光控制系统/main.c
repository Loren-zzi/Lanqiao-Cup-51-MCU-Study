#include <STC15F2K60S2.H>
#include "intrins.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff,0x00};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

sbit Key_1=P3^3;
sbit Key_2=P3^2;
sbit Key_3=P3^1;
sbit Key_4=P3^0;

bit ShiJian,L8,L7;

unsigned char KeyNum,Hour,Min,Sec,Command,LedState=0xff;

void Delay100ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 144;
	k = 71;
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
	unsigned char i;
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2&=0x1f;
	for(i=0;i<9;i++)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xfe<<i;
		P2&=0x1f;
		Delay100ms();
	}
	for(i=0;i<9;i++)
	{
		P2=((P2&0x1f)|0x80);
		P0=~(0xfe<<i);
		P2&=0x1f;
		Delay100ms();
	}
	for(i=0;i<8;i++)
	{
		Temp[i]=12;
		Delay100ms();
	}
	for(i=0;i<8;i++)
	{
		Temp[i]=11;
		Delay100ms();
	}
	Temp[0]=0;
	Temp[1]=0;
	Temp[2]=10;
	Temp[3]=0;
	Temp[4]=0;
	Temp[5]=10;
	Temp[6]=0;
	Temp[7]=0;
	ShiJian=1;
}

unsigned char Key_State()
{
	unsigned char Key=0;
	if(Key_1==0)
	Key=1;
	if(Key_2==0)
	Key=2;
	if(Key_3==0)
	Key=3;
	if(Key_4==0)
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

void ShiJian_Loop()
{
	Sec++;
	if(Sec==60)
	{
		Sec=0;
		Min++;
		if(Min==60)
		{
			Min=0;
			Hour++;
			if(Hour==24)
			Hour=0;
		}
	}
	Temp[0]=Hour/10;
	Temp[1]=Hour%10;
	Temp[3]=Min/10;
	Temp[4]=Min%10;
	Temp[6]=Sec/10;
	Temp[7]=Sec%10;
}

void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
}

void UartInit(void)		//9600bps@11.0592MHz
{
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xFA;		//设定定时初值
	TH1 = 0xFA;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	ES=1;
	EA=1;
}

void UART_SendByte(unsigned char Byte)
{
	SBUF=Byte;
	while(TI==0);
	TI=0;
}

void UART_ReadByte() interrupt 4
{
	if(RI==1)
	Command=SBUF;
	RI=0;
}

void Timer0_loop() interrupt 1
{
	static unsigned int Count0,Count1,Count2;
	if(++Count0==2)
	{
		Count0=0;
		Nixie();
	}
	if(++Count1==20)
	{
		Count1=0;
		Key_Loop();
	}
	if(ShiJian)
	{
		if(++Count2==1000)
		{
			Count2=0;
			ShiJian_Loop();
		}
	}
}

void UART_Command()//为了保证在扫描数码管时卡在中间导致瞬闪，不要把以下程序写在串口中断（中断优先级高）。
{
	switch(Command&0xf0)
	{
		case 0xA0:
		{
			P2=((P2&0x1f)|0x80);
			P0=(LedState|0x0f)&(~Command|0xf0);
			LedState=P0;
			P2&=0x1f;
			Command=0;
		}break;
		case 0xB0:
		{
			if(Command==0xB0)
			{
				UART_SendByte((Hour/10<<4)|(Hour%10));
				UART_SendByte((Min/10<<4)|(Min%10));
				UART_SendByte((Sec/10<<4)|(Sec%10));
				Command=0;
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
			if(L8==0)
			{
				P2=((P2&0x1f)|0x80);
				P0=LedState&0x7f;
				LedState=P0;
				P2&=0x1f;
				L8=1;
			}
			else
			{
				P2=((P2&0x1f)|0x80);
				P0=LedState|0x80;
				LedState=P0;
				P2&=0x1f;
				L8=0;
			}
		}break;
		case 2:
		{
			KeyNum=0;
			if(L7==0)
			{
				P2=((P2&0x1f)|0x80);
				P0=LedState&0xbf;
				LedState=P0;
				P2&=0x1f;
				L7=1;
			}
			else
			{
				P2=((P2&0x1f)|0x80);
				P0=LedState|0x40;
				LedState=P0;
				P2&=0x1f;
				L7=0;
			}
		}break;
	}
}

void main()
{
	Timer0Init();
	Init();
	UartInit();
	while(1)
	{
		UART_Command();
		Key_Select();
	}
}