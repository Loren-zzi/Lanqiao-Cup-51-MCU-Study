#include "STC15F2K60S2.h"

sbit List_1=P4^4;
sbit List_2=P4^2;
sbit List_3=P3^5;
sbit List_4=P3^4;
sbit Line_1=P3^3;
sbit Line_2=P3^2;
sbit Line_3=P3^1;
sbit Line_4=P3^0;

extern unsigned char MkeyNum;

unsigned char Mkey_State()		//∫·…®√Ë
{
	unsigned char Mkey=0;	
	List_1=0;
	List_2=1;
	List_3=1;
	List_4=1;
	P3|=0x0f;       		//∑¿÷πµÕÀƒŒª∫·œﬂ”∞œÏ
	if(Line_1==0)
	Mkey=1;
	if(Line_2==0)
	Mkey=5;
	if(Line_3==0)
	Mkey=9;
	if(Line_4==0)
	Mkey=13;

	List_1=1;
	List_2=0;
	List_3=1;
	List_4=1;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=2;
	if(Line_2==0)
	Mkey=6;
	if(Line_3==0)
	Mkey=10;
	if(Line_4==0)
	Mkey=14;
	
	List_1=1;
	List_2=1;
	List_3=0;
	List_4=1;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=3;
	if(Line_2==0)
	Mkey=7;
	if(Line_3==0)
	Mkey=11;
	if(Line_4==0)
	Mkey=15;
	
	List_1=1;
	List_2=1;
	List_3=1;
	List_4=0;
	P3|=0x0f;
	if(Line_1==0)
	Mkey=4;
	if(Line_2==0)
	Mkey=8;
	if(Line_3==0)
	Mkey=12;
	if(Line_4==0)
	Mkey=16;
	return Mkey;
}

void Mkey_Loop(unsigned char *p)
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(NowState==0&&LastState==1)
	*p=1;
	if(NowState==0&&LastState==2)
	*p=2;
	if(NowState==0&&LastState==3)
	*p=3;
	if(NowState==0&&LastState==4)
	*p=4;
	if(NowState==0&&LastState==5)
	*p=5;
	if(NowState==0&&LastState==6)
	*p=6;
	if(NowState==0&&LastState==7)
	*p=7;
	if(NowState==0&&LastState==8)
	*p=8;
	if(NowState==0&&LastState==9)
	*p=9;
	if(NowState==0&&LastState==10)
	*p=10;
	if(NowState==0&&LastState==11)
	*p=11;
	if(NowState==0&&LastState==12)
	*p=12;
	if(NowState==0&&LastState==13)
	*p=13;
	if(NowState==0&&LastState==14)
	*p=14;
	if(NowState==0&&LastState==15)
	*p=15;
	if(NowState==0&&LastState==16)
	*p=16;
}
