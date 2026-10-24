// 本例主题：第十二届省赛题综合练习。

#include <STC15F2K60S2.H>
#include "onewire.h"
#include "intrins.h"
#include "iic.h"
/*     第十二届        */
unsigned char  Mode=0;
code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xc6,0x8c,0x88};  //0~9 0.~9.  全灭20 C是21 P是22 A是23
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={20,20,20,20,20,20,20,20};
unsigned char led=0xff;
unsigned char mkey;
unsigned char canshu=25;
unsigned char DAC_Mode;
float T;
unsigned int DACdata2,DACdata1;//转化后DAC 转化前DAC
unsigned char DACstate=0;
//=============
sbit list1=P4^4;
sbit list2=P4^2;
sbit line1=P3^3;
sbit line2=P3^2;

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
	P0=led;
	P2&=0x1f;
}
//==============================译码器
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

//==============================矩阵键盘
// 扫描键盘行列，返回当前按下的键号；没有按键应返回 0。
unsigned char  mkey_state()
{
	unsigned char mkey_state=0;
	list1=0;
	list2=1;
	P3|=0x0f;
	if(line1==0)mkey_state=4;
	if(line2==0)mkey_state=5;
	
	list1=1;
	list2=0;
	P3|=0x0f;
	if(line1==0)mkey_state=8;
	if(line2==0)mkey_state=9;
	return mkey_state;
}

// 比较前后两次扫描结果，在松手时生成一次按键事件。
void mkey_loop()
{
	static unsigned char last,now;
	last=now;
	now=mkey_state();
	if(last==4&&now==0)mkey=4;
	if(last==5&&now==0)mkey=5;
	if(last==8&&now==0)mkey=8;
	if(last==9&&now==0)mkey=9;
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
	static unsigned char count1,count2;
	Nixie();
	if(++count1==10)
	{
		mkey_loop();
		count1=0;
	}
	if(++count2==100)
	{
		count2=0;
		DACstate=1;
	}
}



//==============================DS初始化
// 启动 DS18B20 首次温度转换，为后续读取做准备。
void DS_Init()   
{
	init_ds18b20();
	Write_DS18B20(0xcc);					//跳过ROM
	Write_DS18B20(0x44);					//开始温度转化
	Delay1000ms();								//初始化要有1000ms延迟
}

//==============================读取DS温度
// 读取温度并换算成摄氏度，再更新温度显示。
void DS_readbyte()
{
	unsigned char low,high;					//温度低字节 温度高字节
	int Temper;												//合温度
//	float T;													//实际温度
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
	
	
	Temp[0]=21;                           //C
	
	Temp[1]=20;
	Temp[2]=20;
	Temp[3]=20;
	
	Temp[4]=(unsigned char)(T)/10;
	Temp[5]=(unsigned char)(T)%10+10;
	Temp[6]=(unsigned long)(T*10000)%10000/1000;
	Temp[7]=(unsigned long)(T*10000)%10000/100%10;

}


//===================================参数界面
// 处理阈值参数的按键调整与显示。
void canshu_loop()
{

	if(mkey==8)
	{
		mkey=0;
		canshu++;
	}
	if(mkey==9)
	{
		mkey=0;
		canshu--;
	}
	
	Temp[0]=22;
	Temp[1]=20;
	Temp[2]=20;
	Temp[3]=20;
	Temp[4]=20;
	Temp[5]=20;
	Temp[6]=canshu/10;
	Temp[7]=canshu%10;
	
}
//=============================DAC输出界面
// 通过 I2C 向 PCF8591 写入一个 DAC 输出值。
void DAC_write(unsigned char Data)
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
// 根据当前测量值和模式，计算并更新 DAC 输出。
void DACwork()
{
	switch(DAC_Mode)
	{
		case 0:
		{
			if(T<canshu)
			{
				DACdata1=0;
				DAC_write(DACdata1);
				DACdata2=0;
			}
			
			else {DACdata1=255;DACdata2=500;DAC_write(DACdata1);}
			}break;
		case 1:
		{
			if(T>=40){DACdata2=4;DACdata1=DACdata2/500.0*255;DAC_write(DACdata1);}
			else if(T<=20){DACdata2=100;DACdata1=DACdata2/500.0*255;DAC_write(DACdata1);}
			else {DACdata2=15*T-200;DACdata1=DACdata2/500.0*255;DAC_write(DACdata1);}break;
		}
	}
}

// 显示当前 DAC 输出相关的数值。
void showDAC()
{
	Temp[0]=23;
	Temp[1]=20;
	Temp[2]=20;
	Temp[3]=20;
	Temp[4]=20;
	Temp[5]=DACdata2/100+10;
	Temp[6]=DACdata2/10%10;
	Temp[7]=DACdata2%10;
}
	



//==========================转换工作
// 根据 Mode 选择温度、参数或 DAC 页面。
void working()
{
	if(mkey==4)
	{
		mkey=0;
		if(++Mode==3)
		{
			Mode=0;
		}
	}
	if(Mode==0)
	{
		DS_readbyte();
//		select(4);
//		led=0xfd;
//		P0=led;
//		select(0);
	}
	if(Mode==1)
	{
		canshu_loop();
	}
	if(Mode==2)
	{
		showDAC();
	}
		
}

//=======================
// 初始化温度采集和定时器；循环处理 DAC 模式与当前页面。
void main()
{
	Init();	
	DS_Init();
	Timer0Init();	
	
	while(1)
	{
		if(DACstate)
		{
			DACstate=0;
			if(mkey==5)
			{
				mkey=0;
				if(++DAC_Mode==2)
				{
					DAC_Mode=0;
				}
			}
			DACwork();	
		}

		working();
	}

}