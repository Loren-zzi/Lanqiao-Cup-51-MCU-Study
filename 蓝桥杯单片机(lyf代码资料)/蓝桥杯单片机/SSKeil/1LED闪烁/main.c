//#include <STC15F2K60S2.H>
//#include "intrins.h"

//void Delay500ms()		//@12.000MHz
//{
//	unsigned char i, j, k;

//	_nop_();
//	_nop_();
//	i = 23;
//	j = 205;
//	k = 120;
//	do
//	{
//		do
//		{
//			while (--k);
//		} while (--j);
//	} while (--i);
//}



//void main()
//{
//	P2=((P2&0x1f)|0xa0);
//	P0=0x00;
//	P2&=0x1f;
//	while(1)
//	{
//		
//		P2=((P2&0x1f)|0x80);
//		P0=0x0f;
//		P2&=0x1f;
//		Delay500ms();
//		P2=((P2&0x1f)|0x80);
//		P0=0xf0;
//		P2&=0x1f;
//		Delay500ms();
//	}
//}

#include "STC15F2K60S2.h"
#include "iic.h"    //I2C????? 
#include "intrins.h"//_nop()_
#include <stdio.h> //sprintf
//                             0    1    2     3    4    5   6     7    8    9  10?  11.  12U
code unsigned char tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0x7f,0xc1};
unsigned char dspbuf[8] = {12,10,10,10,10,0,0,0};  //?????
unsigned char dspcom = 0;

unsigned char intr;
unsigned char ad_value;
unsigned int v_value;

unsigned char x=0xff;
bit flag_100;
bit flag500;
unsigned char s[]= {"value="};

void SendString(char *s);
void SendData(unsigned char dat);
unsigned char str[20];
unsigned char str_receive[6];
unsigned char rxcnt=0;

void Delay10us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 27;
	while (--i);
}

void init_pcf8591(unsigned char channel)
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(channel);  //ADC??3
	IIC_WaitAck();
	IIC_Stop();
	Delay10us();
}

unsigned char read_pcf8591(void)
{
	unsigned char temp;

	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	temp = IIC_RecByte();
	IIC_SendAck(0);     //??????? 
	IIC_Stop();
	
	return temp;
}
void UartInit(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//01010000
	AUXR |= 0x40;		//???1???Fosc,?1T
	AUXR &= 0xFE;		//??1?????1???????
	TMOD &= 0x0F;		//?????1?16???????
	TL1 = 0xC7;		//??????
	TH1 = 0xFE;		//??????
	ET1 = 0;		//?????1??
	TR1 = 1;		//?????1
}
void SendData(unsigned char dat)
{
	SBUF=dat;
	while(TI == 0);  //?????????
	TI = 0;
}
void SendString(char *s)//Send string 
{
    while (*s!='\0')              //Check the end of the string
    {
        SendData(*s++);     //Send current char and increment string ptr 
    }
}
void Receivedata(void)
{
	if(RI)
    {
        RI = 0;  //??????? 
        str_receive[rxcnt++] = SBUF;
 		}
	}
void clrbuzz(void)
{
		P2= P2&0x1f|0xa0;//P2=10100000, P2=101*****
    P06=0; P04=0;
		P2= P2&0x1f;
}
void clrled(void)
{
		P2=P2&0x1f|0x80;
    P0=0xff;
		P2=P2&0x1f;
}
void Timer0Init(void)		//2ms@12.000MHz
{
	AUXR |= 0x80;		//?????1T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x40;		//??????
	TH0 = 0xA2;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	ET0=1;
	EA=1;
}
void display(void)
{   
	P2 = ((P2&0x1f)|0xe0);//??
	P0 = 0xff;
	P2 &= 0x1f;
	P2 = ((P2&0x1f)|0xc0);//???
	P0 = (1<<dspcom);
	P2 &= 0x1f;
	P2 = ((P2&0x1f)|0xe0);//??
	P0 = tab[dspbuf[dspcom]];
	if (dspcom==5){
  P0 = tab[dspbuf[dspcom]]&0x7f;//0x7f=01111111
  }
	P2 &= 0x1f;
  if(++dspcom == 8)
		{
        dspcom = 0;
    }    
}
void led1(void)
{ 
   if(flag500)
	 {
			flag500=0; 
			P2=P2&0x1f|0x80;
			x=x^0x01;
			P0=x;
			P2=P2&0x1f;
   }	 
}
//???
void main(void)
{         
    clrbuzz();
	  clrled();
    Timer0Init();
    UartInit();
    while(1)
		{
			if(flag_100==1)
			{	
				flag_100=0;
        init_pcf8591(0x03); 
	      ad_value= read_pcf8591(); //ad_value:0--255
        v_value=ad_value*100/51;  // v_value:0--500
				
	      //??????
// 				dspbuf[4] = v_value/100; 
//        dspbuf[5]	= 11;	
// 				dspbuf[6] = v_value%100/10;       
// 				dspbuf[7] = v_value%10;  
				
        dspbuf[5]	= v_value/100; 						
				dspbuf[6] = v_value%100/10;       
				dspbuf[7] = v_value%10;     			
			}
			
			Receivedata();
			if(rxcnt==1)
			{ 
				rxcnt=0;
				//sprintf(str,"%s%d%c%c",s,v_value,'\r','\n');
				sprintf(str,"µçÑ¹£º2.55V\r\n");
				SendString(str);	
				
				SendData(0x30+v_value/100);//5-->'5'
				SendData('.');
				SendData(0x30+v_value%100/10);
				SendData(0x30+v_value%10);
				SendData('\r');SendData('\n');  
			}	
			
			if(v_value>200){
					led1();
			}	
      else{
					P2=0x80;
					P0=0xff;
					P2=0x00;
      }			
		}
    
}

//???0??????
void isr_timer_0(void)  interrupt 1  
{
	  static unsigned char n=0;
    display();
	  if(++intr==50)
		{
			flag_100=1;
			intr=0;
			n++;
		}
		if(n==5)
		{
			flag500=1;
			n=0;
		}
}