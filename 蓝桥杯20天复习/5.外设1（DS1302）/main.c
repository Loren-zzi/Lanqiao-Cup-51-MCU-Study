// 本例主题：DS1302 时钟与日期显示。

#include <STC15F2K60S2.h>
#include <DS1302.h>

//======================数码管显示数组
// seg 是段码表，数组下标对应要显示的数字或符号。
unsigned char seg[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xff};//0~9 - 熄灭
// Temp 是 8 位显示缓冲区；主循环写数据，定时器逐位点亮数码管。
unsigned char Temp[]={11,11,11,11,11,11,11,11};								//数码管缓存，初始是11是为了让数码管开始第一次是熄灭状态
unsigned char Time[]={0x24,0x03,0x20,0x20,0x45,0x04,0x04};			//时间数组，将读取到的时间放到数组内，用于数码管显示调用（用于转接给Temp）

//==========================DS1302数组
code unsigned char TimeWCommand[]={0x8c,0x88,0x86,0x84,0x82,0x80,0x8a};//年月日时分秒 星期  DS1302的地址
code unsigned char TimeRCommand[]={0x8D,0x89,0x87,0x85,0x83,0x81,0x8b};
#define WP 0x8E   // 写保护寄存器地址：写入 0x00 允许修改时间，写入 0x80 禁止修改

//==========================时间，日期状态量   
bit SJState=1,RQState=0;

//==============================
unsigned char key;

//====================================================

// 初始化 LED、蜂鸣器和继电器，让上电状态明确。
void Init()
{
	P2=((P2&0x1f)|0xa0);
	P0=0x00;							//BUzzer
	P2&=0x1f;
	P2=((P2&0x1f)|0x80);
	P0=0xff;						//蜂鸣器
	P2&=0x1f;								//清高三位
}

//===================== Nixie
// 动态扫描数码管：先消影，再选当前位，最后送出 Temp 中的段码。
void Nixie()
{
	static unsigned char Bit;
	P2=((P2&0x1f)|0xe0);				//开7，段选，请零，消影
	P0=0xff;
	P2&=0x1f;
	
	P2=((P2&0x1f)|0xc0);				//开6，位选
	P0=0x01<<Bit;
	P2&=0x1f;
	
	P2=((P2&0x1f)|0xe0);
	P0=seg[Temp[Bit]];
	P2&=0x1f;
	if(++Bit>=8)//				七次
		Bit=0;
}

//===========================扫描第一列//======================松手检验，循环扫描，物理消抖

// 读取当前按键状态；本例只用到需要的那一列。
unsigned char Mkey_State()
{
	unsigned char Mkey=0;
	P44=0;
	P42=1;
	P35=1;
	P34=1;
	P3|=0x0f;
	if(P32==0)
		Mkey=2;
	return Mkey;
}

// 比较前后两次按键状态，松手时把键号交给主循环。
void Mkey_Loop()
{
	static unsigned char NowState,LastState;
	LastState=NowState;
	NowState=Mkey_State();
	if(LastState==2&&NowState==0)
		key=2;
}

//========================时间设置
// DS1302 使用 BCD 时间；按地址依次写入年月日、时分秒和星期。
void Time_Set()
{
	unsigned char i;
	Write_Ds1302_Byte( WP,0x00 );// 关闭写保护，允许写入
	for(i=0;i<=6;i++)
	{Write_Ds1302_Byte(TimeWCommand[i],Time[i]);}
	Write_Ds1302_Byte( WP,0x80 );// 开启写保护，防止误改

}
//======================读取DS1302时间
// 按读取地址取得 DS1302 时间，结果仍是 BCD 格式。
void Time_Read()
{
	unsigned char i;
	for(i=0;i<=6;i++)
	Time[i]=Read_Ds1302_Byte(TimeRCommand[i]);

}
//==============================定时器
// 配置定时器 0 的 2 ms 中断，用来提供数码管扫描节拍。
void Timer0Init(void)		// 2 ms @ 12 MHz
{
	AUXR |= 0x80;		// 定时器 0 使用 1T 模式
	TMOD &= 0xF0;		// 定时器 0 设为 16 位定时模式
	TL0 = 0x40;		// 2 ms 定时初值的低字节
	TH0 = 0xA2;		// 2 ms 定时初值的高字节
	TF0 = 0;		// 清除定时器 0 溢出标志
	TR0 = 1;		// 启动定时器 0
	ET0=1;
	EA=1;
}

// 每 2 ms 刷新一位数码管；每 20 ms 扫描一次按键。
void Timer0_Loop()   interrupt 1     //2ms
{
	static unsigned char Count0;
	Nixie();
	if(++Count0==10)
	{
		Count0=0;
		Mkey_Loop();
	}
}

//========================时间显示
// 把时、分、秒的 BCD 高低位分别放进 8 位显示缓冲区。
void SJ_Loop()
{
	Temp[0]=Time[3]/16;
	Temp[1]=Time[3]%16;
	
	Temp[2]=10;
	
	Temp[3]=Time[4]/16;
	Temp[4]=Time[4]%16;
	
	Temp[5]=10;
	
	Temp[6]=Time[5]/16;
	Temp[7]=Time[5]%16;
}

//===================日期显示
// 把年、月、日的 BCD 高低位分别放进 8 位显示缓冲区。
void RQ_Loop()
{
	Temp[0]=Time[0]/16;
	Temp[1]=Time[0]%16;
	Temp[2]=10;
	Temp[3]=Time[1]/16;
	Temp[4]=Time[1]%16;
	Temp[5]=10;
	Temp[6]=Time[2]/16;
	Temp[7]=Time[2]%16;
}

//======================
// 初始化并设置时间；主循环读取时钟、处理切换键，再显示时间或日期。
void main()
{
	Init();
	Time_Set();
	Timer0Init();
	
		while(1)
		{
			Time_Read();
			switch(key)
			{
				case 2:
					key=0;
				SJState=!SJState;
				RQState=!RQState;
//					if(SJState)
//				{
//					SJState=0;
//					RQState=1;
//				}
//				else
//				{
//					SJState=1;
//					RQState=0;
//				}

				break;
			}
			if(SJState)	SJ_Loop();
			if(RQState)	RQ_Loop();
			
			}


}