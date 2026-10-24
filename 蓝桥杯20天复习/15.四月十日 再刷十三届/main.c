// 本例主题：再次练习第十三届省赛题。

#include <STC15F2K60S2.h>
#include "ds1302.h"
#include "onewire.h"
#include "intrins.h"

sbit list3=P3^5;
sbit list4=P3^4;
sbit line1=P3^3;
sbit line2=P3^2;
//=========================
unsigned char mode,workmode;
bit jdqkai;

bit jdqstart,jdqover=1;
unsigned int timer5s;

unsigned char led=0xff;
bit led1start,led1over=1;
unsigned int led5s;

//===========================
code // seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xff,0xbf,0xc1};//22 U
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={20,20,20,20,20,20,20,20};
unsigned char mkey;

unsigned char a=20;
float T;

unsigned char Time[]={0x11,0x59,0x58};
unsigned char TimeWcommand[]={0x84,0x82,0x80};
unsigned char TimeRcommand[]={0x85,0x83,0x81};
#define WP 0x8e
unsigned char fen,miao;

unsigned char canshu=23;

//=============================
// 给温度传感器留出首次转换时间；这里是阻塞式延时。
void Delay750ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 35;
	j = 51;
	k = 182;
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
//==================================定时器
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

// 每 2 ms 扫描数码管，并累计按键、继电器和 LED 的计时。
void Timer()   interrupt 1
{
	static unsigned int count0,count1;
	Nixie();
	if(++count0==10)
	{
		count0=0;
    mkey_loop();
	}
	if(jdqstart)
	{
		if(++timer5s==2500)
		{	
			timer5s=0;
			jdqover=0;
		}
	}
	if(led1start)
	{
		if(++led5s==2500)
		{
			led5s=0;
			led1over=0;
		}
	}
	if(jdqkai)
	{
		if(++count1==50)
		{
			count1=0;
			led^=0x04;
			select(4);
			P0=led;
			select(0);
		}
	}
	else if(jdqkai==0)
	{
		led|=0x04;
		select(4);
		P0=led;
		select(0);
	}
}
bit jdqstart=0,jdqover=1;

// 这段也是定时器 0 中断函数；原文件已有 Timer() 使用同一中断号。
// 注意：原练习稿这里还保留了重复的中断函数和函数外代码，需整理后才能编译。
void timer() interrupt 1
{
	static unsigned int count0;
	if(jdqstart)
	{
		if(++count0==2500)
		{
			count0=0;
			jdqover=0;									//改变结束标志位
		}
	}
}

if(fen==0&&miao==0&&!jdqstart)
{
	jdqstart=1;
	select(5);
	P0=0x10;
	select(0);
}
if(!jdqover)
{
	jdqover=1;
	jdqstart=0;
	select(5);
	P0=0x00;
	select(0);
}

//=============================temper      mode0
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

//===========================    time   mode1
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
//=============================   canshu   mode2
// 根据按键修改参数，并限制参数的取值范围。
void canshu_loop()
{
	if(mkey==16)
	{
		mkey=0;
		canshu++;
		if(canshu>=99)
		{
			canshu=99;
		}
	}
	if(mkey==17)
	{
		mkey=0;
		canshu--;
		if(canshu<=10)
		{
			canshu=10;
		}
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





//=============================     show_seg
// 按 mode 选择温度、时间或参数页面。
void show_seg()
{
	if(mkey==12)
	{
		mkey=0;
		if(++mode==3)
		{
			mode=0;
		}
	}
	
	switch(mode)
	{
		case 0:
		{
			show_temper();
		}break;
		
		case 1:
		{
			show_time1();
			if(mkey==17)
			{
				show_time2();
			}
		}break;
			
		case 2:
		{
			canshu_loop();
			show_canshu();
		}break;
		
	}

}
//===========================继电器work
// 按工作模式控制继电器：温度阈值或定时触发。
void jdq_work()
{
	if(mkey==13)
	{
		mkey=0;
		if(++workmode==2)
		{
			workmode=0;
		}
	}

	switch(workmode)
	{
		case 0:
		{
			if(T>canshu)
			{
				select(5);
				P0=0x10;
				select(0);
				jdqkai=1;
			}
			else
			{
				select(5);
				P0=0x00;
				select(0);
				jdqkai=0;
			}
		}break;
		
		case 1:
		{
			if(fen==0&&miao==0&&!jdqstart)
			{
				jdqstart=1;
				select(5);
				P0=0x10;
				select(0);
				jdqkai=1;

			}
			if(!jdqover)
			{
				jdqstart=0;
				jdqover=1;
				select(5);
				P0=0x00;
				select(0);
				jdqkai=0;
			}
			
		}break;
	}
}

//========================ledwork
// 根据计时和工作模式更新 LED。
void led_work()
{
	if(fen==0&&miao==0&&!led1start)
	{
		led5s=0;
		led1start=1;
		led&=0xfe;
		select(4);
		P0=led;
		select(0);
	}
	if(!led1over)
	{
		led1start=0;
		led1over=1;
		led|=0x01;
		select(4);
		P0=led;
		select(0);
	}
		if(workmode==0)
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


//================
// 持续读取温度与时间，更新显示、继电器和 LED。
void main()
{
	init();
	Timer0Init();
	read_temper();
	Delay750ms();
	set_time();
	while(1)
	{
		read_temper();
		r_time();
		show_seg();
		jdq_work();
		led_work();
	}
}