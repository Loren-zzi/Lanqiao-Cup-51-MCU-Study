#include <REGX52.H>

void Delay(unsigned int xms)		//@11.0592MHz
{
			while(xms)
			{
	unsigned char i, j;

	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
			xms--;
			}
}

unsigned char Numbertable[]={0x3f,0x06,0x58,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

void Nixie(unsigned char Location,Number)
{
			switch(Location)
			{	
				case 1:P2_4=1;P2_3=1;P2_2=1;break;//y7
				case 2:P2_4=1;P2_3=1;P2_2=0;break;//y6
				case 3:P2_4=1;P2_3=0;P2_2=1;break;
				case 4:P2_4=1;P2_3=0;P2_2=0;break;
				case 5:P2_4=0;P2_3=1;P2_2=1;break;
				case 6:P2_4=0;P2_3=1;P2_2=0;break;
				case 7:P2_4=0;P2_3=0;P2_2=1;break;
				case 8:P2_4=0;P2_3=0;P2_2=0;break;
			}

			P0=Numbertable[Number];
			Delay(1);
			P0=0x00;
			

}


void main()
{
	
//		P2_4=1;
//		P2_3=1;
//		P2_2=1;
//		P0=0x06;
	while(1)
	{
					Nixie(1,1);
					Nixie(2,3);
					Nixie(3,1);
					Nixie(4,4);
	}

}