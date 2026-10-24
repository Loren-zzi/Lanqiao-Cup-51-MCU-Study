#ifndef __MKEY_H_
#define __MKEY_H_


 // 读取矩阵键盘：1~16 是键号；无按键时应返回 0（见 mkey.c 中的初始化提醒）。
 unsigned char mkey_state();
 // 定时调用，检测松手并把按键事件写入 main.c 的 mkey。
 void mkey_loop()	;					//，物理消抖

#endif     