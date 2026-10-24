// 本例主题：复习多外设模板及页面切换。
// 定时器 0 计外部脉冲，定时器 1 提供扫描节拍。

#include <STC15F2K60S2.h>
#include "DS1302.h"
#include "IIC.h"
#include "ONEWIRE.h"
#include "intrins.h"
//=========================================
unsigned char show_mode;


//===============================================
sbit list3=P3^5;
sbit line1=P3^3;

bit dac_start;

code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xbf};
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={20,20,20,20,20,20,20,20};

unsigned char TimeWcommand[]={0x82,0x80};
unsigned char TimeRcommand[]={0x83,0x81};
#define WP   0x8e
unsigned char Time[]={0x59,0x57};

unsigned char mkey;
unsigned int Fdata;
float T;
unsigned int dacdata1,dacdata2;
//===========================


//====================
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


//=============================select
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

//===============================inint
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
//============================Nixie
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
	
	if(++Bit==8)
	{
		Bit=0;
	}
}
//======================mkey
// 扫描键盘行列，返回当前按下的键号；没有按键应返回 0。
unsigned char mkey_state()
{
	unsigned char mkey_state=0;
	list3=0;
	P3|=0x0f;
	if(line1==0)mkey_state=12;
	
	return mkey_state;
}

// 比较前后两次扫描结果，在松手时生成一次按键事件。
void mkey_loop()
{
	static unsigned char last,now;
	last=now;
	now=mkey_state();
	if(last==12&&now==0)mkey=12;
}

//===========================定时器
// 把定时器 0 设为外部计数器，累计 NE555 脉冲。
void Timer0Init(void)		// 外部脉冲计数 @ 12 MHz
{
	AUXR |= 0x7F;		// 原代码置低 7 位；此句不能确定定时器 0 的 1T/12T 模式
	TMOD &= 0xF0;		// 清除定时器 0 模式位
	TMOD|=0x05;		// 定时器 0 设为外部计数模式
	TL0 = 0x00;		// 外部计数初值清零
	TH0 = 0x00;		// 外部计数初值清零
	TF0 = 0;		// 清除定时器 0 溢出标志
	TR0 = 1;		// 启动定时器 0
}

// 定时器 1 提供 2 ms 的数码管与按键扫描节拍。
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

// 在中断中刷新数码管、扫描按键，并按计数窗口读取频率。
void Timer()  interrupt 3
{
	static unsigned int count0,count1,count2;
	Nixie();
	if(++count0==10)
	{
		count0=0;
		mkey_loop();
	}
	if(++count1==500)
	{
		count1=0;
		Fdata=TH0<<8|TL0;
		TH0=0;
		TL0=0;
	}
	if(++count2==100)
	{
		count2=0;
		dac_start=1;
	}
}
//======================Ne555  show_mode0
// 把 NE555 的计数结果拆成各位数字，用数码管显示。
void show_ne555()
{
	Temp[0]=Fdata/10000;
	Temp[1]=Fdata%10000/1000;
	Temp[2]=Fdata%1000/100;
	Temp[3]=Fdata%100/10;
	Temp[4]=Fdata%10;
}
//=========================DS18B20  show_mode1
// 先发转换命令，再读取 DS18B20 的两个温度字节，合成实际温度。
void read_temper()
{
	unsigned char low,high;
	int temper;
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	
	low=Read_DS18B20();
	high=Read_DS18B20();
	
	temper=high<<8|low;
	
	T=temper/16.0;
}

// 把温度拆成各位数字，写入 Temp 显示缓冲区。
void show_temper()
{	
	
	Temp[0]=(int)(T)/10;
	Temp[1]=(int)(T)%10+10;
	Temp[2]=(int)(T*10)%10;
}

//========================DS1302    show_mode2
// 关闭 DS1302 写保护，按地址写入初始时间，再重新开启写保护。
void set_time()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<=1;i++)
	{
		Write_Ds1302_Byte(TimeWcommand[i],Time[i]);
	}
	Write_Ds1302_Byte(WP,0x80);
}

// 读取 DS1302 的时间寄存器，供显示函数使用。
void read_time()
{
	unsigned char i;
	for(i=0;i<=1;i++)
	{
		Time[i]=Read_Ds1302_Byte(TimeRcommand[i]);
	}
}
// 把 DS1302 的 BCD 时间拆成十位和个位，写入 Temp。
void show_time()
{
	Temp[0]=Time[0]/16;
	Temp[1]=Time[0]%16;
	
	Temp[2]=21;
	
	Temp[3]=Time[1]/16;
	Temp[4]=Time[1]%16;
}

//===========================PCF8591   (856)   show_mode3
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
}
//============================show_work
// 按 show_mode 切换频率、温度、时钟和 ADC 显示。
void show_work()
{
	if(mkey==12)
	{
		mkey=0;
		if(++show_mode==4)
		{
			show_mode=0;
		}
	}
	switch(show_mode)
	{
		case 0: show_ne555();break;
		case 1:show_temper();break;
		case 2:show_time();break;
		case 3:
		{
			if(dac_start)
		{
			dac_start=0;
			dacdata1=ad_read();
			dacdata2=dacdata1/255.0*500;
			show_ad();
		}
		}break;
	}
}


//=========================
// 初始化各外设；主循环读取温度与时间，再更新当前页面。
void main()
{
	init();
	Timer0Init();
	Timer1Init();
	read_temper();
	Delay800ms();
	set_time();
	while(1)
	{
		read_temper();
		read_time();
		show_work();
//		show_ne555();
	}
}