// 本例主题：第十三届省赛题综合练习。

#include <STC15F2K60S2.h>
#include "ds1302.h"
#include "onewire.h"
#include "intrins.h"
sbit list3=P3^5;
sbit list4=P3^4;
sbit line1=P3^3;
sbit line2=P3^2;

unsigned int time5s_count=0;
unsigned int time5s_count1=0;

bit time5s_flag=0;               //标志位
bit time5s_flag1=0;               //标志位

bit led3liang;

unsigned char mode,workmode;
unsigned char led=0xff;
unsigned char jdq_state=0x00;


bit jdq_flag,jdq5s,led5s;

code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xbf,0xc1};//22 U
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={20,20,20,20,20,20,20,20};
unsigned char mkey;
float T;

unsigned char Time[]={0x11,0x59,0x58};
unsigned char TimeWcommand[]={0x84,0x82,0x80};
unsigned char TimeRcommand[]={0x85,0x83,0x81};
#define WP 0x8e

unsigned char canshu=23;

unsigned char miao,fen;

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
//==================================mkey
// 扫描键盘行列，返回当前按下的键号；没有按键应返回 0。
unsigned char mkey_state()
{
	unsigned char mkey_state;
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
	
	if(last==17&&now==17)mkey=1;
	if(last==17&&now==0)mkey=2;
}
//==============================定时器
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
void ledwork();

// 每 2 ms 扫描显示；同时累计按键和约 5 秒的状态计时。
void Timer()   interrupt 1
{
	static unsigned int count0,count1;
	Nixie();
	ledwork();
	if(++count0==10)
	{
		count0=0;
		mkey_loop();
	}

	if(time5s_count<2500)
	{
		time5s_count++;
	}
	else
	{
		time5s_count=0;
		time5s_flag1=1;
	}
	
		if(time5s_count1<2500)
	{
		time5s_count1++;
	}
	else
	{
		time5s_count1=0;
		time5s_flag1=1;
	}
	
	if(jdq_flag)
	{
		led3liang=1;
		if(++count1==50)
		{
			count1=0;
			led3liang=!led3liang;
		}
	}
	else
	{
		led3liang=0;
	}
}

//================Temper
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
	Temp[0]=22;
	Temp[1]=1;
	Temp[2]=20;
	Temp[3]=20;
	Temp[4]=20;
	Temp[5]=(int)T/10;
	Temp[6]=(int)T%10+10;
	Temp[7]=(int)(T*10)%10;
}

//=========================Time
// 关闭 DS1302 写保护，按地址写入初始时间，再重新开启写保护。
void set_time()
{
	unsigned char i;
	Write_Ds1302_Byte(WP,0x00);
	for(i=0;i<=2;i++)
	{
		Write_Ds1302_Byte(TimeWcommand[i],Time[i]);
	}
	Write_Ds1302_Byte(WP,0x80);
}

// 读取 DS1302 的时间寄存器，供显示函数使用。
void r_time()
{
	unsigned char i;
	for(i=0;i<=2;i++)
	{
		Time[i]=Read_Ds1302_Byte(TimeRcommand[i]);
	}
	fen=Time[1]/16*10+Time[1]%16;
	miao=Time[2]/16*10+Time[2]%16;
}

// 把当前时间排成数码管显示格式。
void show_time1()
{
	Temp[0]=22;
	Temp[1]=2;
	Temp[2]=20;
	Temp[3]=Time[0]/16;
	Temp[4]=Time[0]%16;
	
	Temp[5]=21;
	
	Temp[6]=Time[1]/16;
	Temp[7]=Time[1]%16;

}

// 把时间的另一种显示内容写入 Temp。
void show_time2()
{
	Temp[0]=22;
	Temp[1]=2;
	Temp[2]=20;
	Temp[3]=Time[1]/16;
	Temp[4]=Time[1]%16;
	
	Temp[5]=21;
	
	Temp[6]=Time[2]/16;
	Temp[7]=Time[2]%16;

}




//=========================参数
// 根据按键修改参数，并限制参数的取值范围。
void canshu_loop()
{
	if(mkey==16)
	{
		mkey=0;
		canshu++;
		if(canshu>99)
		{
			canshu=99;
		}
	}
	
	else if(mkey==2)
	{
		mkey=0;
		canshu--;
		if(canshu<10)canshu=10;
	}
}

// 把当前参数拆成数字，写入 Temp。
void show_canshu()
{
	Temp[0]=22;
	Temp[1]=3;
	
	Temp[2]=20;
	Temp[3]=20;
	Temp[4]=20;
	Temp[5]=20;
	
	Temp[6]=canshu/10;
	Temp[7]=canshu%10;
}

//====================================work
// 按 workmode 切换温度、时间、参数显示。
void work()
{
	if(mkey==12)
	{
		mkey=0;
		if(++workmode==3)
		{
			workmode=0;
		}
	}
	
	if(workmode==0)
	{
		show_temper();
	}
	
	if(workmode==1)
	{
		if(mkey==1)
		{
			if(mkey==2)
			{
				mkey=0;
			}
			show_time1();
		}
		else 
		{
			mkey=0;
			show_time2();
		}
	}
	
	if(workmode==2)
	{
		canshu_loop();
		show_canshu();
	}
}
//======================================继电器
// 根据模式与温度阈值，控制继电器状态。
void jdq()
{
	if(mkey==13)
	{
		mkey=0;
			if(++mode==2)
		{
			mode=0;
		}
	}
	
	if(mode==0)
	{
		if(T>canshu)
		{
			select(5);
			jdq_state|=0x10;
			P0=jdq_state;
			select(0);
			jdq_flag=1;
		}
		else
		{
			select(5);
			jdq_state&=0x10;
			P0=jdq_state;
			select(0);
			jdq_flag=0;
		}
	}
	
	else if(mode==1)
	{
		if(fen==0&&miao==0&&!jdq5s)
		{
			select(5);
			P0=0x10;
			select(0);
			time5s_count=0;
			jdq5s=1;
			jdq_flag=1;
			if(jdq5s)
			{
					if(time5s_flag==1)
				{
					time5s_flag=0;
					select(5);
					P0=0x00;
					select(0);	
					jdq5s=0;
					jdq_flag=0;

				}
			}
	
		}
	}
}
//===================================led
// 根据继电器和计时状态更新 LED。
void ledwork()
{
	if(fen==0&&miao==0&&!led5s)
	{
		led5s=1;
		select(4);
		led&=0xfe;
		P0=led;
		select(0);
		time5s_count1=0;
	}
	if(led5s)
	{
		if(time5s_flag1==1)
		{
			time5s_flag1=0;
			led5s=0;
			select(4);
			led|=0x01;
			P0=led;
			select(0);
		}
	}

	if(mode==0)
	{
		select(4);
		led&=0xfd;
		P0=led;
		select(0);
	}
	else if(mode==1)
	{
		select(4);
		led|=0x02;
		P0=led;
		select(0);
	}
	if(led3liang)
	{
		select(4);
		led&=0xfb;
		P0=led;
		select(0);
	}
	else if(led3liang==0)
	{
		select(4);
		led|=0x04;
		P0=led;
		select(0);
	}
}

//========================================
// 持续读取温度和时间，再更新界面与继电器。
void main()
{
	Timer0Init();
	init();
	read_temper();
	Delay800ms();                     
	set_time();
	while(1)
	{
		read_temper();
		r_time();
		work();
		jdq();

	}
}




