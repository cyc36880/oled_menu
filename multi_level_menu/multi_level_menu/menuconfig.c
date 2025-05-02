#include "menu.h"
#include "menuconfig.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数

uint16_t id = MENUHEARDID;

void test1_1(menu_area *target);
void always(void);

menu_area menutest0;
#define arraynum  20 //批量菜单个数
menu_area menuarray[arraynum];//批量菜单数组


/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
static void MakeMenu(void)
{
	TargetMenu = &menutest0;
	SetMenu(&menutest0, MenuScreenCenterX(32), 0, 32, 16, 1, NULL)->menuinterface = test1_1; //使用以创建好的菜单配置
	
	SetMenu(&menuarray[0], MenuScreenCenterX(32), 0, 32, 16, 1, NULL);
	BatchFastSimilarMenu(&menuarray[0], &menuarray[0], arraynum, 2, test1_1); //批量菜单初始化
	LinkToParentClass(&menutest0, &menuarray[0]);
	
	AddToSpecialFunction(&menuarray[arraynum-1], EnterMenu, 0); //添加特殊功能
	
	MenuOverall(&menuarray[0])->menuinterface = always;
	
	
	
	MenuHeartTimeStart = ENABLE;//菜单心跳
}

/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/
void test1_1(menu_area *target)
{
	if(TriggerCheck(target, EnterMenu)) {
		TargetMenu = FindMenuOfID(TargetMenu, id, 0);
		return;
	}
	MenuShowNum(target, 0, 0, 2, target->id);
}



void always(void)
{
	id = TargetMenu->id;
	ScrollingDisplay_Y(TargetMenu, 0, 64, 16, 32);
}






//功能：初始化菜单相关内容
void MenuInit(void)
{
	OLED_Init(); // OLED初始化
	MakeMenu(); //菜单列表初始化
}




