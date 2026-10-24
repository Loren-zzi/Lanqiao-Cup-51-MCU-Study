#include <STC15F2K60S2.H>
#include "iic.h"
#include "onewire.h"

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,//0~9 0.0~0.9 -灭
													0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x08,0x00,0x10,0xBF,0xff};//段选
unsigned char Temp[]={21,21,21,21,21,21,21,21};//8位数码管0~7，位选

unsigned char KeyNum,MkeyNum;//分别与Key_Loop()，Mkey_Loop()有关;

unsigned char AT24C02Data,ADCData;

int TData;

void Init()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;
	P2&=0x1f;//前三行关闭继电器
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;//关闭LED
}

void Nixie()//数码管
{
	static unsigned char Bit;
	P2=((P2&0x1f)|0xe0);
	P0=0xff;
	P2&=0x1f;//消隐
	P2=((P2&0x1f)|0xc0);
	P0=0x01<<Bit;
	P2&=0x1f;//位选
	P2=((P2&0x1f)|0xe0);
	P0=Seg[Temp[Bit]];
	P2&=0x1f;//段选
	if(++Bit>=8)
	Bit=0;
}

unsigned char Key_State()//独立按键按下的状态 从下到上为1~4
{
	unsigned char Key=0;
	if(P33==0)
	Key=1;
	if(P32==0)
	Key=2;
	if(P31==0)
	Key=3;
	if(P30==0)
	Key=4;
	return Key;
}

void Key_Loop() 
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Key_State();
	if(LastState==1&&NowState==0)//检测按键为松手响应
	KeyNum=1;
	if(LastState==2&&NowState==0)
	KeyNum=2;
	if(LastState==3&&NowState==0)
	KeyNum=3;
	if(LastState==4&&NowState==0)
	KeyNum=4;
//	if(LastState==0&&NowState==1)//检测按键为按下响应
//	KeyNum=1;
//	if(LastState==0&&NowState==2)
//	KeyNum=2;
//	if(LastState==0&&NowState==3)
//	KeyNum=3;
//	if(LastState==0&&NowState==4)
//	KeyNum=4;
}

unsigned char Mkey_State()//矩阵键盘按键状态 
{                         //13 14 15 16
	unsigned char Mkey=0;		//9  10 11 12
	P44=0;                  //5  6  7  8
	P42=1;									//1  2  3  4
	P35=1;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=1;
	if(P32==0)
	Mkey=5;
	if(P31==0)
	Mkey=9;
	if(P30==0)
	Mkey=13;
	
	P44=1;
	P42=0;
	P35=1;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=2;
	if(P32==0)
	Mkey=6;
	if(P31==0)
	Mkey=10;
	if(P30==0)
	Mkey=14;
	
	P44=1;
	P42=1;
	P35=0;
	P34=1;
	P3|=0x0f;
	if(P33==0)
	Mkey=3;
	if(P32==0)
	Mkey=7;
	if(P31==0)
	Mkey=11;
	if(P30==0)
	Mkey=15;
	
	P44=1;
	P42=1;
	P35=1;
	P34=0;
	P3|=0x0f;
	if(P33==0)
	Mkey=4;
	if(P32==0)
	Mkey=8;
	if(P31==0)
	Mkey=12;
	if(P30==0)
	Mkey=16;
	return Mkey;
}

void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(NowState==0&&LastState==1)//按键松手响应
	MkeyNum=1;
	if(NowState==0&&LastState==2)
	MkeyNum=2;
	if(NowState==0&&LastState==3)
	MkeyNum=3;
	if(NowState==0&&LastState==4)
	MkeyNum=4;
	if(NowState==0&&LastState==5)
	MkeyNum=5;
	if(NowState==0&&LastState==6)
	MkeyNum=6;
	if(NowState==0&&LastState==7)
	MkeyNum=7;
	if(NowState==0&&LastState==8)
	MkeyNum=8;
	if(NowState==0&&LastState==9)
	MkeyNum=9;
	if(NowState==0&&LastState==10)
	MkeyNum=10;
	if(NowState==0&&LastState==11)
	MkeyNum=11;
	if(NowState==0&&LastState==12)
	MkeyNum=12;
	if(NowState==0&&LastState==13)
	MkeyNum=13;
	if(NowState==0&&LastState==14)
	MkeyNum=14;
	if(NowState==0&&LastState==15)
	MkeyNum=15;
	if(NowState==0&&LastState==16)
	MkeyNum=16;
//	if(LastState==0&&NowState==1) 以下为按键按下相响应
//	MkeyNum=1;
//	if(LastState==0&&NowState==2)
//	MkeyNum=2;
//	if(LastState==0&&NowState==3)
//	MkeyNum=3;
//	if(LastState==0&&NowState==4)
//	MkeyNum=4;
//	if(LastState==0&&NowState==5)
//	MkeyNum=5;
//	if(LastState==0&&NowState==6)
//	MkeyNum=6;
//	if(LastState==0&&NowState==7)
//	MkeyNum=7;
//	if(LastState==0&&NowState==8)
//	MkeyNum=8;
//	if(LastState==0&&NowState==9)
//	MkeyNum=9;
//	if(LastState==0&&NowState==10)
//	MkeyNum=10;
//	if(LastState==0&&NowState==11)
//	MkeyNum=11;
//	if(LastState==0&&NowState==12)
//	MkeyNum=12;
//	if(LastState==0&&NowState==13)
//	MkeyNum=13;
//	if(LastState==0&&NowState==14)
//	MkeyNum=14;
//	if(LastState==0&&NowState==15)
//	MkeyNum=15;
//	if(LastState==0&&NowState==16)
//	MkeyNum=16;
}

void Timer0Init(void)		//2??@12.000MHz定时器0 16位自动重装 1T模式
{
	AUXR |= 0x80;		//?????1T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x40;		//??????
	TH0 = 0xA2;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	ET0=1;//关键
	EA=1;//关键
}

void AT24C02_Write(unsigned char Command,Data)//AT24C02数据存储芯片 写数据
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(Command);
	IIC_WaitAck();
	IIC_SendByte(Data);
	IIC_WaitAck();
	IIC_Stop();
}

unsigned char AT24C02_Read(unsigned char Command)//AT24C02数据存储芯片 读数据
{
	unsigned char Data;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(Command);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0xa0|0x01);
	IIC_WaitAck();
	Data=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return Data;
}

unsigned char ADC_Read()//用PCF8591读取电压值 返回0~255
{
	unsigned char Data;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0x90|0x01);
	IIC_WaitAck();
	Data=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();	
	return Data;
}

unsigned char GM_Read()//用PCF8591读取光敏电阻值
{
	unsigned char Data;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0x90|0x01);
	IIC_WaitAck();
	Data=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();	
	return Data;
}

void DAC_Write(unsigned char Data)//DAC在P3^5输出电压值 写入0~255
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x40);
	IIC_WaitAck();
	IIC_SendByte(Data);
	IIC_WaitAck();
	IIC_Stop();
}

int DS18B20_Read()//读取温度传感器的值
{
	unsigned char Low,High;
	int TEMP;
	int T;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	Low=Read_DS18B20();
	High=Read_DS18B20();
	TEMP=Low|(High<<8);
	T=TEMP/16.0;//后四位为小数部分
	return T;
}

void Timer0_Loop() interrupt 1 //定时器0中断服务函数 2ms
{
	static unsigned char Count0,Count1;
	Nixie();
	if(++Count0>=10)//20ms扫描一次独立按键
	{
		Count0=0;
		Key_Loop();
	}
	if(++Count1>=10)////20ms扫描一次矩阵键盘
	{
		Count1=0;
		Mkey_Loop();
	}
}

void main()
{
	Init();
	Timer0Init();
	while(1)
	{
		AT24C02Data=AT24C02_Read(0);
		ADCData=ADC_Read();
		TData=DS18B20_Read();
		switch(KeyNum)//独立按键
		{
			case 1:
			{
				KeyNum=0;//每个按键不管执行什么操作都要先清0
			}break;
			case 2:
			{
				KeyNum=0;//每个按键不管执行什么操作都要先清0
			}break;//往下类推
		}
		switch(MkeyNum)
		{
			case 1:
			{
				MkeyNum=0;//每个按键不管执行什么操作都要先清0
			}break;
			case 2:
			{
				MkeyNum=0;//每个按键不管执行什么操作都要先清0
			}break;//依次类推
		}
	}
}