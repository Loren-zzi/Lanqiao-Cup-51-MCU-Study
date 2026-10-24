// 本例主题：用定时器测量 NE555 脉冲。
// 定时器 0 计外部脉冲，定时器 1 提供扫描节拍。

#include <STC15F2K60S2.h>

code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={10,10,10,10,10,10,10,10};

unsigned int FData;

//======================选择译码器
// 选择 74HC138 译码器通道；操作结束选 0，避免 P0 的后续写入影响外设。
void select(unsigned char n)
{
	switch(n)
	{
		case 0:P2=((P2&0x1f)|0x00);break;
		case 4:P2=((P2&0x1f)|0x80);break;
		case 5:P2=((P2&0x1f)|0xa0);break;
		case 6:P2=((P2&0x1f)|0xc0);break;
		case 7:P2=((P2&0x1f)|0xe0);break;
	}
}
//=====================初始化
// 初始化 LED、蜂鸣器和继电器，让上电状态明确。
void init()
{
	select(4);
	P0=0xff;
	select(0);
	
	select(5);
	P0=0x00;
	select(0);
}
//=========================
// 动态扫描数码管：先消影，再选当前位，最后送出 Temp 中的段码。
void Nixie()
{
	static unsigned char Bit;
	select(7);
	P0=0xff;
	select(0);
	
	select(6);
	P0=0x01<<Bit;
	select(0);
	
	select(7);
	P0=seg[Temp[Bit]];
	select(0);
	
	if(++Bit==8)
	{
		Bit=0;
	}
}
//========================                  NE555
// 把定时器 0 设为外部计数器，用来累计 NE555 脉冲。
void Timer0Init(void)		// 外部脉冲计数 @ 12 MHz
{
	AUXR &= 0x7F;		// 定时器 0 使用 12T 模式
	TMOD &= 0xF0;		// 清除定时器 0 模式位
	TMOD|=0x05;		// 定时器 0 设为外部计数模式
	TL0 = 0x00;		// 外部计数初值清零
	TH0 = 0x00;		// 外部计数初值清零
	TF0 = 0;		// 清除定时器 0 溢出标志
	TR0 = 1;		// 启动定时器 0
}

// 定时器 1 每 2 ms 中断一次，作为 1 秒计数窗口的节拍。
void Timer1Init(void)		// 2 ms @ 12 MHz
{
	AUXR |= 0x40;		// 定时器 1 使用 1T 模式
	TMOD &= 0x0F;		// 清除定时器 1 模式位
	TL1 = 0x40;		// 2 ms 定时初值的低字节
	TH1 = 0xA2;		// 2 ms 定时初值的高字节
	TF1 = 0;		// 清除定时器 1 溢出标志
	TR1 = 1;		// 启动定时器 1
	ET1=1;
	EA=1;
}

// 累计 500 次中断约为 1 秒；读取并清零脉冲计数。注意原代码的 count0 未初始化。
void Timer()  interrupt 3
{
	unsigned int count0;
	Nixie();
	if(++count0==500)
	{
		count0=0;
		FData=TH0<<8|TL0;
		TH0=0;
		TL0=0;
	}
}

// 把 NE555 的计数结果拆成各位数字，用数码管显示。
void show_NE555()
{
	Temp[0]=FData/10000;
	Temp[1]=FData%10000/1000;
	Temp[2]=FData%1000/100;
	Temp[3]=FData%100/10;
	Temp[4]=FData%10;
}

//===========================
// 启动计数器与定时器，在主循环中显示测得的脉冲数。
void main()
{
	init();
	Timer0Init();
	Timer1Init();
	while(1)
	{
		show_NE555();
	}

}