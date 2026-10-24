#include <REGX52.H>
#include "Timer0.h"
#include "Key.h"
#include "Nixie.h"
#include "Delay.h"
#include "AT24C02.h"

unsigned char KeyNum;
unsigned char Hour,Min,Sec;
unsigned char RunFlag;

void main()
{ 
	Timer0_Init(); // 初始化定时器
    Hour =15;       
    Min= 30;    
    Sec = 0; 
	
	while(1)
	{
		KeyNum=Key();
		if(KeyNum==1)			//K1按键按下
		{
			RunFlag=!RunFlag;	//启动标志位翻转
		}
		
		Nixie_SetBuf(1,Hour/10);	//设置显示缓存，显示数据
		Nixie_SetBuf(2,Hour%10);
		Nixie_SetBuf(3,11);
		Nixie_SetBuf(4,Min/10);
		Nixie_SetBuf(5,Min%10);
		Nixie_SetBuf(6,11);
		Nixie_SetBuf(7,Sec/10);
		Nixie_SetBuf(8,Sec%10);
	}


void Sec_Loop(void)
{
	if(RunFlag)
	{
		Sec++;
		if(Sec>=60)
		{
			Sec=0;
			Min++;
			if(Min>=60)
			{
				Min=0;
				Hour++;
				if(Hour>=24)
				{
					Hour=0;
				}
			}
		}
	}
}


//void Timer0_Routine() interrupt 1
//{
//	static unsigned int T0Count1,T0Count2,T0Count3;
//	TL0 = 0x18;		//设置定时初值
//	TH0 = 0xFC;		//设置定时初值
//	T0Count1++;
//	if(T0Count1>=20)
//	{
//		T0Count1=0;
//		Key_Loop();	//20ms调用一次按键驱动函数
//	}
//	T0Count2++;
//	if(T0Count2>=2)
//	{
//		T0Count2=0;
//		Nixie_Loop();//2ms调用一次数码管驱动函数
//	}
//	T0Count3++;
//	if(T0Count3>=10)
//	{
//		T0Count3=0;
//		Sec_Loop();	//10ms调用一次数秒表驱动函数
//	}
//}
//}