#ifndef __AT24C02_H__
#define __AT24C02_H__

void AT24C02_WriteByte(unsigned char WordAddress,Data);//0~255
unsigned char AT24C02_ReadByte(unsigned char WordAddress);

#endif
