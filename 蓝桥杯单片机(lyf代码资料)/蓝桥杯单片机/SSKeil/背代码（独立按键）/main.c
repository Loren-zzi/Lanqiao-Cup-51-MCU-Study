#include "STC15F2K60S2.h"

unsigned char key_value=0;
unsigned char intr;
bit key_re;
bit key_flag;
                           //  0    1    2    3    4    5    6    7    8    9    全灭
code unsigned char tab[] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};
unsigned char dspbuf[8] = {10,10,10,10,10,0,0,0};  //显示缓冲区
unsigned char dspcom = 0;

void read_key(void);
void display(void);

//定时器0初始化函数
void Timer0Init(void)		//2ms@12.000MHz
{
	AUXR &= 0x7F;		//?????12T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x30;		  //??????
	TH0 = 0xF8;		  //??????
	TF0 = 0;		    //??TF0??
	TR0 = 1;		    //???0????
	ET0=1;
	EA=1;
}


//主函数
void main(void)
{ 
    unsigned char num;
    
    P3 = 0xff;  //按键接口初始化
	
    Timer0Init();
		P2=((P2&0xA0)|0x80);
		P0=0x00;
		P2&=0x1f;
    while(1)
    {
        if(key_flag)
        {
            key_flag = 0;
            read_key();
        }        
        //进入按键处理程序部分
        switch(key_value)
        {
            
					 case 1://S7
						       num++; 
						       key_value= 0; 
						       break;  //跳出switch，如果不写break，后面的语句都会执行，直到遇到braek
           case 2://S6
				           num--; 
						       key_value= 0; 
						       break;      
        }
		   
			//key_value= 0;  //完成按键处理程序后，清除键值
		   
		   dspbuf[5] = num/100;
		   dspbuf[6] = num%100/10;
		   dspbuf[7] = num%10;

			 if(num==1){
						P2 = ((P2&0x1f)|0x80);
						P0 = 0x55;//01010101
						P2 &= 0x1f;
			  }
			  else{
						P2 = ((P2&0x1f)|0x80);
						P0 = 0xff;
						P2 &= 0x1f;
			 }
				
    }
}

//定时器0的中断服务函数
void isr_timer_0(void)  interrupt 1  
{    
    display();               //2ms执行一次
	  if(++intr == 10)  
	  {
      intr = 0;
		  key_flag = 1;          //20ms按键扫描标志位置1
    }
		
}

void read_key(void)                
{
	static unsigned char key_state=0;
	static unsigned char key_temp;
 	key_temp =P3&0x0f;//0000xxxx
	switch(key_state)
	{
		case 0:
		{
			//key_temp =P3&0x0f;
			if(key_temp != 0x0f) //有按键按下
			key_state=1;	
			
		}break;
		case 1:
		{
			//key_temp =P3&0x0f;
			if(key_temp!= 0x0f) //有按键按下
			{
				key_state=2;
				switch(key_temp)
        {
            case 0x0e:   //S7,00001110 
								key_value = 1; break;             
            case 0x0d:  //00001101
						    key_value = 2; break; 	    
            case 0x0b:
						    key_value = 3; break; 	    
            case 0x07:
						    key_value = 4; break; 	
        }
			}
			else
			{
				key_state=0;	
			}  	   
		}break;
		case 2:  
		{
			//key_temp =P3&0x0f;
			if(key_temp == 0x0f)
      {
        key_state=0;	
      }
		}break;
				
    } 
		
}
//显示函数
void display(void)
{   
		P2 = ((P2&0x1f)|0xe0);
		P0 = 0xff;
	  P2 &= 0x1f;
	
		P2 = ((P2&0x1f)|0xc0);
		P0 = (1<<dspcom);
	  P2 &= 0x1f;
	
		P2 = ((P2&0x1f)|0xe0);
		P0 = tab[dspbuf[dspcom]];
		P2 &= 0x1f;
    if(++dspcom == 8)
			{
        dspcom = 0;
      }   
}
