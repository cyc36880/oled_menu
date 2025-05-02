#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// ================================ 菜 单 ==============================

#define MENUHEARDID 1 //菜单列表起始ID

// 菜单操作
enum MenuState
{
	Menu_noaction=0, // 无操作
	Menu_up,  		 // 向上
	Menu_down, 		 // 向下
	Menu_confirm 	 // 确认
};

//菜单参数
typedef struct MENU_AREA
{
	int16_t x;
	int16_t y;
	int16_t width;
	int16_t high;
	uint16_t id; //菜单ID，不要修改
	bool checked; //能否被选中 
	bool menulistend; //菜单列表结束
	struct MENU_AREA *next;     //下一个
	struct MENU_AREA *previous; //上一个
	struct MENU_AREA *subclass; //子类
	struct MENU_AREA *father;   //父类
	void (*menuinterface)(struct MENU_AREA *target); //菜单内容
	uint16_t userinformation;//用户自定义信息
}menu_area;




//功能：注册或添加菜单 
menu_area *AddToMenuList(uint16_t x, uint16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer);

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

//功能：目标菜单首位相连，空指针跳过
void MakeMenuListRing(menu_area *target);





// =========================== 屏 幕 ====================================

#define SCREENWIDTH 128 // 屏幕宽度
#define SCREENHIGH  64  // 屏幕高度

//向上整除 x=8 -> 1, x=9 -> 2
#define DIVIDEUP(x) (x*10/8%10 ? x/8+1:x/8)

//屏幕参数
typedef struct
{
	const uint16_t screenwidth; //屏幕宽度
	const uint16_t screenhigh;  //屏幕高度 (向上除8)
	bool refresh; //刷新标志
}TypedefScreen;

extern unsigned char DisplayBuff[]; // 屏幕显示缓存
extern TypedefScreen ScreenPara; // 屏幕具体参数
extern menu_area * TargetMenu; // 实时目标菜单
extern enum MenuState StatusInformation; //输入设备状态

// 功能：在目标菜单的相对位置画点
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b); 

//功能：画目标菜单的矩形，空指针不画
void DrawMenuRectangle(menu_area *target);



// 功能：菜单运行函数
void MenuRun(void); 



#endif





