// DS18B20 单总线驱动的函数声明；具体时序见 onewire.c。
#ifndef __ONEWIRE_H_
#define __ONEWIRE_H_


void Write_DS18B20(unsigned char dat);
unsigned char Read_DS18B20(void);
bit init_ds18b20(void);




#endif  


