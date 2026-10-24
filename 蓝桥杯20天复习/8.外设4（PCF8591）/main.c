// 本例主题：PCF8591 模数转换。

#include <STC15F2K60S2.h>
#include "iic.h"

code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff};  //0~9 0.~9.  全灭20
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={20,20,20,20,20,20,20,20};
unsigned char ADC_data;
unsigned int Udata;

bit ADC_star;       //ADC标志位

//=============================
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

//==========================定时器0 刷新Nixie     200ms刷新ADC标志位
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

// 每 2 ms 刷新数码管；每 100 次置位标志，让主循环约 200 ms 读一次 ADC。
void Timer0()     interrupt 1
{
	static unsigned char count1;
	Nixie();
	if(++count1==100)
	{
		count1=0;
		ADC_star=1;
	}
}

//==========================ADC 写配置0x90    写通道 读取数据
// 先写控制字选择 ADC 通道，再从 PCF8591 的读地址取得转换值。
unsigned char ADC_read()
{
	unsigned char Data;
	I2CStart();
	I2CSendByte(0x90);      //PCF8951写地址
	I2CWaitAck();
	I2CSendByte(0x03);			//通道三电压  通道一光敏
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0x91);			//ADC读地址
	I2CWaitAck();
	Data=I2CReceiveByte();
	I2CSendByte(1);					//读取完成
  I2CStop();
	return Data;

}

//void DAC_Write(unsigned char Data)  
//{
//	I2CStart();
//	I2CSendByte(0x90);
//	I2CWaitAck();
//	I2CSendByte(0x40);
//	I2CWaitAck();
//	I2CSendByte(Data);
//	I2CWaitAck();
//	I2CStop();
//}


// 主循环等采样标志，再把 0~255 的 ADC 值换算为本例的电压显示值。
void main()
{
	Init();
	Timer0Init();
	while(1)
	{
		if(ADC_star)
		{
			ADC_star=0;
			ADC_data=ADC_read();         
			Udata=ADC_data/255.0*250;     //将原信号0~255先转为0~1 然后转为想要的参数，这里参数是2.5V    
			Temp[0]=Udata/100+10;
			Temp[1]=(Udata/10)%10;
			Temp[2]=Udata%10;
		}
	}
}