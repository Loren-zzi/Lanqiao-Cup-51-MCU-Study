#include <STC15F2K60S2.H>
#include "CLED.h"
#include "CBuzz.h"
#include "Timer0.h"
#include "Delay.h"

unsigned char Frequency_Select,Music_Select;
unsigned char Music_Select2,Music_Select3;
sbit Buzzer=P0^6;

#define SPEED 1053  //以4分音符为基准，给16、8、2、1分音符 4分音符需要计算一下，公式在本子上

#define P   0   //休止符
#define L1  1   //低音1
#define L_1 2   //低音1#
#define L2  3
#define L_2 4
#define L3  5
#define L4  6
#define L_4 7
#define L5  8
#define L_5 9
#define L6  10
#define L_6 11
#define L7  12
#define M1  13   //中音1
#define M_1 14   //中音1#
#define M2  15
#define M_2 16
#define M3  17
#define M4  18
#define M_4 19
#define M5  20
#define M_5 21
#define M6  22
#define M_6 23
#define M7  24
#define H1  25   //高音1
#define H_1 26   //高音1#
#define H2  27
#define H_2 28
#define H3  29
#define H4  30
#define H_4 31
#define H5  32
#define H_5 33
#define H6  34
#define H_6 35
#define H7  36

unsigned int Frequency_Table[]={
	0,
	63785,63880,63976,64061,64146,64222,64296,64366,64431,64493,64552,64607,
	64659,64708,64755,64799,64840,64879,64916,64951,64984,65015,65044,65072,
	65097,65122,65146,65168,65188,65208,65226,65243,65260,65275,65290,65304,
};

unsigned char code Music[]={
	
	//1
	P,4,
	P,4,
	P,2+1,
	M3,1,
	M2,1,
	M1,1,
	L6,2,
	
	//2
	M2,2,
	M3,2,
	M1,4,	
	P,2+1,
	M3,1,
	M2,1,
	M1,1,
	L6,2,
	
	M2,2,
	M3,2,
	M1,4,
	P,2+1,
	M3,1,
	M2,1,
	M1,1,
	L6,2,
	
	M2,2,
	M3,2,
	M1,2,
	M3,1,
	M2,1,
	M2,2,		
	M3,1,
	M1,1,
	M1,2+1,
	M1,1,
	
	//3
	L7,2+1,
	L5,1,
	L5,2+1,
	L5,1,
	M3,1,
	M2,1,
	M2,1,
	M1,1,
	M2,1,
	L5,2+1,
	
	P,2,
	M2,1,
	M3,1,
	M1,2+1,
	L6,1,	
	P,2+1,
	M3,1,
	M2,1,
	M1,1,
	L6,1,
	L5,1,
	
	M2,2,
	M3,1,
	M5,1+4,	
	P,2+1,
	M3,1,
	M2,1,
	M1,1,
	L6,1,
	L5,1,
	
	//4
	M2,2,
	M3,1,
	M1,1,
	M1,2,
	L6,1,
	L7,1,	
	M1,2,
	M3,1,
	M2,1+1,
	M1,1,
	M1,2,
	
	M1,2,
	M3,2,
	M3,4,
	P,1,
	M1,1,
	M2,1,
	M1,1,
	M3,1,
	M2,1,
	M1,1,
	M6,1+1,
	
	M5,1,
	M5,2,
	M5,1,
	M3,2,
	M2,1,
	M2,2+1,
	M3,1,
	M2,1,
	M1,1,
	M1,1,
	M7,1,
	
	//5
	M5,2,
	M2,1,
	M3,1,
	M2,2,
	M1,1,
	L6,1+1,
	M1,2+1,
	L5,1,
	L6,1,
	M1,1,
	M5,1+1,
	
	M3,1,
	M3,1,
	M2,1,	
	M2,1,
	M1,1,
	M1,1,
	M6,1,
	M6,1,
	M5,1,
	M5,1,
	M5,1,
	M2,1,
	M2,1,
	M1,1,
	M1,1,
	
	//6
	M2,4+4,
		
	0xff
	};//第一个数为音符，第二个数为分音符（时长）1为16分音符，2为8分音符，4为4分音符，8为2分音符，16为1分音符
//最后要有终止判断，不然会数值越界乱响，ROM有大小限制，如果超出限制，会从数组开头开始（有0xff也停不下来）
//一首歌可以无限循环	
	
unsigned char code Music2[]={
		
	P,4,
	M3,1,
	M4,1,
	M5,1,
	M5,1,
	
	M5,2,
	M1,1,
	M1,1,
	M1,2,
	L6,1,
	M2,1,
	M2,1,
	M3,2+1,
	M7,1,
	M1,1,
	M2,1,
	M2,1,
	
	M2,2,
	M1,1,
	L7,1,
	L7,2,
	M5,1,
	M2,1,
	M2,1,
	M1,2+1,
	M3,1,
	M4,1,
	M5,1,
	M5,1,
	
	//7
	M5,2,
	M1,1,
	M1,1,
	M1,2,
	M1,1,
	M2,1,
	M2,2,
	M1,1,
	L7,1,
	L1,1,
	L5,2+1,
	
	M4,2,
	M3,1,
	M3,1,
	M3,2,
	M2,1,
	M3,1,
	P,4,
	M3,1,
	M4,1,
	M5,1,
	M5,1,
	
	M5,2,
	M1,1,
	M1,1,
	M1,2,
	L6,1,
	M2,1,
	M2,1,
	M3,2+1,
	M7,1,
	M1,1,
	M2,1,
	M2,1+2,
	
	//8
	M1,1,
	M7,1,
	L7,2,
	L5,1,
	M2,1,
	M2,1,
	M1,2+1,
	M5,1,
	M4,1,
	M3,1,
	M4,1+2,
	
	M3,1,
	M4,1,
	M4,2,
	M1,1,
	M3,1,
	M3,2,
	M1,2+2,
	L7,1,
	M1,1+4+4,
	
	P,4+4,
	
		
	0xff
	};//第一个数为音符，第二个数为分音符（时长）1为16分音符，2为8分音符，4为4分音符，8为2分音符，16为1分音符

unsigned char code Music3[]={
//	//9
//	P,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	P,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	
//	P,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	P,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	P,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	M2,1,
//	M3,1,
//	M1,2,
//	
//	//10
//	P,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	P,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	
//	P,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	P,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	P,1,
//	M1,1,
//	M6,1,
//	M1,1,
//	M2,1,
//	M3,1,
//	M1,2,
//	
//	//11
//	P,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M5,1,
//	M5,2,
//	P,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M1,1,
//	M5,1,
//	M1,1,
//	M1,1+4+4,
		
	0xff
	};//第一个数为音符，第二个数为分音符（时长）1为16分音符，2为8分音符，4为4分音符，8为2分音符，16为1分音符

	
void Timer0_Routine() interrupt 1
{
	if(Frequency_Select)//判断是否为休止符
	{
		TH0=Frequency_Table[Frequency_Select]/256;		//重装初值
		TL0=Frequency_Table[Frequency_Select]%256;		
		Buzzer=~Buzzer;
	}	
	else
	{
		TH0=Frequency_Table[Frequency_Select]/256;		//重装初值
		TL0=Frequency_Table[Frequency_Select]%256;		
		Buzzer=0;
	}
}

void main ()
{
	LED_Close();
	Buzz_Close();
	Timer0Init();
	P2&=0x1F;
	P2|=0xA0;
	while(1)
	{
		if(Music[Music_Select]!=0xff)//判断是否播放完
		{
			Frequency_Select=Music[Music_Select];
			Music_Select++;
			Delay(SPEED/4*Music[Music_Select]);
			Music_Select++;
			TR0=0;//两个音连在一起需要停顿,先关闭定时器，再把定时器打开
			Delay(5);
			TR0=1;
		}
		else
		{
			if(Music2[Music_Select2]!=0xff)//判断是否播放完
			{
				Frequency_Select=Music2[Music_Select2];
				Music_Select2++;
				Delay(SPEED/4*Music2[Music_Select2]);
				Music_Select2++;
				TR0=0;//两个音连在一起需要停顿,先关闭定时器，再把定时器打开
				Delay(5);
				TR0=1;
			}
			else
			{
				if(Music3[Music_Select3]!=0xff)//判断是否播放完
				{
					Frequency_Select=Music3[Music_Select3];
					Music_Select3++;
					Delay(SPEED/4*Music3[Music_Select3]);
					Music_Select3++;
					TR0=0;//两个音连在一起需要停顿,先关闭定时器，再把定时器打开
					Delay(5);
					TR0=1;
				}
				else
				{
					TR0=0;
					while(1)
					Buzzer=0;//关闭蜂鸣器
				}
			}
		}
	}
}