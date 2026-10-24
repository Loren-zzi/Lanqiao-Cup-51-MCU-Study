// 本例主题：集中复习时钟、温度和 I2C 模板。

#include <STC15F2K60S2.h>
#include "DS1302.h"
#include "IIC.h"
#include "onewire.h"
#include "intrins.h"
code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xbf};
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={20,20,20,20,20,20,20,20};
float T;
bit adc_start;
//===================================
// 给温度传感器留出首次转换时间；这里是阻塞式延时。
void Delay800ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 37;
	j = 123;
	k = 92;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


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

//========================NIXIE
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

//=======================定时器
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


// 定时器中断持续刷新数码管，并给 ADC 设置周期采样标志。
void Timer()    interrupt 1
{
	static unsigned int count0;
	Nixie();
	if(++count0==100)
	{
		count0=0;
		adc_start=1;
	}
}

//===========================DS1302
unsigned char Time[]={0x25,0x04,0x05,0x11,0x42,0x06,0x06};

unsigned char Time_Wcommand[]={0x8c,0x88,0x86,0x84,0x82,0x80,0x8a};
unsigned char Time_Rcommand[]={0x8d,0x89,0x87,0x85,0x83,0x81,0x8b};
#define WP 0x8e

// 关闭 DS1302 写保护，按地址写入初始时间，再重新开启写保护。
void set_time()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<=6;i++)
	{
		Write_Ds1302_Byte(Time_Wcommand[i],Time[i]);
	}
		Write_Ds1302_Byte(WP,0x80);
}

// 读取 DS1302 的时间寄存器，供显示函数使用。
void read_time()
{
	unsigned char i;
	for(i=0;i<=6;i++)
	{
		Time[i]=Read_Ds1302_Byte(Time_Rcommand[i]);
	}
}

// 把 DS1302 的 BCD 时间拆成十位和个位，写入 Temp。
void show_time()
{
	Temp[0]=Time[5]/16;
	Temp[1]=Time[5]%16;
	
	Temp[2]=11;
}

//==========================温度
// 先发转换命令，再读取 DS18B20 的两个温度字节，合成实际温度。
void read_temper()
{
	unsigned char low,high;
//	float T;
	int Temper;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);

		
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);

	
	low=Read_DS18B20();
	high=Read_DS18B20();
	
	Temper=high<<8|low;
	T=Temper/16.0;
}

// 把温度拆成各位数字，写入 Temp 显示缓冲区。
void show_temper()
{
	Temp[0]=(int)(T)/10;
	Temp[1]=(int)(T)%10+10;
	Temp[2]=(int)(T*100)/10%10;
	Temp[3]=(int)(T*1000)/10%10;
	Temp[4]=20;
	Temp[5]=20;
	Temp[6]=20;
	Temp[7]=20;
}

//===============================PCF8591     200ms
unsigned int dacdata1,dacdata2;//输入输出

// 通过 I2C 向 PCF8591 写入一个 DAC 输出值。
void dac_write(unsigned char Data)
{
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x40);
	I2CWaitAck();
	I2CSendByte(Data);
	I2CWaitAck();
	I2CStop();
}

// 通过 I2C 选择 PCF8591 的 ADC 通道并读取转换结果。
unsigned char ad_read()
{
	unsigned char Data;
	I2CStart();
	I2CSendByte(0x90);
	I2CWaitAck();
	I2CSendByte(0x03);
	I2CWaitAck();

	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	Data=I2CReceiveByte();
	I2CSendByte(1);	
	I2CStop();

	return Data;
}

// 把 ADC 换算后的数值写入 Temp。
void show_ad()
{
	Temp[0]=dacdata2/100+10;
	Temp[1]=dacdata2/10%10;
	Temp[2]=dacdata2%10;
	Temp[3]=20;
	Temp[4]=20;
	Temp[5]=20;
	Temp[6]=20;
	Temp[7]=20;
}


//=====================================
// 把时钟、温度、ADC 和 DAC 的常用写法集中练习；主循环中的调用目前被注释。
void main()
{
	init();
	Timer0Init();
	set_time();
	read_temper();
	Delay800ms();
	while(1)
	{
//		read_time();      //时间
//		show_time();
		
//		read_temper();      //温度
//		show_temper();
		
//		if(adc_start)					//adc
//		{
//			adc_start=0;
//			dacdata1=ad_read();
//			dacdata2=dacdata1/255.0*500;
//			show_ad();
//		}
	}

}