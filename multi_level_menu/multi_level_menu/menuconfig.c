#include "menu.h"
#include "menuconfig.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数

void FTRunTest0(void);
void FTRunTest1(void);

menu_area menutest;
void test1_1(menu_area *target);

uint8_t count=0;
uint8_t count1=0;

/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
static void MakeMenu(void)
{
	TargetMenu = &menutest;
	SetMenu(&menutest, MenuScreenCenterX(32), MenuScreenCenterY(16), 32, 16, ENABLE, NULL)->menuinterface = test1_1;
	AddToSpecialFunction(&menutest, MenuTime, 1000);
	
	AddToFunctionTicker(100, interruptRun, FTRunTest0);
	AddToFunctionTicker(200, interruptRun, FTRunTest1);
	
	
}

/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/
void test1_1(menu_area *target)
{
	MenuShowNum(NULL, 0, 0, 0, count);
	MenuShowNum(NULL, 0, 32, 0, count1);
}

void FTRunTest0(void)
{
	count++;
}
void FTRunTest1(void)
{
	count1++;
}






// ==================================================================
// ======================== 菜 单 固 定 函 数 =======================
// ==================================================================



//**************** while中全速运行  ************************


void AlwaysRun(void)
{
	
}



//*************** 菜单跟随刷新 *********************


// 菜单的每次刷新都会执行该函数 执行优先级最高
void MenuAlwaysRun_PH(void)
{
	
}

// 菜单的每次刷新都会执行该函数 执行优先级最低
void MenuAlwaysRun_PL(void)
{
	
}


//************* 初始化菜单相关内容 *********************

void MenuInit(void)
{
	OLED_Init(); // OLED初始化
	MakeMenu(); //菜单列表初始化
	MenuHeartTimeStart = ENABLE;//菜单心跳
}




