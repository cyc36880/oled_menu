#include "menuconfig.h"
#include "menu.h"
#include "oledfont.h" //字体文件
#include "menufontshow.h" //字符函数
#include "Graphicalfunctions.h" // 图形化函数


void test1_1(menu_area *target);

TypLineChartMap Map = {120, 40, 20}; //折线图


/*
	功能：初始化菜单列表 <此函数名不可修改>
*/
void MakeMenu(void)
{
	ClearnLineChartMapDat(&Map); //折线图内存清零
	
	TargetMenu = AddToMenuList(34, 0, 32, 16, 1, NULL);
	TargetMenu->menuinterface = test1_1;
	AddToSpecialFunction(TargetMenu, MenuTime, 100);
	
	MenuHeartTimeStart = 1;//菜单心跳
}


/*
	功能：菜单显示的具体实现函数
	注意：函数必须是 void 类型，形参必须且仅为 menu_area *target
*/
void test1_1(menu_area *target)
{

	static uint8_t h = 4;
	static uint8_t r0 = 5;
	
	int16_t *p;
	//SetFont(F8X16, F8X16_SizeInf);//设置字体
	SetFont(F6X8, F6X8_SizeInf);//设置字体
	
	if(TriggerCheck(target, MenuTime)) { //时间列表
		
		if(StatusInformationAlways == Menu_up) {
			ClearnSerialShowBuf();
			h++;
		}
		if(StatusInformationAlways == Menu_down) {
			h--;
		}
		if(--r0 == 0) {
			r0 = 5;
		}
		AddDatToLineChartMap(&Map, h); //折线图填充数据
	}
	MenuShowNum(target, 0, 0, 0, h);
	
	p = LineChart(&Map, 0, 20); //折线图
	
	if(Map.RxNum) { //接收数量为零不显示
		DrawCircle(p[0], p[1], r0);
	}
	GRAPHICSSHOWMANNER = GraphicsRollColor;
	PictureShow(NULL, picturesize, picture0, 20, 16); //图片显示
	GRAPHICSSHOWMANNER = GraphicsNormal;
}







