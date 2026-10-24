// 本例主题：串口接收命令控制 LED。

#include <STC15F2K60S2.h>

/*  实现串口命令   LED       */

//=================LED状态量
unsigned int led_state=0xff;

//==================串口命令变量
unsigned char command;


//===================选择译码器
// 选择 74HC138 译码器通道；操作结束选 0，避免 P0 的后续写入影响外设。
void select138(unsigned char n)
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

//====================初始化
// 初始化 LED、蜂鸣器和继电器，让上电状态明确。
void Init()
{
	select138(4);
	P0=led_state;
	select138(0);
	
	select138(5);
	P0=0x00;
	select138(0);
}

//====================串口
// 配置串口为 9600 bps，并打开串口中断。
void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		// 串口方式 1：8 位数据，允许接收
	AUXR |= 0x40;		// 定时器 1 使用 1T 模式
	AUXR &= 0xFE;		// 使用定时器 1 产生串口波特率
	TMOD &= 0x0F;		// 清除定时器 1 模式位
	TL1 = 0xC7;		// 9600 bps 波特率初值的低字节
	TH1 = 0xFE;		// 9600 bps 波特率初值的高字节
	ET1 = 0;		// 关闭定时器 1 中断；这里仅用它产生波特率
	TR1 = 1;		// 启动定时器 1
	ES=1;
	EA=1;
}

// 向串口发送 1 字节数据，等待发送完成。
void sendbyte(unsigned char dat)
{
	SBUF=dat;
	while(TI==0)
	TI=0;

}

// 串口接收中断：收到字节后放入 command，并清除接收标志。
void readbyte()   interrupt 4
{
	if(RI==1)
	{
		command=SBUF;
		RI=0;
	}
}

//=====================workLED
// 把收到的命令当作 LED 状态写到 P0，然后清除命令。
void workLED()
{
	if(command!=0x00)
	{
		led_state=command;
		select138(4);
		P0=led_state;	
		command=0x00;
		select138(0);
	}
}






//==========================================
// 先初始化外设和串口，再在主循环中处理收到的命令。
void main()
{
	Init();
	UartInit();	
	while(1)
	{	
	 workLED();

	}

}