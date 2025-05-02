#ifndef _MENU_TOOL_H_
#define _MENU_TOOL_H_

#include "menu.h"

// *******************************
// *********** 消 息 *************
// *******************************

// 消息状态 
enum MessageState
{
	Mes_Have = 0x01, //消息 
};

// 消息 
typedef struct MESSAGE
{
	uint16_t message; // 消息内容 
	uint8_t state;    // 状态 
	struct MESSAGE *next; 
}MessageTypedef;

/*
	功能：消息初始化
	Target：要列入的消息列表头句柄（指针） 
	mes：要初始化的消息 
*/

MessageTypedef * MessageInit(MessageTypedef** Target, MessageTypedef* mes);

/**
	功能：  消息广播
	mes：   消息链表句柄 
	mesage：要广播的消息 
 **/
void MessageBroadcast(MessageTypedef *mes, uint16_t mesage);

#define MesHave(p)  (p.state & Mes_Have) //判断 是否有消息
#define MesClear(p) (p.state &= !Mes_Have) // 清除消息状态
#define MesRead(p)  (p.message)           //读消息


extern MessageTypedef *Message_Key; //按键消息广播列表 








// ************** 横 向 滚 动 *******************

/*
	功能：滚动显示
	target：当前所处的任一菜单指针， 滚动参考为此
	showSY：列表允许显示的起始y坐标 头坐标
	showEY：列表允许显示的结束y坐标 底坐标
	TarSY： 指针允许的起始y坐标     头坐标 应 >= showSY
	TarEY： 指针允许的结束y坐标     底坐标 应 <= showEY
	style： 逐次逼近显示.   0 直接刷新至目标   1 逐像素刷新至目标

	ret: 无需滚动或滚动完成返回1，否则返回0

	注意：该函数会改变大量菜单的menulistend属性，在该菜单所在的菜单列表中，对于出入菜单特殊功能，
		建议使用EnterMenu，ExitMenu。EnterShowMenuList与ExitShowMenuList存在多次触发问题
*/
uint8_t ScrollingDisplay_X(menu_area *target, int16_t showSX, uint8_t showEX, int16_t TarSX, int16_t TarEX, uint8_t style);


// ************** 纵 向 滚 动 *******************


/*
	功能：滚动显示
	target：当前所处的任一菜单指针， 滚动参考为此
	showSY：列表允许显示的起始y坐标 头坐标
	showEY：列表允许显示的结束y坐标 底坐标
	TarSY： 指针允许的起始y坐标     头坐标 应 >= showSY
	TarEY： 指针允许的结束y坐标     底坐标 应 <= showEY
	style： 逐次逼近显示   0 直接刷新至目标   1 逐像素刷新至目标

	ret: 无需滚动或滚动完成返回1，否则返回0

	注意：style=0时 该函数会改变大量菜单的menulistend属性，在该菜单所在的菜单列表中，对于出入菜单特殊功能，
		建议使用EnterMenu，ExitMenu。EnterShowMenuList与ExitShowMenuList存在多次触发问题
*/
uint8_t ScrollingDisplay_Y(menu_area *target, int16_t showSY, uint8_t showEY, int16_t TarSY, int16_t TarEY, uint8_t style) ;






// *********** 菜 单 堆 叠 与 展 开 ****************

/*
	功能：菜单堆叠（与显示的头菜单重叠）
	target：句柄
*/
void MenuCoorAlignment(menu_area *target);

/*
	功能：使堆叠的菜单 以头菜单 为目标恢复展开状态
	target：句柄
	mod： 0 纵向展开  1：横向展开
	style: 0 直接展开   1 动画展开
	ret : 完全展开或无需展开返回1，否则返回0
		
	注意：重新展开后，可能与原有的坐标不一致，会使其更紧凑，
				！！会使菜单依次往后排序，无滚动菜单列表不要使用此函数
 */
uint8_t MenuCoorRecovery(menu_area *target, uint8_t mod, uint8_t style);


/*
	功能：列表切换实例（有动画效果）
				列表的 展开 与 滚动
	mes：消息句柄, （按键消息）
	start：指示器允许起始位置
	end：  指示器允许结束位置
	dir：  方向。0 纵向，1横向

	注意：当使用“特殊功能”的EnterMenu来堆叠菜单 （MenuCoorAlignment） 时，此函数应比
			MenuCoorAlignment执行的优先级低，（如可放入在 MenuAlwaysRun_PM）
*/
void ListSwitchIns(MessageTypedef *mes, int16_t start, int16_t end, bool dir);








// *******************************************
// ************ 函 数 定 时 执 行 ************
// *******************************************

enum FUNCTINOTICKEROPTIONS
{
	normalRun = 0,
	interruptRun,
};
typedef struct FUNCTINOTICKER
{
	uint32_t count; //计数
	uint32_t ms;  //目标
	struct FUNCTINOTICKER *next; //下一个
	void (*Function)(void); //执行函数
	enum FUNCTINOTICKEROPTIONS RunMod; //模式
	bool Flag; //执行标志
	bool run; //运行 <使能>
}FunctionTicker;

FunctionTicker *SetFunctionTicker(FunctionTicker *FTtarget, uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(void));
//功能：函数定时执行
FunctionTicker *AddToFunctionTicker(uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(void));







// **********************************************
// ****************** 单次运行 ******************
// **********************************************

typedef struct RUNONE
{
	unsigned int dat;
	unsigned char floag;
}TypeRunOne; //需初始化为 {1, 0};

/* t=0: dat相等运行一次  t=1：dat2改变运行一次!!!只能被调用一次!!! */
bool tRunOne(TypeRunOne *RunOne, bool t, unsigned int dat1,unsigned int dat2);




// ****** 工 具 **********

// 绝对值
uint16_t myabs(int16_t dat);



#endif



