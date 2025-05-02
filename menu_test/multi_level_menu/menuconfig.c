#include "menuconfig.h"

#include "menu.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数
#include "menu_tool.h" //工具

MessageTypedef mes;

ProgressBarTypedef bar;

const uint8_t *menutext[] = 
{
	"7k7k",
	"123",
	"456",
	"56789",
	"a滋生\nd",
	"cdeqwe",
	"12滋生34",
	"33\n44",
	"34444"
};
menu_area menuarray[sizeof(menutext)/sizeof(menutext[0])];
const uint8_t MenuNum = sizeof(menutext)/sizeof(menutext[0]);

void ment_f(menu_area *target);

/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
static void MakeMenu(void)
{
	TargetMenu = &menuarray[0];
	
	TargetMenuPointrt.style = ENABLE; //使能指示器动画
	
	MesInit_Key(&mes);
	
	AddToFunctionTicker(50, normalRun, MenuRefresh, 0); //所有界面刷新

	SetMenu(&menuarray[0], 0, 0, 16*3, 16, ENABLE, NULL);
	
	BatchFastSimilarMenuDown(menuarray, menuarray, MenuNum, 2, ment_f);
	MakeMenuListRing(menuarray);

	
	ProgressBarInit(&bar, 0, 60, 15, sizeof(menutext)/sizeof(menutext[0]))->animation = ENABLE;
	
}

/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/


void ment_f(menu_area *target)
{
	FontInfoType *FontInfo;
	
	const uint8_t *str = menutext[target->id-1];
	
	FontInfo = StringDeal(str);
	
	switch(target->id)
	{
		case 6:
			SetFont(F6X8);
			

			
		break;
		
		default:
		
		break;
	}
	if(FontInfo->hznum) //判断是否有汉字
		CharacterTextC(target, 0, 0, FontInfo->maxPix+10, str);
	else
		CharacterText(target, 0, 0, FontInfo->ascnum, str);
	
	if(SetIndicatorSize(FontInfo, target, NULL)) //指示器变化
	{
		MesBro_Key(Menu_noaction); //发送空白消息
	}
	SetFont(F8X16); //复位字体大小
}








// ==================================================================
// ======================== 菜 单 固 定 函 数 =======================
// ==================================================================



//**************** while中全速运行  ************************


void AlwaysRun(void)
{
	
}



/* ************** 菜单跟随刷新 ******************** */


/**** 菜单的每次刷新都会执行该函数 执行优先级最高 《！！不要写显示相关函数，会被异常清空！！》 ****/
void MenuAlwaysRun_PH(void)
{
	
	
}



/**** 菜单的每次刷新都会执行该函数 执行优先级中****/
void MenuAlwaysRun_PM(void)
{
	ListSwitchIns(&mes, 0, 0, 63);
}



/**** 菜单的每次刷新都会执行该函数 执行优先级低 ****/
void MenuAlwaysRun_PL(void)
{
	ProgressBar(&bar, 64, 40, TargetMenu->id);
}




//************* 初始化菜单相关内容 *********************

void MenuInit(void)
{
	MenuSysBaseInit(); //系统初始化
	MakeMenu(); //菜单列表初始化

	/* ********** USER BEGIN********** */
	
	MenuCoorAlignment(TargetMenu, 0);
	
	/* ********** USER END ********** */
	
	MenuHeartTimeStart = ENABLE;//菜单心跳
	
}








