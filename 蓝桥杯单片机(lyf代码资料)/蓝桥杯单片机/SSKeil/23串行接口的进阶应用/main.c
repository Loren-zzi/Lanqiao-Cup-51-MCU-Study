#include <STC15F2K60S2.H>

unsigned char Command;

void UartInit(void)		//9600bps@11.0592MHz
{
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xFA;		//设定定时初值
	TH1 = 0xFA;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	ES=1;
	EA=1;
}

void UART_SendByte(unsigned char Byte)
{
	SBUF=Byte;
	while(TI==0);
	TI=0;
}

void UART_ReadByte() interrupt 4
{
	if(RI==1)
	Command=SBUF;
	RI=0;
}

void UART_SendString(unsigned char *str)
{
	while(*str!='\0')//注意while循环
	{
		UART_SendByte(*str++);//不可在外面++
	}
}

void UART_Command()
{
	switch(Command&0xf0)
	{
		case 0xA0:
		{
//			switch(Command&0x0f)
//			{
//				case 0x00:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xff);
//					P2&=0x1f;
//				}break;
//				case 0x01:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xfe);
//					P2&=0x1f;
//				}break;
//				case 0x02:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xfd);
//					P2&=0x1f;
//				}break;
//				case 0x03:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xfc);
//					P2&=0x1f;
//				}break;
//				case 0x04:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xfb);
//					P2&=0x1f;
//				}break;
//				case 0x05:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xfa);
//					P2&=0x1f;
//				}break;
//				case 0x06:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf9);
//					P2&=0x1f;
//				}break;
//				case 0x07:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf8);
//					P2&=0x1f;
//				}break;
//				case 0x08:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf7);
//					P2&=0x1f;
//				}break;
//				case 0x09:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf6);
//					P2&=0x1f;
//				}break;
//				case 0x0A:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf5);
//					P2&=0x1f;
//				}break;
//				case 0x0b:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf4);
//					P2&=0x1f;
//				}break;
//				case 0x0c:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf3);
//					P2&=0x1f;
//				}break;
//				case 0x0d:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf2);
//					P2&=0x1f;
//				}break;
//				case 0x0e:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf1);
//					P2&=0x1f;
//				}break;
//				case 0x0f:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0x0f)&0xf0);
//					P2&=0x1f;
//				}break;
//			}
			P2=((P2&0x1f)|0x80);
			P0=(P0|0x0f)&(~Command|0xf0);
			P2&=0x1f;
		}break;
		case 0xB0:
		{
//			switch(Command&0x0f)
//			{
//				case 0x00:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0xff);
//					P2&=0x1f;
//				}break;
//				case 0x01:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0xef);
//					P2&=0x1f;
//				}break;
//				case 0x02:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0xdf);
//					P2&=0x1f;
//				}break;
//				case 0x03:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0xcf);
//					P2&=0x1f;
//				}break;
//				case 0x04:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0xbf);
//					P2&=0x1f;
//				}break;
//				case 0x05:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0xaf);
//					P2&=0x1f;
//				}break;
//				case 0x06:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x9f);
//					P2&=0x1f;
//				}break;
//				case 0x07:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x8f);
//					P2&=0x1f;
//				}break;
//				case 0x08:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x7f);
//					P2&=0x1f;
//				}break;
//				case 0x09:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x6f);
//					P2&=0x1f;
//				}break;
//				case 0x0A:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x5f);
//					P2&=0x1f;
//				}break;
//				case 0xbf:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x4f);
//					P2&=0x1f;
//				}break;
//				case 0x0c:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x3f);
//					P2&=0x1f;
//				}break;
//				case 0x0d:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x2f);
//					P2&=0x1f;
//				}break;
//				case 0x0e:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x1f);
//					P2&=0x1f;
//				}break;
//				case 0x0f:
//				{
//					P2=((P2&0x1f)|0x80);
//					P0=((P0|0xf0)&0x0f);
//					P2&=0x1f;
//				}break;
//			}
			P2=((P2&0x1f)|0x80);
			P0=(P0|0xf0)&(~Command<<4|0x0f);
			P2&=0x1f;
		}break;
		case 0xC0:
		{
			UART_SendString("System is running!\r\n");
			Command=0;
		}break;
	}
}

void main()
{
	UartInit();
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	UART_SendString("Welcome to LYF system!\r\n");
	while(1)
	{
		UART_Command();
	}
}