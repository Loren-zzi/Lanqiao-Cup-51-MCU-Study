#include <STC15F2K60S2.h>

// list_1~list_4 是矩阵键盘的 4 根列线；扫描时每次只拉低一列。
sbit list_1=P4^4;
sbit list_2=P4^2;
sbit list_3=P3^5;
sbit list_4=P3^4;

// line_1~line_4 是 4 根行线；当前列中的某行读到 0，表示该交点的键被按下。
sbit line_1=P3^3;
sbit line_2=P3^2;
sbit line_3=P3^1;
sbit line_4=P3^0;

//======================按键变量
// 使用 main.c 中的按键事件变量，让扫描模块把键号交给主循环。
extern unsigned char mkey;

//=========================横扫描
 // 依次扫描 4 列，键号按行排列：第一行 1~4，第二行 5~8，以此类推。
// 注意：原代码未把局部变量初始化为 0；无按键时返回值可能不确定。
unsigned char mkey_state()
{
	unsigned char mkey_state;
	list_1=0;										//第一列
	list_2=1;
	list_3=1;
	list_4=1;
	P3|=0x0f;						// 先把 4 根行线写 1，再读取按下后被拉低的行
	if(line_1==0)mkey_state=1;
	if(line_2==0)mkey_state=5;
	if(line_3==0)mkey_state=9;
	if(line_4==0)mkey_state=13;
	
	list_1=1;										//第二列
	list_2=0;
	list_3=1;
	list_4=1;
	P3|=0x0f;						// 先把 4 根行线写 1，再读取按下后被拉低的行
	if(line_1==0)mkey_state=2;
	if(line_2==0)mkey_state=6;
	if(line_3==0)mkey_state=10;
	if(line_4==0)mkey_state=14;
	
	list_1=1;										//第三列
	list_2=1;
	list_3=0;
	list_4=1;
	P3|=0x0f;						// 先把 4 根行线写 1，再读取按下后被拉低的行
	if(line_1==0)mkey_state=3;
	if(line_2==0)mkey_state=7;
	if(line_3==0)mkey_state=11;
	if(line_4==0)mkey_state=15;
	
	list_1=1;										//第四列
	list_2=1;
	list_3=1;
	list_4=0;
	P3|=0x0f;						// 先把 4 根行线写 1，再读取按下后被拉低的行
	if(line_1==0)mkey_state=4;
	if(line_2==0)mkey_state=8;
	if(line_3==0)mkey_state=12;
	if(line_4==0)mkey_state=16;
	
	return mkey_state;
}

// 上一次有键、本次无键，才把键号写入 mkey；相当于在松手时触发。
// 这里依赖 mkey_state() 在无键时返回 0，因此上面的未初始化问题需要注意。
void mkey_loop()						//松手检验，物理消抖
{
   static unsigned char lastkey,nowkey;
	lastkey=nowkey;
	nowkey=mkey_state();
	if(nowkey==0&&lastkey==1)mkey=1;
	if(nowkey==0&&lastkey==2)mkey=2;
	if(nowkey==0&&lastkey==3)mkey=3;
	if(nowkey==0&&lastkey==4)mkey=4;
	if(nowkey==0&&lastkey==5)mkey=5;
	if(nowkey==0&&lastkey==6)mkey=6;
	if(nowkey==0&&lastkey==7)mkey=7;
	if(nowkey==0&&lastkey==8)mkey=8;	
	if(nowkey==0&&lastkey==9)mkey=9;	
	if(nowkey==0&&lastkey==10)mkey=10;
	if(nowkey==0&&lastkey==11)mkey=11;
	if(nowkey==0&&lastkey==12)mkey=12;
	if(nowkey==0&&lastkey==13)mkey=13;
	if(nowkey==0&&lastkey==14)mkey=14;
	if(nowkey==0&&lastkey==15)mkey=15;
	if(nowkey==0&&lastkey==16)mkey=16;
}










