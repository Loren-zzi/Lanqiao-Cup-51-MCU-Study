#include <REGX52.H>
sbit led=P2^0;


void Delay(unsigned int t)
{
	while(t--);
}
unsigned char time,i;


void main()
{
	led=1;
      while(1)
        {
					if(P3_1==0)
					{
						for(time=0;time<100;time++)//亮度由暗变亮
						{
							for(i=0;i<20;i++)//延长这个亮度时间达到变化平滑
							{
								led=0;
								Delay(time);
								led=1;
								Delay(100-time);
								if(P3_1==0);
							}
					
						
						for(time=100;time>0;time--)//亮度由亮变暗
						{
							for(i=0;i<20;i++)//延长这个亮度时间达到变化平滑
							{
								led=0;
								Delay(time);
								led=1;
								Delay(100-time);
							}
						}
					}


					
				}
					
        }

}