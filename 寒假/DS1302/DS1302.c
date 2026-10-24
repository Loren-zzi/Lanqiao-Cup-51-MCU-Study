#include <REGX52.H>

sbit DS1302_SCLK=P3^6;
sbit DS1302_IO=P3^4;
sbit DS1302_CE=P3^5;

#define DS1302_SECOND		0x80
#define DS1302_MINUTE		0x82
#define DS1302_HOUR			0x84
#define DS1302_DATE			0x86
#define DS1302_MONTH		0x88
#define DS1302_DAY			0x8A
#define DS1302_YEAR			0x8C
#define DS1302_WP				0x8E //手写保护

unsigned char DS1302_Time[]={25,1,28,15,37,11,2};

void DS1302_Init(void)
{
	DS1302_CE=0;
	DS1302_SCLK=0;
}

void DS1302_WriteByte(unsigned char command,unsigned char Data)
{
	unsigned char i;
   DS1302_CE=1;
	for(i=0;i<8;i++)
	{
			DS1302_IO=command&(0x01<<i);
			DS1302_SCLK=1;
			DS1302_SCLK=0;
	}

		for(i=0;i<8;i++)
	{
			DS1302_IO=Data&(0x01<<i);
			DS1302_SCLK=1;
			DS1302_SCLK=0;
	}
			DS1302_CE=0;
}
unsigned char DS1302_ReadByte(unsigned char command)
{
		unsigned char i,Data=0x00;
		command|=0x01;
		DS1302_CE=1;
	for(i=0;i<8;i++)
	{
			DS1302_IO=command&(0x01<<i);
			DS1302_SCLK=0;
			DS1302_SCLK=1;
	}
  DS1302_IO=1;
	for(i=0;i<8;i++)
		{
				DS1302_SCLK=1;
				DS1302_SCLK=0;
				if(DS1302_IO){Data|=(0x01<<i);}
		}
				DS1302_CE=0;
				DS1302_IO=0;	//读取后将IO设置为0，否则读出的数据会出错
				return Data;


}

void DS1302_SetTime()
{
				DS1302_WriteByte(DS1302_WP,0x00);
				DS1302_WriteByte(DS1302_SECOND,DS1302_Time[5]/16*10+DS1302_Time[5]%16);
				DS1302_WriteByte(DS1302_WP,0x00);

}
void DS1302_ReadTime()
{
				unsigned char Temp;
				Temp=DS1302_ReadByte(DS1302_SECOND);
				DS1302_Time[5]=Temp/16*10+Temp%16;
				
}