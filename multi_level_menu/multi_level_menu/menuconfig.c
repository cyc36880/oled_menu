#include "menuconfig.h"
#include "menu.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数


void test1_1(menu_area *target);

menu_area menutest1;


/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
static void MakeMenu(void)
{
	TargetMenu = SetMenu(&menutest1, MenuScreenCenterX(32), 0, 32, 16, 1, NULL); //使用以创建好的菜单配置
	TargetMenu->menuinterface = test1_1;
	AddToSpecialFunction(TargetMenu, MenuTime, 100);

	MenuHeartTimeStart = ENABLE;//菜单心跳
}

/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/
void test1_1(menu_area *target)
{
	static uint8_t h = 4;

	
	//SetFont(F8X16, F8X16_SizeInf);//设置字体
//	SetFont(F6X8, F6X8_SizeInf);//设置字体
	
	
	if(TriggerCheck(target, MenuTime)) { //时间列表
		
		
		if(StatusInformationAlways == Menu_up) {
			h++;
			ClearnSerialShowBuf();
		}
		if(StatusInformationAlways == Menu_down) {
			h--;
		}
	}
	
	MenuShowNum(target, 0, 0, 0, h);
	if(!MenuSerialRxNum) {
		MenuShowAscStr(NULL, 25, 25, "wait...");
	}
	else {
		SetFont(F6X8, F6X8_SizeInf);//设置字体
		SerialCharacterText(NULL, 20, 20);
		SetFont(F8X16, F8X16_SizeInf);//设置字体
	}
}









//功能：初始化菜单相关内容
void MenuInit(void)
{
	OLED_Init(); // OLED初始化
	MakeMenu(); //菜单列表初始化
}




