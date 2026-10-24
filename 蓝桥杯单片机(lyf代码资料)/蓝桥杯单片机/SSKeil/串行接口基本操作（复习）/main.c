#include <STC15F2K60S2.H>

void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xC7;		//设定定时初值
	TH1 = 0xFE;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	EA=1;
	ES=1;
}

void UART_SendByte(unsigned char Data)
{
	SBUF=Data;
	while(TI==0);
	TI=0;
}

void UART_ReadByte() interrupt 4
{
	if(RI)
	UART_SendByte(SBUF);
	RI=0;
}
//void UART_SendByte(unsigned char Byte)
//{
//	SBUF=Byte;
//	while(TI==0);
//	TI=0;
//}

//void UART_ReadByte() interrupt 4
//{
//	if(RI==1)
//	UART_SendByte(SBUF+1);
//	RI=0;
//}

void main()
{
	UartInit();
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	UART_SendByte(0xA5);
	UART_SendByte(0x5A);
	while(1)
	{
		
	}
}