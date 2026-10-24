// 本例主题：AT24C02 掉电存储。

#include <STC15F2K60S2.h>
#include <IIC.h>

/*     实现掉电存储数据       */

unsigned char AT_Data;

// smg 是段码表，数组下标对应要显示的数字或符号。
unsigned char smg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};//0~9 熄灭
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={11,11,11,11,11,11,11,11};

//================================   写周期   每次写完之后要有5ms的写周期（后面要加上5ms延迟）
// AT24C02 写入后需要写周期；连续写入前等待约 5 ms。
void Delay5ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 59;
	j = 90;
	do
	{
		while (--j);
	} while (--i);
}



//==================================
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

//==============================
// 初始化 LED、蜂鸣器和继电器，让上电状态明确。
void Init()
{
	select(4);
	P0=0xff;
	select(0);
	
	select(5);
	P0=0x00;
	select(0);
}

//=============================
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
	P0=smg[Temp[Bit]];
	select(0);
	if(++Bit==8)
	{
		Bit=0;
	}

}

//===========================定时器中断
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
void Timer0()   interrupt 1
{
	Nixie();
	
}

//==============================AT24C02     0xa0写模式    0xa1读模式 856
// I2C 写入顺序：器件写地址 → 存储单元地址 → 数据。
void AT_write(unsigned char address,unsigned char Data)
{
	IIC_Start();
	IIC_SendByte(0xa0);           //开启写模式
	IIC_WaitAck();
	IIC_SendByte(address);
	IIC_WaitAck();
	IIC_SendByte(Data);
	IIC_WaitAck();
	IIC_Stop();
}

// 先写入目标单元地址，再重复起始并切换到读地址取回数据。
unsigned char AT_read(unsigned char address)
{
	unsigned char Data;
	IIC_Start();
	IIC_SendByte(0xa0);           //开启写模式
	IIC_WaitAck();
	IIC_SendByte(address);
	IIC_WaitAck();
	 
	IIC_Start();
	IIC_SendByte(0xa1);            //开启读模式
	IIC_WaitAck();
	Data=IIC_RecByte();
	IIC_SendByte(1);
	IIC_Stop();
	return Data;
}




//===============================
// 先向 EEPROM 写两个示例数据，再读出并显示；主循环保持数码管扫描。
void main()
{
	Init();
	Timer0Init();
	AT_write(2,3);
	Delay5ms();
	AT_write(3,7);
	Delay5ms();
	AT_Data=AT_read(3);
	Temp[0]=AT_Data;
	AT_Data=AT_read(2);
	Temp[7]=AT_Data;
//	Temp[3]=6;
	
		while(1)
		{
			
		}

}
