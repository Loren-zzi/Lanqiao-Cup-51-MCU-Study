//注意：串口助手需要清发送缓冲区
//将接收到的数字字符显示在数码管上

#include "STC15F2K60S2.h"
code unsigned char tab[11] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[8] = {0};  //显示缓冲区
unsigned char dspcom = 0;

unsigned char rxbuf[11];
unsigned char rxcnt=0, i;
bit rx_over = 0;  //命令接收完成标志位


void UartInit(void);
void display(void);
void Timer0Init(void);

//主函数
void main(void)
{ 
		P2=((P2&0x1f)|0xa0);
	  P04=0;P06=0;
	  P2 &=0x1f;
	  UartInit();
	  Timer0Init();
    
    while(1)
    {
        if(rx_over)
        {   
            for(i=0;i<8;i++)
            {
							dspbuf[i]=rxbuf[i]-0x30;//'12',rxbuf[0]=0x31,rxbuf[1]=0x32
						}
					  rx_over = 0;
						ES = 1;  //命令处理完成后，打开串口
        }
    }
}

//串口中断服务函数
void isr_uart(void) interrupt 4 
{
    if(RI)
    {   		  
        RI = 0;  //清除接收标志位 
        rxbuf[rxcnt] = SBUF;
			  rxcnt++;
				if(rxcnt==8)
					{ 
						rx_over = 1;
						rxcnt=0;
						ES = 0;
					}
    }	
	
    
}

void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8???,?????
	AUXR |= 0x40;		//???1???Fosc,?1T
	AUXR &= 0xFE;		//??1?????1???????
	TMOD &= 0x0F;		//?????1?16???????
	TL1 = 0xC7;		//??????
	TH1 = 0xFE;		//??????
	ET1 = 0;		//?????1??
	TR1 = 1;		//?????1
	ES=1;
	EA=1;
}
void Timer0Init(void)		//1ms@12.000MHz
{
	AUXR |= 0x80;		//?????1T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x20;		//??????
	TH0 = 0xD1;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	ET0=1;
	EA=1;
}

void isr_timer_0(void)  interrupt 1  //默认中断优先级 1
{
//     TH0 = (65536-2000)/256;
//     TL0 = (65536-2000)%256;  //定时器重载 
	  display();  //1ms执行一次
	  
}

//显示函数
void display(void)
{   
		P2 = ((P2&0x1f)|0xe0);//消隐
		P0 = 0xff;
	  P2 &= 0x1f;
		P2 = ((P2&0x1f)|0xc0);//位控制
		P0 = (1<<dspcom);
	  P2 &= 0x1f;
		P2 = ((P2&0x1f)|0xe0);//段码
		P0 = tab[dspbuf[dspcom]];
		P2 &= 0x1f;
   
    if(++dspcom == 8)
       {
        dspcom = 0;
       }   
}