#include <STC15F2K60S2.H>

bit LED_1_State=1,LED_1_End,LED_8_State=1,LED_8_End;

void LED_1()
{
	P2=((P2&0x1f)|0x80);
	P0&=0xfe;
	P2=0x1f;
}

void CLED_1()
{
	P2=((P2&0x1f)|0x80);
	P0|=0x7f;
	P2=0x1f;
}
void LED_8()
{
	P2=((P2&0x1f)|0x80);
	P0&=0x7f;
	P2=0x1f;
}

void CLED_8()
{
	P2=((P2&0x1f)|0x80);
	P0|=0xfe;
	P2=0x1f;
}

void Timer0_Loop() interrupt 1
{
	static unsigned int Count0,Count1;
	static unsigned char i,m;
	if(++Count0>=500)
	{
		Count0=0;
		i++;
		if(i==1)
		{
			LED_1_State=0;
			LED_1_End=1;
		}
		if(i==2)
		{
			LED_1_State=1;
			LED_1_End=0;
			i=0;
		}
	}
	if(++Count1>=5000)
	{
		Count1=0;
		m++;
		if(m==1)
		{
			LED_8_State=0;
			LED_8_End=1;
		}
		if(m==2)
		{
			LED_8_State=1;
			LED_8_End=0;
			m=0;
		}
	}
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


void main()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2=0x1f;
	Timer0Init();
	while(1)
	{
		if(LED_1_State)
		LED_1();
		if(LED_1_End)
		CLED_1();
		if(LED_8_State)
		LED_8();
		if(LED_8_End)
		CLED_8();
	}
}