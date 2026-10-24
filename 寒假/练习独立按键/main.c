#include <REGX52.H>

//void Delay(unsigned int xms)		
//{
//	while(xms)
//	{unsigned char i, j;

//	i = 11;
//	j = 190;
//	do
//	{
//		while (--j);
//	} while (--i);
//	xms--;
//	}
//}

//void main()
//{      
////			unsigned char LED=0;
//			while(1)
//			{  
//				if(P3_0==0)
//			{
//					Delay(20);   //消抖
//					while(P3_0==0); //松手检验
//					Delay(20);     //消抖
//			    P2--;
//					P2=P2--;
////			    LED++;
////					P2=~LED;
//				
//			}
//			
//			}




//}
void Delay(unsigned int xms)
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
	
	  while(1)
		{
			
			if(P3_1==0)
			{		Delay(20);
					while(P3_1==0);
					Delay(20);
				P2=~(0x01<<led);
				led++;
							if(led==8)
							{
								led=1;
							}
				
			
			
			}
				



		}

	}