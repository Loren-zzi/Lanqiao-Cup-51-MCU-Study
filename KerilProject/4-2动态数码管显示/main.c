#include <REGX52.H>


unsigned char NixieTable[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x7F,0x6F,0x77,
0x7C,0x39,0x5E,0x79,0x71,0x00,};

void Delay(unsigned int xms )		//@11.0592MHz
{
	unsigned char i, j;
			while(xms--)
			{
				i = 2;
				j = 15;
				do
				{
					while (--j);
				} while (--i);
			}
}

void Nixie(unsigned char Location, unsigned char Number)
{
			switch(Location)
			{
				case 1:P2_4=1;P2_3=1;P2_2=1;break;
				case 2:P2_4=1;P2_3=1;P2_2=0;break;
				case 3:P2_4=1;P2_3=0;P2_2=1;break;
				case 4:P2_4=1;P2_3=0;P2_2=0;break;
				case 5:P2_4=0;P2_3=1;P2_2=1;break;
				case 6:P2_4=0;P2_3=1;P2_2=0;break;
				case 7:P2_4=0;P2_3=0;P2_2=1;break;
				case 8:P2_4=0;P2_3=0;P2_2=0;break;
}
					P0=NixieTable[Number];
					Delay(2);
					P0=0x00;
					

}


void main()
{
			
				while(1)
				{
					
					Nixie(1,1);
					Nixie(2,3);
					Nixie(3,1);
					Nixie(4,4);
			//		Nixie(4,4);
					Nixie(6,5);
					Nixie(7,2);
					Nixie(8,0);
				
				}
}
