#include <STC15F2K60S2.H>

void UART_Init()//4800bps@11.0592MHz
{
	SCON=0x50;    //发出数据且接收数据
	AUXR&=0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR&=0xFE;		//串口1选择定时器1为波特率发生器  
	PCON|=0x80;	  //设置波特率	
	TMOD=0x20;	  //选择定时器0模式		
	TL1=0xF4;			//设置定时器初值
	TH1=0xF4;		  //设置定时器重装值
	EA=1;					//开启总中断
	ES=1;					//开启串行口中断
	//ET1=0;        //关闭定时器1中断 上电自动置0
	TR1=1;				//启动定时器1
}

void UART_SendByte(unsigned char Byte)
{
	SBUF=Byte;
	while(TI==0);//判断是否发送完成，完成后IT由硬件自动置1
	TI=0;        //由软件置0
}

/*串口中断服务函数模板
void UART_Routine() interrupt 4
{
	if(RI==1)
	{
		RI=0;
	}
}
*/
