#include <STC15F2K60S2.H>

sbit DS1302_SCLK=P1^7;  //时序
sbit DS1302_IO=P2^3;    //输入输出
sbit DS1302_RST=P1^3;   //使能端口（开关）

#define WRITE    0x8E //0x8F   写保护，0可写，1不可写

unsigned char DS1302_Time[]={23,1,6,10,0,0,5};//年月日时分秒星期
code unsigned TimeWCommand[]={0x8c,0x88,0x86,0x84,0x82,0x80,0x8A};
code unsigned TimeRCommand[]={0x8D,0x89,0x87,0x85,0x83,0x81,0x8B};

//BCD码转十进制：DEC=BCD/16*10+BCD%16 （2位BCD）
//十进制转BCD码：BCD=DEC/10*16+DEC%10 （2位BCD）

void DS1302_Init()  //上电所有IO口为1，先置零
{
	DS1302_RST=0;
	DS1302_SCLK=0;
}

void DS1302_WriteByte(unsigned char Command,Data)//（命令字，数据）
{
	unsigned char i;
	DS1302_RST=1;
	for(i=0;i<8;i++)//有16个脉冲，所以DS1302_SCLK先给1再给0
	{                               //取出最后一位判断是输入还是读取 R/W 0为W 1为R
		DS1302_IO=Command&(0x01<<i);  //右边是一字节，左边是位，位判断：非零即1，0为0(懂了)
		DS1302_SCLK=1;
		DS1302_SCLK=0;
	}
	for(i=0;i<8;i++)
	{
		DS1302_IO=Data&(0x01<<i);
		DS1302_SCLK=1;
		DS1302_SCLK=0;
	}
	DS1302_RST=0;
}

unsigned char DS1302_ReadByte(unsigned char Command)//（命令字，数据）
{
	unsigned char i,Data=0x00;
	//Command|=0x01; //只用写上半部分宏定义 末位由0变1的方法
	DS1302_RST=1;
	for(i=0;i<8;i++)//有15个脉冲，所以DS1302_SCLK先给0再给1
	{
		DS1302_IO=Command&(0x01<<i);
		DS1302_SCLK=0;
		DS1302_SCLK=1;
	}

	for(i=0;i<8;i++)//一半脉冲卡在1，所以先给1，再给0和后半段时序完全重合，目的为最后DS1302_SCLK为0
	{
		DS1302_SCLK=1;
		DS1302_SCLK=0;
		if(DS1302_IO){Data|=(0x01<<i);}
	}
	DS1302_RST=0;
	DS1302_IO=0;//蓝桥杯不加会乱码，普中也会乱码。重要啊。
	return Data;//读出来的是用BCD码将十进制转化为二进制
}

void DS1302_SetTime(void)
{
	unsigned char i;
	DS1302_WriteByte(WRITE,0x00);
	for(i=0;i<+6;i++)
	DS1302_WriteByte(TimeWCommand[i],DS1302_Time[i]/10*16+DS1302_Time[i]%10);
	DS1302_WriteByte(WRITE,0x80);
}

void DS1302_ReadTime()
{
	unsigned char Read,i;
	for(i=0;i<=6;i++)
	{
		Read=DS1302_ReadByte(TimeRCommand[i]);
		DS1302_Time[i]=Read/16*10+Read%16;
	}
}

