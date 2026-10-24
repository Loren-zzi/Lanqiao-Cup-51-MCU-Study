#include <STC15F2K60S2.H>
#include "intrins.h"

sbit OneWire_DQ=P1^4;

//unsigned char i, j;

unsigned char OneWire_Init(void)
{
	unsigned char i, j;
	unsigned char ACKBit;
	OneWire_DQ=1;
	OneWire_DQ=0;
	_nop_();
	_nop_();
	i = 6;
	j = 93;
	do
	{
		while (--j);
	} while (--i);//Delay500us
	OneWire_DQ=1;
	_nop_();
	_nop_();
	i = 1;
	j = 189;
	do
	{
		while (--j);
	} while (--i);//Delay70us
	ACKBit=OneWire_DQ;
	_nop_();
	_nop_();
	i = 6;
	j = 93;
	do
	{
		while (--j);
	} while (--i);//Delay500us
	return ACKBit;
}

void OneWire_SendBit(unsigned char Bit)
{
	unsigned char i, j;
	OneWire_DQ=0;
	_nop_();
	i = 25;
	while (--i);//10us
	OneWire_DQ=Bit;
	i = 1;
	j = 148;
	do
	{
		while (--j);
	} while (--i);//55us
	OneWire_DQ=1;
}

unsigned char OneWire_ReceiveBit(void)//5+7+52不行，好像跳过了读取环节，记得上下拉也需要时间
{//5+5+55
	unsigned char i, j;	
	unsigned char Bit;
	OneWire_DQ=0;
	_nop_();
	i = 11;
	while (--i);//5us
	OneWire_DQ=1;
	i = 11;
	while (--i);//5us
	Bit=OneWire_DQ;
	i = 1;
	j = 142;
	do
	{
		while (--j);
	} while (--i);//55us
	return Bit;
}

void OneWire_SendByte(unsigned char Byte)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		OneWire_SendBit(Byte&(0x01<<i));
	}
}

unsigned char OneWire_ReceiveByte(void)
{
	unsigned char i,Byte=0x00;
	for(i=0;i<8;i++)
	{
		if(OneWire_ReceiveBit())
		{
			Byte|=(0x01<<i);
		}
	}
	return Byte;
}
