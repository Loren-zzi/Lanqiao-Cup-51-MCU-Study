#include <STC15F2K60S2.H>

void Timer0Init()		//@11.0592MHZ  1ms中断一次
{
	TMOD=0x01;							//设置定时器0的工作模式为16位定时器
	TH0=(65536-917)/256;		//装初值
	TL0=(65536-917)%256;		
	EA=1;										//开总中断
	ET0=1;									//允许定时器0申请中断
	TR0=1;									//启动定时器0
}

/*
void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count=0;
	TH0=(65536-917)/256;		//重装初值
	TL0=(65536-917)%256;		
	T0Count++;
	if(T0Count>=1000)
	{
		T0Count=0;
	}
}
*/