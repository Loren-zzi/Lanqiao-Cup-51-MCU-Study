#include <REGX52.H>


void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 5;
	j = 52;
	k = 195;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


//实现流水灯、
//0111 1110  7e
//1011 1101  bd 
//1101 1011  db
//1110 0111  e7
//1101 1011  db
//1011 1101  bd


//void main()
//{   while(1)
//	{
//    P2=0x7e;  
//   Delay100ms();
//	    P2=0xbd;  
//   Delay100ms();
//	    P2=0xdb;  
//   Delay100ms();
//	    P2=0xe7;  
//   Delay100ms();
//    P2=0xdb;  
//   Delay100ms();
//    P2=0xbd;  
//   Delay100ms();
//	}

//}

unsigned char led=0xfe;//1111 1110 
void main()
{
    while(1)
		{
     P2=led;
			Delay100ms();
			led=((led<<1)|(led>>7));//1111 1100|0000 0001=1111 1101
		
		
		}





}