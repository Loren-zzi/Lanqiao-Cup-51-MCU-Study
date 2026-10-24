#include <STC15F2K60S2.H>
#include "OneWire.h"

#define DS18B20_SKIP_ROM 0xcc   //跳过ROM
#define DS18B20_CONVERT_T 0x44  //转换温度
#define DS18B20_READ_SCRATCHPAD 0xbe  //读取存储器中的所有字节

void DS18B20_ConvertT(void)
{
	OneWire_Init();
	OneWire_SendByte(DS18B20_SKIP_ROM);
	OneWire_SendByte(DS18B20_CONVERT_T);
}

float DS18B20_ReadT(void)
{
	unsigned char TLSB,TMSB;//低八位 高八位
	int Temp;
	float T;
	OneWire_Init();
	OneWire_SendByte(DS18B20_SKIP_ROM);
	OneWire_SendByte(DS18B20_READ_SCRATCHPAD);
	TLSB=OneWire_ReceiveByte();
	TMSB=OneWire_ReceiveByte();
	Temp=(TMSB<<8)|TLSB;
	T=Temp/16.0;  //例如10001，转化为1.0001，除10/因为int类型储存的是二进制，后四位为小数，除以16.0，.0不损失精度
	//浮点数进行整数类型转换的时候，以十进制进行操作
	return T;
}
