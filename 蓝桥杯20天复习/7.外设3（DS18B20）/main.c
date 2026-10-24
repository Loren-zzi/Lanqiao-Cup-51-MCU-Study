// 本例主题：DS18B20 温度读取。

#include <STC15F2K60S2.H>
#include "onewire.h"
#include "intrins.h"
/*     温度        */

code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff};  //0~9 0.~9.  全灭20
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={20,20,20,20,20,20,20,20};

//==============================
// 等待 DS18B20 完成首次温度转换；这里是阻塞式延时。
void Delay1000ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 46;
	j = 153;
	k = 245;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


//=====================================
// 初始化 LED、蜂鸣器和继电器，让上电状态明确。
void Init()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
}

// 动态扫描数码管：先消影，再选当前位，最后送出 Temp 中的段码。
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
	P0=seg[Temp[Bit]];
	P2&=0x1f;
	if(++Bit==8)
	Bit=0;
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

// 定时器 0 中断：刷新数码管，并按计数值安排较慢的任务。
void Timer0()    interrupt 1
{
	Nixie();
}





//==============================读取DS温度
// DS18B20 先转换温度，再读取低、高两个字节；原始值除以 16 得到摄氏度。
void DS_readbyte()
{
	unsigned char low,high;					//温度低字节 温度高字节
	int Temper;												//合温度
	float T;													//实际温度
	init_ds18b20();
	Write_DS18B20(0xcc);             //跳过ROM指令
	Write_DS18B20(0x44);							//开始温度转化
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);							//读取温度暂存器
	low=Read_DS18B20();							
	high=Read_DS18B20();
	Temper=(high<<8)|low;										
	T=Temper/16.0;										//转化为浮点温度
	
	

	Temp[2]=(unsigned char)(T)/10;
	Temp[3]=(unsigned char)(T)%10+10;
	Temp[4]=(unsigned long)(T*10000)%10000/1000;
	Temp[5]=(unsigned long)(T*10000)%10000/100%10;
	Temp[6]=(unsigned long)(T*10000)%10000/10%10;
	Temp[7]=(unsigned long)(T*10000)%10000%10;

}


//===================================

// 启动数码管扫描并完成首次转换，此后不断读取温度。
void main()
{
	Init();	
	Timer0Init();	
	DS_readbyte();
	Delay1000ms();
	while(1)
	{
		DS_readbyte();
	}

}