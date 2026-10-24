#include <STC15F2K60S2.H>

void UART_Init()//4800bps@11.0592MHz
{
	SCON=0x40;    //只发出数据不接收数据
	AUXR&=0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR&=0xFE;		//串口1选择定时器1为波特率发生器  
	PCON|=0x80;	  //设置波特率	
	TMOD=0x20;	  //选择定时器0模式为8位自动重装		
	TL1=0xF4;			//设置定时器初值
	TH1=0xF4;		  //设置定时器重装值
	//ET1=0;        //禁止定时器1中断 上电自动置1
	TR1=1;				//启动定时器1
}

void UART_SendByte(unsigned char Byte)
{
	SBUF=Byte;
	while(TI==0);//判断是否发送完成，完成后IT由硬件自动置1
	TI=0;        //由软件置0
}