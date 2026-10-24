#include <REGX52.H>

void Delay(unsigned int xms)	//@11.0592MHz
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

void main()
{	
	unsigned char led=0;
	P2=~0x01;
	while(1)
	{
					if(P3_1==0)
				{
						Delay(20);
						while(P3_1==0);
						Delay(20);
						led++; 
					if(led==8)
					 led=0;
					P2=~(0x01<<led);
				
				}
			 if(P3_0==0)
					{
						Delay(20);
						while(P3_0==0);
						Delay(20);
					  
					if(led==0)
							led=7;
					else led--;
						P2=~(0x01<<led);
						
				}					
		   
				
			}
}
