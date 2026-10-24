// 本例主题：底层知识点测试草稿。

#include <STC15F2K60S2.h>


sbit list3=P3^5;
sbit list4=P3^4;
sbit line1=P3^3;
sbit line2=P3^2;


code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xbf,0xc1};//22 U
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={20,20,20,20,20,20,20,20};
unsigned char mkey;

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

//===========================Nixie
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
		Bit=0;
		
}
//=============================mkey
// 扫描键盘行列，返回当前按下的键号；没有按键应返回 0。
unsigned char mkey_state()
{
	unsigned char mkey_state=0;
	list3=0;
	list4=1;
	P3|=0x0f;
	if(line1==0)mkey_state=12;
	if(line2==0)mkey_state=13;
	
	list3=1;
	list4=0;
	P3|=0x0f;
	if(line1==0)mkey_state=16;
	if(line2==0)mkey_state=17;
	
	return mkey_state;
}

// 比较前后两次扫描结果，在松手时生成一次按键事件。
void mkey_loop()
{
	static unsigned char last,now;
	last=now;
	now=mkey_state();
	
	if(last==12&&now==0)mkey=12;
	if(last==13&&now==0)mkey=13;
	if(last==16&&now==0)mkey=16;
	
	if(mode==1)                       //模式一的时候进行特殊化处理
	{
		if(now==17)
		{
			mkey=17;
		}
		else if(last==17&&now==0)
		{
			mkey=0;
		}
	}
	else 
		if(last==17&&now==0)mkey=17;
}

// 配置定时器 0 的 2 ms 中断，用来提供数码管扫描节拍。
void Timer0Init(void)		// 2 ms @ 12 MHz
{
		AUXR |= 0x80;		// 定时器 0 使用 1T 模式
		TMOD &= 0xF0;		// 定时器 0 设为 16 位定时模式
		TL0 = 0x40;		// 2 ms 定时初值的低字节
		TH0 = 0xA2;		// 2 ms 定时初值的高字节
		TF0 = 0;		// 清除定时器 0 溢出标志
		TR0 = 1;		// 启动定时器 0
		ET0=1;
		EA=1;
}

// 原意是周期刷新显示并扫描按键；此处 Nixie 少了调用括号。
void Timer()     interrupt 1
{
	static unsigned int count0;
	Nixie;
	if(++count0==10)
	{
		count0=0;
		mkey_loop();
	}
}



//=====================
// 当前是三个知识点的测试草稿；原代码没有调用 Timer0Init()。
void main()
{
	init();
	while(1)
	{
	
	}
}