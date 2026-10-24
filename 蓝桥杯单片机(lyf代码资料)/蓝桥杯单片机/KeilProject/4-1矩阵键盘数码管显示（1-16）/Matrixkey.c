#include <STC15F2K60S2.H>
#include "Delay.h"

sbit List_1=P4^4;
sbit List_2=P4^2;
sbit List_3=P3^5;
sbit List_4=P3^4;
sbit Line_1=P3^3;
sbit Line_2=P3^2;
sbit Line_3=P3^1;
sbit Line_4=P3^0;

unsigned char MKeyNum;

unsigned char MKey(void)
{
	unsigned char Temp=0;
	Temp=MKeyNum;
	MKeyNum=0;
	return Temp;
}

unsigned char Matrixkey_State()
{
	unsigned char Key=0;
	List_1=0;
	List_2=1;
	List_3=1;
	List_4=1;
	Line_1=1;
	Line_2=1;
	Line_3=1;
	Line_4=1;
	if(Line_1==0)
	Key=1;		

	if(Line_2==0)
	Key=5;
	
	if(Line_3==0)
	Key=9;
	
	if(Line_4==0)
	Key=13;
	
	List_1=1;
	List_2=0;
	List_3=1;
	List_4=1;
	Line_1=1;
	Line_2=1;
	Line_3=1;
	Line_4=1;

	if(Line_1==0)
	Key=2;		

	if(Line_2==0)
	Key=6;

	if(Line_3==0)
	Key=10;
	
	if(Line_4==0)
	Key=14;
	
	List_1=1;
	List_2=1;
	List_3=0;
	List_4=1;
	Line_1=1;
	Line_2=1;
	Line_3=1;
	Line_4=1;
	if(Line_1==0)
	Key=3;		
	
	if(Line_2==0)
	Key=7;
	
	if(Line_3==0)
	Key=11;

	if(Line_4==0)
	Key=15;
	
	List_1=1;
	List_2=1;
	List_3=1;
	List_4=0;
	Line_1=1;
	Line_2=1;
	Line_3=1;
	Line_4=1;
	if(Line_1==0)
	Key=4;		
	
	if(Line_2==0)
	Key=8;
	
	if(Line_3==0)
	Key=12;
	
	if(Line_4==0)
	Key=16;
	
	return Key;
}

void MKey_Loop(void)
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Matrixkey_State();
	if(NowState==0 && LastState==1)
	MKeyNum=1;
	if(NowState==0 && LastState==2)
	MKeyNum=2;
	if(NowState==0 && LastState==3)
	MKeyNum=3;
	if(NowState==0 && LastState==4)
	MKeyNum=4;
	if(NowState==0 && LastState==5)
	MKeyNum=5;
	if(NowState==0 && LastState==6)
	MKeyNum=6;
	if(NowState==0 && LastState==7)
	MKeyNum=7;
	if(NowState==0 && LastState==8)
	MKeyNum=8;
	if(NowState==0 && LastState==9)
	MKeyNum=9;
	if(NowState==0 && LastState==10)
	MKeyNum=10;
	if(NowState==0 && LastState==11)
	MKeyNum=11;
	if(NowState==0 && LastState==12)
	MKeyNum=12;
	if(NowState==0 && LastState==13)
	MKeyNum=13;
	if(NowState==0 && LastState==14)
	MKeyNum=14;
	if(NowState==0 && LastState==15)
	MKeyNum=15;
	if(NowState==0 && LastState==16)
	MKeyNum=16;
}