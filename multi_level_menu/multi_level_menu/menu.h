#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h>  //uint16_t 等所在地
#include <stdlib.h>  //malloc   所在地
#include <stdbool.h> //bool     类型所在地

#include "oled.h"
#include "menuconfig.h"

/*
特别注意：

	请勿在中断中使用以下函数！！！
		
	每个函数详细的注意事项以及返回值和功能等，请跳转该函数上部查看
	
	该菜单使用malloc申请空间，无free，注意内存溢出。请确保heap(堆)大小足够
*/

// ================================ 菜 单 ==============================

#define MENUHEARDID 1 //菜单列表起始ID

// 菜单操作
enum MenuState
{
	Menu_noaction=0, // 无操作
	Menu_up,  		 // 向上
	Menu_down, 		 // 向下
	Menu_Sub,        // 子类
	Menu_Father,     // 父类
	Menu_confirm 	 // 确认
};

// 用户特殊信息
enum SpecialInformation
{
	MenuTime = 0x01, //时间列表
	EnterMenu = 0x02, // 进入菜单，if末尾建议加return
	ExitMenu = 0x04,  // 退出菜单，if末尾建议加return
	EnterShowMenuList = 0x08, // 进入显示菜单列表，if末尾建议加return
	ExitShowMenuList = 0x10, // 退出显示菜单列表，if末尾建议加return
	
	/*****上述功能的改进*****/
	
	MenuTimeForce = 0x20, //时间列表强制执行。 与MenuTime配合使用，否则无效，无需对此判断，if末尾加return
	
	/******** 其 它 **********/
	MenuHaveOverall = 0x40,//不可使用，其它功能占用该位。菜单列表全局
};




//菜单时间队列
typedef struct MENU_TIMEMS
{
	uint16_t counttime;
	uint16_t timems;
}menu_timems;

//菜单参数
typedef struct MENU_AREA
{
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t high;
	uint16_t id; //菜单ID，不要修改
	bool checked; //能否被选中 
	bool menulistend; //菜单列表结束
	struct MENU_AREA *next;     //下一个
	struct MENU_AREA *previous; //上一个
	struct MENU_AREA *subclass; //子类
	struct MENU_AREA *father;   //父类
	void (*menuinterface)(struct MENU_AREA *target); //菜单内容
	menu_timems *menu_time;
	uint16_t specialfeatures;//特殊功能注册
	uint16_t specfeattrigflag; //特殊功能触发标记
}menu_area;

//列表全局队列
typedef struct MENULISTOVERALL 
{
	menu_area *Affiliation; //隶属于
	void (*menuinterface)(void); //菜单内容
	struct MENULISTOVERALL *next;
}MenuListOverall;



//功能：注册或添加菜单 
menu_area *AddToMenuList(int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer);

//功能：链接到父类
void LinkToParentClass(menu_area *target, menu_area *source);



//功能：对菜单上下偏移寻址 
menu_area *MenuListAddressing(menu_area *target, bool upordown, uint16_t offset);

//功能：对菜单头前后偏移寻址 
menu_area *MenuHeadAddressing(menu_area *target, bool upordown, uint16_t offset);

//功能：寻找以目标单元为基准的第num个可选中的菜单，无则返回原地址
menu_area *NextCancheMenuList(menu_area *target, int16_t num);

//功能：寻找以目标单元为基准的第num个可选中的菜单头，无则返回原地址
menu_area *NextCancheMenuHeard(menu_area *target, int16_t num);

//功能：找到菜单所在菜单列表的菜单头，空指针NULL 
menu_area *FindMeunListHeard(menu_area *target);

//功能：找到菜单所在菜单列表的菜单尾，空指针NULL
menu_area *FindMeunListTail(menu_area *target);

//功能：返回当前显示列表的头，即使它不能被选中
menu_area *MenuListShowHead(menu_area *target);

//功能：返回当前显示列表的尾，即使它不能被选中
menu_area *MenuListShowTail(menu_area *target);




//功能：目标菜单首位相连，空指针跳过
void MakeMenuListRing(menu_area *target);

//功能：菜单列表始终执行函数
MenuListOverall *MenuOverall(menu_area *target);

//功能：特殊功能注册
void AddToSpecialFunction(menu_area *target, uint16_t function, uint16_t ms);

//功能：特殊功能检查，触发返回1，否则返回0
bool TriggerCheck(menu_area *target, enum SpecialInformation function);


extern menu_area * TargetMenu; // 实时目标菜单

extern enum MenuState StatusInformation; //输入设备状态
extern enum MenuState StatusInformationAlways; // 输入设备状态 <不会改变>

extern uint32_t MenuSize;//菜单申请的空间大小





// =========================== 屏 幕 ====================================

#define SCREENWIDTH 128 // 屏幕宽度
#define SCREENHIGH  64  // 屏幕高度

//向上整除 x=8 -> 1, x=9 -> 2
#define DIVIDEUP(x) (x*10/8%10 ? x/8+1:x/8)

enum ScreenShowManner //屏幕显示方式
{
	ScreenNormal,  //正常
	ScreenRollback //反转
};

enum GraphicsShowManner //图形显示方式
{
	GraphicsNormal,  //正常
	GraphicsCover, //覆盖
	GraphicsRollColor //反色
};

//屏幕参数
typedef struct
{
	const uint16_t screenwidth; //屏幕宽度
	const uint16_t screenhigh;  //屏幕高度 (向上除8)
	bool refresh; //刷新标志
}TypedefScreen;

extern enum ScreenShowManner SCREENSHOWMANNER; //屏幕显示方式
extern enum GraphicsShowManner GRAPHICSSHOWMANNER;//图形显示方式

extern unsigned char DisplayBuff[]; // 屏幕显示缓存
extern TypedefScreen ScreenPara; // 屏幕具体参数


// 功能：在目标菜单的相对位置画点
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b); 






// ============================== 其 它 ==================================


// 功能：菜单运行函数，为保证正常运行，该函数在while中每秒循环次数应大于2000次
void MenuRun(void); 
// 功能：菜单心跳执行，每1ms执行该函数
void MenuHeartTime(void);
//菜单心跳开始标志 放在最后 置1运行
extern bool MenuHeartTimeStart; 

#endif





