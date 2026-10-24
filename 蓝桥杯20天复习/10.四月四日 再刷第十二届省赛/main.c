// 本例主题：再次练习第十二届省赛题。

#include <STC15F2K60S2.h>
#include "onewire.h"
#include "iic.h"
#include "intrins.h"

sbit list1=P4^4;
sbit list2=P4^2;
sbit line1=P3^3;
sbit line2=P3^2;


bit ds_start,dac_start;
unsigned char mode,dac_mode;
unsigned int dacdata1,dacdata2;
code  // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xc6,0x8c,0x88};  //0~9 0.~9.  全灭20 C是21 P是22 A是23
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={20,20,20,20,20,20,20,20};
unsigned char mkey;
float T;
unsigned char a=25;//参数
unsigned char led_state;



//===========================
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


//=================================选择译码器
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
//================================初始化
// 初始化 LED、蜂鸣器和继电器，让上电状态明确。
void init()
{
	select(5);           //继电器
	P0=0x00;
	select(0);
	
	select(4);					 //LED
	P0=0xff;
	select(0);
}

//===============================数码管
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

//================================键盘
// 扫描键盘行列，返回当前按下的键号；没有按键应返回 0。
unsigned char mkey_state()
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
//================================定时器
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

void led();

// 定时器中断：刷新数码管，并按计数值安排较慢的任务。
void Timer()      interrupt 1
{
	static unsigned int count0,count1,count2;
	Nixie();led();
	if(++count0==10)
	{
		count0=0;
		mkey_loop();
	}
	if(++count1==100)
	{
		count1=0;
		ds_start=1;
	}
	if(++count2==50)
	{
		count2=0;
		dac_start=1;
	}
	
}

//==============================温度
// 读取 DS18B20 的温度数据。
void ds_read()
{
	unsigned char low,high;
	int temper;

	init_ds18b20();
	Write_DS18B20(0xcc);			//跳过ROM
	Write_DS18B20(0x44);			//开始温度转化

	init_ds18b20();
	Write_DS18B20(0xcc);			//跳过ROM
	Write_DS18B20(0xbe);			//读取温度暂存器
	
	low=Read_DS18B20();
	high=Read_DS18B20();
	temper=(high<<8)|low;
	T=temper/16.0;
}

// 将温度写到数码管显示缓冲区。
void show_ds()
{
	Temp[0]=21;
	Temp[4]=(int)(T)/10;
	Temp[5]=(int)(T)%10+10;
	Temp[6]=(int)(T*100)%1000%100/10;
	Temp[7]=(int)(T*100)%10;

}
//===================参数
// 根据按键调整参数。
void canshu()
{
	if(mkey==9)
	{
		mkey=0;
		a++;
	}
	if(mkey==8)
	{
		mkey=0;
		a--;
	}	
}

// 把当前参数拆成数字，写入 Temp。
void show_canshu()
{
	Temp[0]=22;
	Temp[4]=20;
	Temp[5]=20;
	Temp[6]=a/10;
	Temp[7]=a%10;
	
}
//=================================dac
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

// 根据当前测量值和模式，计算并更新 DAC 输出。
void dac_work()
{
	if(mkey==5)
	{
		mkey=0;
		dac_mode=(dac_mode+1)%2;
		
	}
	switch(dac_mode)
	{
		case 0:
		{
			if(T<a)
			{
				dacdata1=dacdata2=0;
				dac_write(dacdata1);
			}
			else
			{
				dacdata1=255;
				dacdata2=500;
				dac_write(dacdata1);
			}break;
		}
		
		case 1:
		{
			if(T<20)
			{
				dacdata2=100;
				dac_write(dacdata1=(dacdata2/500.0*255));
			}
			else if(T>40)
			{
				dacdata2=400;
				dac_write(dacdata1=(dacdata2/500.0*255));
			}
			else
			{
				dacdata2=(15*T-200);
				dac_write(dacdata1=(dacdata2/500.0*255));
			}
		}break;
	}
}

// 把 DAC 相关数值写入数码管显示缓冲区。
void show_dac()
{
	Temp[0]=23;
	Temp[4]=20;

	Temp[5]=dacdata2/100+10;
	Temp[6]=dacdata2/10%10;
	Temp[7]=dacdata2%10;
}
//=============================LED
// 根据当前工作状态更新 LED。
void led()
{
	if(mode==0)led_state=0xfd;
	else if(mode==1)led_state=0xfb;
	else led_state=0xf7;
	if(dac_mode==0)led_state&=0xfe;
	else led_state|=0x01;
	select(4);
	P0=led_state;
	select(0);
}



//=======================working
// 按 mode 分配温度、参数和 DAC 页面。
void work()
{
	if(mkey==4)
	{
		mkey=0;
		if(++mode==3)
		{
			mode=0;
		}
	}
	if(mode==0)
	{
		show_ds();
	}
	else if(mode==1)
	{
		canshu();
		show_canshu();
	}
	else
	{
		show_dac();
	}
}


//===============================
// 第十二届题目的再次练习；当前主循环保留了部分调试代码。
void main()
{
	init();
	Timer0Init();
	ds_read();
	Delay800ms();
	while(1)
	{
//		if(ds_start)
//		{
//			ds_start=0;
//			ds_read();
//		}
//		if(dac_start)
//		{
//			dac_start=0;
//			dac_work();
//		}
//		work();


//	}
		


	if(mkey==17)
	{
		mkey=0;
		a++;
	}
	if(mkey==16)
	{
		mkey=0;
		a--;
	}	
	Temp[0]=a/10;
	Temp[1]=a%10;
}

}