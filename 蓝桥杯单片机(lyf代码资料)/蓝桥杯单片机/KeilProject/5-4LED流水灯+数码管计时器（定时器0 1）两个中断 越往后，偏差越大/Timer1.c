#include <STC15F2K60S2.H>

void Timer1Init()		//@11.0592MHZ  1ms中断一次
{
	TMOD=0x10;							//设置定时器1的工作模式为16位定时器
	TH1=(65536-917)/256;		//装初值
	TL1=(65536-917)%256;		
	EA=1;										//开总中断
	ET1=1;									//允许定时器1申请中断						
	TR1=1;									////启动定时器0
}

/*
void Timer1_Routine() interrupt 3
{
	static unsigned int T1Count=0;
	TH1=(65536-917)/256;		//重装初值
	TL1=(65536-917)%256;		
	T1Count++;
	if(T1Count>=1000)
	{
		T1Count=0;
	}
}
*/