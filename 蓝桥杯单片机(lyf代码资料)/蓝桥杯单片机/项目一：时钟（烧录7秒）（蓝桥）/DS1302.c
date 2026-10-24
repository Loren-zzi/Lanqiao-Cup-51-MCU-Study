#include <STC15F2K60S2.H>


sbit DS1302_SCLK=P1^7;  //时序
sbit DS1302_IO=P2^3;    //输入输出
sbit DS1302_RST=P1^3;   //使能端口（开关）

#define DS1302_SECOND         0x80 //0x81
#define DS1302_MINUTE         0x82 //0x83
#define DS1302_HOUR           0x84 //0x85
#define DS1302_DATE           0x86 //0x87
#define DS1302_MONTH          0x88 //0x89
#define DS1302_DAY            0x8A //0x8B
#define DS1302_YEAR           0x8C //0x8D
#define DS1302_WP             0x8E //0x8F   写保护，0可写，1不可写

#define DS1302_RSECOND        0x81
#define DS1302_RMINUTE        0x83
#define DS1302_RHOUR          0x85
#define DS1302_RDATE          0x87
#define DS1302_RMONTH         0x89
#define DS1302_RDAY           0x8B
#define DS1302_RYEAR          0x8D
#define DS1302_RWP            0x8F   //写保护，0可写，1不可写

signed char DS1302_Time[]={23,1,12,12,54,30,4};//年月日时分秒星期 改为char类型



//BCD码转十进制：DEC=BCD/16*10+BCD%16 （2位BCD）
//十进制转BCD码：BCD=DEC/10*16+DEC%10 （2位BCD）

void DS1302_Init()
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

unsigned char DS1302_ReadByte(unsigned char Command)//（命令字）
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
	DS1302_WriteByte(DS1302_WP,0x00);
	DS1302_WriteByte(DS1302_YEAR,DS1302_Time[0]/10*16+DS1302_Time[0]%10);
	DS1302_WriteByte(DS1302_MONTH,DS1302_Time[1]/10*16+DS1302_Time[1]%10);
	DS1302_WriteByte(DS1302_DATE,DS1302_Time[2]/10*16+DS1302_Time[2]%10);
	DS1302_WriteByte(DS1302_HOUR,DS1302_Time[3]/10*16+DS1302_Time[3]%10);
	DS1302_WriteByte(DS1302_MINUTE,DS1302_Time[4]/10*16+DS1302_Time[4]%10);
	DS1302_WriteByte(DS1302_SECOND,DS1302_Time[5]/10*16+DS1302_Time[5]%10);
	DS1302_WriteByte(DS1302_DAY,DS1302_Time[6]/10*16+DS1302_Time[6]%10);
	DS1302_WriteByte(DS1302_WP,0x80);
}

void DS1302_ReadTime()
{
	unsigned char Read;
	Read=DS1302_ReadByte(DS1302_RYEAR);
	DS1302_Time[0]=Read/16*10+Read%16;
	Read=DS1302_ReadByte(DS1302_RMONTH);
	DS1302_Time[1]=Read/16*10+Read%16;
	Read=DS1302_ReadByte(DS1302_RDATE);
	DS1302_Time[2]=Read/16*10+Read%16;
	Read=DS1302_ReadByte(DS1302_RHOUR);
	DS1302_Time[3]=Read/16*10+Read%16;
	Read=DS1302_ReadByte(DS1302_RMINUTE);
	DS1302_Time[4]=Read/16*10+Read%16;
	Read=DS1302_ReadByte(DS1302_RSECOND);
	DS1302_Time[5]=Read/16*10+Read%16;
	Read=DS1302_ReadByte(DS1302_RDAY);
	DS1302_Time[6]=Read/16*10+Read%16;
}

