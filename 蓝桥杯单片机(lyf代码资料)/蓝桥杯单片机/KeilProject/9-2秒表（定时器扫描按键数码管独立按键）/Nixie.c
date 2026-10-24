#include <STC15F2K60S2.H>
#include "Delay.h"

unsigned char Nixie_Temp[9]={0,10,10,10,10,10,10,10,10};//暂时数组,设置值用
unsigned char Segment_Selection[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00,0x40};
unsigned char Bit_Selection[]={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};

void Nixie(unsigned char Bit,Seg)
{
	Nixie_Temp[Bit]=Seg;
}

void Nixie_Init(unsigned char Bit,Seg)
{
	Bit--;
	P2&=0x1f;
	P2|=0xc0;
	P0=~Bit_Selection[Bit];  
	P2&=0x1f;
	P2&=0x1f;
	P2|=0xe0;
	P0=~Segment_Selection[Seg];  
	P2&=0x1f;
//P0=0x00;      //消隐
}

void Nixie_Loop()
{
	static unsigned char i=1;
	Nixie_Init(i,Nixie_Temp[i]);
	i++;
	if(i>=9)
	i=1;	
}
