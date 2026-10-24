#include <STC15F2K60S2.h>
#include "mkey.h"
 
 //实现功能：矩阵键盘；按按键1加1变量，按案件二减1变量
 
 // 示例计数值：矩阵键盘的 1 键加 1、2 键减 1。
 unsigned int a;
 

//======================按键变量
// mkey 保存“已经松开的按键”编号；主循环读到后立刻清零。
unsigned char mkey=0;
 
// 段码表：数字 0~9 对应的数码管亮灯方式；最后两个值供横杠和熄灭使用。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};//0~9 - 熄灭
// 显示缓冲区：8 个元素对应 8 位数码管。主循环改 Temp，定时器负责逐位显示。
unsigned char Temp[]={11,11,11,11,11,11,11,11};	//数码管缓存(就是数码管显示的内容)，初始是11是为了让数码管开始第一次是熄灭状态

 //================================LED状态量   防止其他译码器使用时状态丢失
// LED 为低电平点亮；0xff 表示 8 个 LED 初始全部熄灭。
unsigned char state_led=0xff;

//=================================//选择译码器
// P2 高 3 位连接 74HC138 译码器；n 为 4/5/6/7 时分别选通 LED、蜂鸣器/继电器、位选、段选。
// 每次操作完再选 0，避免后续写 P0 时误改其他外设。
void select138(unsigned char n)  
{
		switch(n)
		{
			case 0:P2=(P2&0x1f)|0x00;break;  //关闭
			case 4:P2=(P2&0x1f)|0x80;break;	//LED
			case 5:P2=(P2&0x1f)|0xa0;break;		//蜂鸣器，继电器
			case 6:P2=(P2&0x1f)|0xc0;break;		//位选
			case 7:P2=(P2&0x1f)|0xe0;break;		//段选
		}

}

//=============================初始化
// 上电先关闭 LED、蜂鸣器和继电器，防止程序开始时误亮或误响。
void Init()
{
	select138(4);        //LED初始化
	P0=state_led;
	select138(0);
	
	select138(5);					//蜂鸣器。继电器初始化
	P0=0x00;
	select138(0);
}

//==========================数码管
// 动态扫描：一次只点亮一位；每 2 ms 切换下一位，8 位循环后看起来会同时发光。
// 顺序是“先灭段码消影 → 选中当前位 → 写入该位段码”。
void Nixie()
{
	static unsigned char Bit;
	select138(7);				//开7，段选，请零，消影
	P0=0xff;
	select138(0);
	
	select138(6);				//开6，位选
	P0=0x01<<Bit;
	select138(0);
	
	select138(7);				//开7，段选
	P0=seg[Temp[Bit]];
	select138(0);
	if(++Bit>=8)//				八次
		Bit=0;
}



//=============================定时器0   2ms
// 12 MHz 晶振、1T 模式下，每 2 ms 触发一次定时器 0 中断。
// 2 ms 需要 24000 个计数，装入初值 65536-24000=0xA240。
void Timer0Init(void)		// 2 ms @ 12.000 MHz
{
	AUXR |= 0x80;		// 定时器 0 使用 1T 模式
	TMOD &= 0xF0;		// 定时器 0 设为 16 位定时模式
	TL0 = 0x40;		// 定时初值低字节
	TH0 = 0xA2;		// 定时初值高字节
	TF0 = 0;		// 清除定时器 0 溢出标志
	TR0 = 1;		// 启动定时器 0
	ET0=1;		// 允许定时器 0 中断
	EA=1;		// 开启总中断
}


// 数码管每 2 ms 扫描一位；矩阵键盘每 20 ms 扫描一次。
void Timer0()   interrupt 1   //2ms
{
	static unsigned int count0;
	Nixie();
	if(++count0==10)             //20ms检验一次矩阵键盘
	{
		mkey_loop();
		count0=0;
	}
	
}






// 按键扫描在 mkey.c 中；这里只处理按键事件并更新显示缓冲区。
void main()
{
	Init();
	Timer0Init();
	while(1)
	{
		if(mkey==1)
		{
			mkey=0;
			a++;
		}
		if(mkey==2)
		{
			mkey=0;
			a--;
		}
		// 这里只显示两位；若要持续计数，应给 a 增加 0~99 的边界处理。
		Temp[6]=a/10;
		Temp[7]=a%10;
		
	}

}
