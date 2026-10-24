#include <STC15F2K60S2.H>

code unsigned char Seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};
unsigned char Temp[]={11,11,11,11,11,11,11,11};

unsigned char Compare=3;

unsigned int FData;

void Init()
{
	P2=((P2&0x1f)|0x80);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&0x1f)|0xA0);
	P0=0x00;
	P2&=0x1f;
}

void Nixie()
{
	static unsigned char Bit;
	P2=((P2&0x1f)|0xe0);
	P0=0xff;
	P2&=0x1f;
	P2=((P2&0x1f)|0xc0);
	P0=0x01<<Bit;
	P2&=0x1f;
	P2=((P2&0x1f)|0xe0);
	P0=Seg[Temp[Bit]];
	P2&=0x1f;
	if(++Bit>=8)
	Bit=0;
}

void Timer0Init(void)		//65536??@12.000MHz
{
	AUXR &= 0x7F;		//?????12T??
	TMOD &= 0xF0;		//???????
	TMOD |= 0x05;		//???????
	TL0 = 0x00;		//??????
	TH0 = 0x00;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
}

void Timer1Init(void)		//2??@12.000MHz
{
	AUXR |= 0x40;		//?????1T??
	TMOD &= 0x0F;		//???????
	TL1 = 0x40;		//??????
	TH1 = 0xA2;		//??????
	TF1 = 0;		//??TF1??
	TR1 = 1;		//???1????
	EA=1;
	ET1=1;
}

void Timer1_Loop() interrupt 3
{
	static unsigned char Count0,Count1;
	Nixie();
	if(++Count0>=250)
	{
		Count0=0;
		FData=TH0<<8|TL0;
		FData*=2;
		TH0=0;
		TL0=0;
	}
	Count1++;
	if(Count1<=Compare)
	{
		P2=((P2&0x1f)|0x80);
		P0=0x00;
		P2&=0x1f;
	}
	if(Count1>Compare)
	{
		P2=((P2&0x1f)|0x80);
		P0=0xff;
		P2&=0x1f;
	}
	Count1%=5;
}

void main()
{
	Init();
	Timer0Init();
	Timer1Init();
	while(1)
	{
		Temp[3]=FData/10000;
		Temp[4]=FData%10000/1000;
		Temp[5]=FData%10000%1000/100;
		Temp[6]=FData%10000%1000%100/10;
		Temp[7]=FData%10;
	}
}


