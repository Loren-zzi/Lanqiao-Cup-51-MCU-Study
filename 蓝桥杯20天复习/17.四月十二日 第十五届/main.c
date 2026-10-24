// 本例主题：第十五届省赛题综合练习。
// 定时器 0 计外部脉冲，定时器 1 提供扫描节拍。

#include <STC15F2K60S2.h>
#include "iic.h"
#include "ds1302.h"

//============
sbit list1=P4^4;
sbit list2=P4^2;
sbit line1=P3^3;
sbit line2=P3^2;

//  F  11     H 12   A 13    P 14   - 15   L 16
code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x8e,0x89,0x88,0x8c,0xbf,0xc7}; 
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={10,10,10,10,10,10,10,10};
unsigned char mkey;
unsigned int Fdata;
int F=0,PF=2000,a=0;              //a  标准jiaoyan参数
unsigned char mode, canshumode,backmode;
unsigned char Time[]={0x13,0x17,0x58};
unsigned char TimeWcommand[]={0x84,0x82,0x80};
unsigned char TimeRcommand[]={0x85,0x83,0x81};
#define   WP    0x8e
unsigned int HF,b,c,d;     //最大频率     时 分 秒
float dacdata1,dacdata2;
unsigned char led=0xff;

//==============================select
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

//==============================  init
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

//==================================Nixie
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

//================================mkey
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
//================================================定时器
// 把定时器 0 设为外部计数器，累计 NE555 脉冲。
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
// 定时器 1 提供 2 ms 中断，统一安排扫描和计数窗口。
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

// 中断中读取频率、记录峰值及发生时间，并更新 LED 提示。
void Timer()  interrupt 3
{
	static unsigned int count0,count1,count2,count3,count4;
	Nixie();
	F=Fdata+a;
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
	if(++count2==500)
	{
		count2=0;
		if(HF<Fdata+a)
		{
			HF=Fdata+a;
			b=Time[0];
			c=Time[1];
			d=Time[2];
		}
	}
	if(mode==0)
	{
		if(++count3==100)
		{
			count3=0;
			led^=0x01;
			select(4);
			P0=led;
			select(0);
		}
	}
	else
	{
		led|=0x01;
		select(4);
		P0=led;
		select(0);
	}
	if(F>PF)
	{
		if(++count4==100)
		{
			count4=0;
			led^=0x02;
			select(4);
			P0=led;
			select(0);
		}
	}
	else if(F<0)
	{
		led&=0xfd;
		select(4);
		P0=led;
		select(0);
	}
	else 
	{
		led|=0x02;
		select(4);
		P0=led;
		select(0);

	}
}
//============================show_ne555   mode0
// 显示当前频率；前导零留空，负值走单独的显示分支。
void show_ne555()
{
	Temp[0]=11;
	Temp[1]=10;
	Temp[2]=10;
	if(F<0)
	{
		Temp[0]=11;
		Temp[1]=10;
		Temp[2]=10;
		Temp[3]=10;
		Temp[4]=10;
		Temp[5]=10;
		Temp[7]=16;
		Temp[5]=16;
	}
	else 
	{
		unsigned char pos=3;
		unsigned int chushu=10000;
		unsigned int num=F;
		
		while(chushu>1&&num/chushu==0)
		{
			Temp[pos++]=10;
			chushu/=10;
		}
		
		while(chushu>=1)
		{
			Temp[pos++]=num/chushu%10;
			chushu/=10;
		}
		if(num==0)
		{
			Temp[7]=0;
		}
	}
		
}

//============================canshu    mode1
// 按键选择并调整频率上限 PF 或校正参数 a。
void canshu_loop()
{
	if(mkey==5)
	{
		mkey=0;
		if(++canshumode==2)
		{
			canshumode=0;
		}
	}
	switch(canshumode)
	{
		case 0:
		{
			if(mkey==8)
			{
				mkey=0;
				PF+=1000;
				if(PF>9000)PF=9000;
			}
			else if(mkey==9)
			{
				mkey=0;
				PF-=1000;
				if(PF<1000)PF=1000;
			}
		}break;
		
		case 1:
		{
			if(mkey==8)
			{
				mkey=0;
				a+=100;
				if(a>900)a=900;
			}
			else if(mkey==9)
			{
				mkey=0;
				a-=100;
				if(a<-900)a=-900;
			}
		}break;
	}
}

// 显示频率上限参数。
void show_canshu1()
{
	Temp[0]=14;
	Temp[1]=1;
	Temp[2]=10;
	Temp[3]=10;
	Temp[4]=PF/1000;
	Temp[5]=0;
	Temp[6]=0;
	Temp[7]=0;	
}

// 显示校正参数。
void show_canshu2()
{
	if(a==0)
	{
	Temp[0]=14;
	Temp[1]=2;
	Temp[2]=10;
	Temp[3]=10;
	Temp[4]=10;
	Temp[5]=10;
	Temp[6]=10;
	Temp[7]=0;


	}
	else
	{Temp[0]=14;
	Temp[1]=2;
	Temp[2]=10;
	Temp[3]=10;
	if(a>=0)
	{Temp[4]=10;
	Temp[5]=a/100;
	Temp[6]=0;
	Temp[7]=0;}

	else 
	{Temp[4]=15;
	Temp[5]=(-a)/100;
	Temp[6]=0;
	Temp[7]=0;}}
	
}

//==================================time   mode2
// 关闭 DS1302 写保护，按地址写入初始时间，再重新开启写保护。
void set_time()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<3;i++)
	{
		Write_Ds1302_Byte(TimeWcommand[i],Time[i]);
	}
	Write_Ds1302_Byte(WP,0x8e);
}

// 读取 DS1302 的时间寄存器，供显示函数使用。
void read_time()
{
	unsigned char i;
	for(i=0;i<3;i++)
	{
		Time[i]=Read_Ds1302_Byte(TimeRcommand[i]);
	}
}

// 把 DS1302 的 BCD 时间拆成十位和个位，写入 Temp。
void show_time()
{
	Temp[0]=Time[0]/16;
	Temp[1]=Time[0]%16;
	Temp[2]=15;
	Temp[3]=Time[1]/16;
	Temp[4]=Time[1]%16;
	Temp[5]=15;
	Temp[6]=Time[2]/16;
	Temp[7]=Time[2]%16;
}
//====================================back   mode3
// 显示记录到的最高频率。
void show_HF()
{

	unsigned int num=HF;
	unsigned char pos=3;
	unsigned int chushu=10000;
	
	while(chushu>1&&num/chushu==0)
	{
		Temp[pos++]=10;
		chushu/=10;
	}
	
	while(chushu>=1)
	{
		Temp[pos++]=num/chushu%10;
		chushu/=10;
	}
	
	if(num==0)
	{
		Temp[7]=0;
	}
}

// 显示最高频率对应的时间。
void show_sj()
{
			Temp[0]=12;
			Temp[1]=13;
	Temp[2]=b/16;
	Temp[3]=b%16;

	Temp[4]=c/16;
	Temp[5]=c%16;

	Temp[6]=d/16;
	Temp[7]=d%16;

}

// 切换历史记录中的频率与发生时间。
void back()
{
	if(mkey==5)
	{
		mkey=0;
		if(++backmode==2)
		{
			backmode=0;
		}
	}
	switch(backmode)
	{
		case 0:             //show_HF
		{
			Temp[0]=12;
			Temp[1]=11;
			Temp[2]=10;
			show_HF();
		}break;
		
		case 1:
		{
			show_sj();
		}
	}
}
//===========================================show_seg  
// 按 mode 切换实时频率、参数、时间和历史记录页面。
void show_seg()
{
	if(mkey==4)
	{
		mkey=0;
		if(++mode==4)
		{
			mode=0;
		
		}	if(mode==1)
			{
				canshumode=0;
			}
			if(mode==3)
			{
				backmode=0;
			}
	}
	switch(mode)
	{
		case 0:
		{
			show_ne555();
		}break;
		
		case 1:
		{
			
			canshu_loop();
			if(canshumode==0)show_canshu1();
			else if(canshumode==1)show_canshu2();
		}break;
		
		case 2:
		{
			show_time();
		}break;
		
		case 3:
		{
		
			back();
		}break;
	}
}
//===========================dac
// 通过 I2C 给 PCF8591 写入 DAC 数值。
void write_dac(unsigned char Data)
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
// 把频率换算成 DAC 输出范围并发送；这里保留原公式供复习。
void dacwork()
{
	if(dacdata2>PF)
	{
		dacdata2=500;
	}
	else if(dacdata2<100)
	{
		dacdata2=100;
	}
	else
	{
	dacdata2=((400.0/((float)PF)-500)*F+100-(2000.0/(float)PF-500));
	}
	
	dacdata1=dacdata2/500.0*255;
	
	write_dac((unsigned char)dacdata1);
}



//==========================
// 初始化计数器、时钟和外设；循环更新显示与 DAC。
void main()
{
	init();
	Timer0Init();
	Timer1Init();
	set_time();
	HF=0;
	b=Time[0];
	c=Time[1];
	d=Time[2];
	while(1)
	{
		read_time();
		show_seg();
		dacwork();
	}
}
