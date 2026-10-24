// DS1302 时钟驱动的函数声明；具体时序见 ds1302.c。
#ifndef __DS1302_H_
#define __DS1302_H_


void Write_Ds1302(unsigned  char temp) ;
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )   ;  
unsigned char Read_Ds1302_Byte ( unsigned char address );



#endif