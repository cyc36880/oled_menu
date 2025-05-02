#include "menu.h"
#include "oled.h"
#include "main.h"

// ================================= 设 备 ========================================

enum MenuState StatusInformation; //输入设备状态

/*
	* 功能：输入设备状态
*/

enum MenuState Scan(void)
{
	if(HAL_GPIO_ReadPin(K0_GPIO_Port, K0_Pin) == GPIO_PIN_RESET) return Menu_down;
	else if(HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_RESET) return Menu_confirm;
	return Menu_noaction;
}

bool KeyState(enum MenuState *k)
{
	static enum MenuState KeySt = Menu_noaction;
	static uint8_t count=0;
	
	if(KeySt != Scan())
	{
		HAL_Delay(1);
		if(KeySt != Scan())
		{
			if(++count > 1){
				count = 0;
				*k = KeySt;
				return 1;	
			}
			KeySt = Scan();
		}
	}
	return 0;
}

/*
	功能：输入设备扫描，该函数仅由系统调用
*/
static void EquipmentState(void)
{
	if(KeyState(&StatusInformation)){ //按键扫描
		if(StatusInformation != Menu_noaction){ //非空闲
			ScreenPara.refresh = 1;
		}
	}
}


// ================================= 屏 幕 ========================================

const unsigned int BUFFWEIGH = SCREENWIDTH;
const unsigned int BUFFHIGH  = DIVIDEUP(SCREENHIGH);

// 定义屏幕信息
TypedefScreen ScreenPara = {BUFFWEIGH, BUFFHIGH, 1};

// 显示缓存
unsigned char DisplayBuff[BUFFWEIGH* BUFFHIGH] = {0};


//清空显示缓存
void ClearnBuff(void)
{
	unsigned int i, j = BUFFWEIGH * BUFFHIGH;
	for(i=0; i<j; i++)
		DisplayBuff[i] = 0;
}




// ========================== 菜 单 ==================================
/*
*	功能：注册或添加菜单 
*	x, y, width, high: 为注册菜单单元的位置与大小 
*	checked: 能否被选中，1能 0否 
*	transfer：为 NULL 注册的是菜单头，填入菜单地址为其尾加入 
*/
menu_area * AddToMenuList(uint16_t x, uint16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p;
	menu_area *k;
	
	p = (menu_area *) malloc(sizeof(menu_area));
	
	if(transfer != NULL)  //尾部加入 
	{
		k = FindMeunListTail(transfer); //找到文件尾部
		k->next = p;
		p->previous = k;
		p->id = k->id + 1; 
	}
	else //创建菜单头
	{
		p->id = MENUHEARDID; // 菜单头ID 
		p->previous = NULL; //上一个
	}
	
	p->x = x>=BUFFWEIGH ? BUFFWEIGH-1 : x;
	p->y = y>=SCREENHIGH ? SCREENHIGH-1 : y;
	p->width = width+p->x>BUFFWEIGH ? BUFFWEIGH-p->x : width;
	p->high = high+p->y>SCREENHIGH ? SCREENHIGH-p->y : high;
	p->checked =  checked; // 能否选中 
	p->menulistend = NULL; //菜单列表结束
	p->next = NULL;      //下一个
	p->subclass = NULL;  //子类 
	p->father = NULL;    // 父类 
	p->menuinterface = NULL; // 菜单内容
	p->userinformation = NULL; //用户自定义信息
	return p;
}

/**
 *  功能：链接到父类
 *  target：目标地址
 *  source：源地址 
 * 
 **/
void LinkToParentClass(menu_area *target, menu_area *source)
{
	if(target==NULL || source==NULL) return; //检查地址是否有效
	target->subclass = source;
	source->father = target;
}

/**
 * 功能：对菜单向上下偏移寻址
 * target：目标寻址菜单 
 * offset：偏移步数 
 * upordown: 0向下偏移 1向上偏移 
 * 返回值：相对与源地址偏移后的地址，offset=0返回源地址，无则返回NULL 
 **/
menu_area * MenuListAddressing(menu_area *target, bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; //检查地址是否有效
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->next == NULL){  //检查下一个是否存在
				return NULL;
			}
			p = p->next;
		}
		else
		{
			if(p->previous == NULL){ //检查上一个是否存在
				return NULL;
			}
			p = p->previous;
		}
	}
	return p;
}

/**
 * 功能：对菜单头的前后偏移寻址 
 * target：目标寻址菜单 
 * offset：偏移步数 
 * upordown: 0向下偏移 1向上偏移 
 * 返回值：相对与源地址偏移后的地址，offset=0返回源地址，无则返回NULL 
 **/
menu_area * MenuHeadAddressing(menu_area *target,  bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; //检查地址是否有效
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->subclass == NULL){ //检查子类是否存在
				return NULL;
			}
			p = p->subclass;
		}
		else
		{
			if(p->father == NULL){ //检查父类是否存在
				return NULL;
			}
			p = p->father;
		}
	}
	return p;
}

/*
	功能：寻找以目标单元为基准的第num个菜单可选中的菜单，无则返回原地址
		  输入空地址则返回空
	target: 基地址
	num：正值向下寻找，负值向上寻找
	返回值：寻址后的地址
*/
menu_area *NextCancheMenuList(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num; // 绝对值
	uint16_t i;

	if(p==NULL) return NULL; //检查地址是否有效
	for(i=0; i<j;)
	{
		p=MenuListAddressing(p, num<0, 1);
		if(p==NULL) return target;
		if(p->checked == 1) i++;
	}
	
	return p;
}


/*
	功能：寻找以目标单元为基准的第num个菜单可选中的菜单头，无则返回原地址
	target: 基地址
	num：正值向右寻找，负值向左寻找
	返回值：寻址后的地址
*/
menu_area *NextCancheMenuHeard(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num; // 绝对值
	uint16_t i;

	if(p==NULL) return NULL;//检查地址是否有效
	for(i=0; i<j;)
	{
		p=MenuHeadAddressing(p, num<0, 1); 
		if(p==NULL) return target;
		if(p->checked == 1) i++;
	}
	return p;	
}


/*
	功能：找到菜单所在菜单列表的菜单头，空指针返回NULL 
	target：菜单指针
*/
menu_area *FindMeunListHeard(menu_area *target) 
{
	menu_area *heard=target;
	
	if(target == NULL) return NULL;//检查地址是否有效
	for( ; ; )
	{
		if(heard->id == MENUHEARDID) break; // 菜单列表头id 
		if(heard->previous == NULL) break; // 检查上一个是否存在
		heard = heard->previous;
	}
	return heard;
}


/*
	功能：找到菜单所在菜单列表的菜单尾，空指针或超过最大列表数返回NULL 
	target：菜单指针
*/
menu_area *FindMeunListTail(menu_area *target) 
{
	menu_area *tail=target;
	
	if(target == NULL) return NULL; //检查地址是否有效
	
	for( ; ; )
	{
		if(tail->next == NULL) break; //检查下一个是否存在
		if(tail->next->id == MENUHEARDID) break; // 循环中，下一个是否为头
		tail = tail->next;
	}
	return tail;
}


/*
	功能： 目标菜单首位相连，地址为空跳过 
	target：所在菜单列表的任一菜单 
*/
void MakeMenuListRing(menu_area *target)
{
	menu_area *heard, *tail;
	
	if(target == NULL) return; //检查地址是否有效
	
	heard = FindMeunListHeard(target); //找标准头
	tail = FindMeunListTail(target); //找标准尾
	
	tail->next = heard; //头尾链接
	heard->previous = tail;
}





// ========================== 图 形 化 =======================

/*
	功能：在目标菜单的 >相对位置< 画点
	target：目标菜单
	x, y；相对位置
	w_b：1有色 0无色
*/
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b)
{
	int16_t px=0, py=0; // 屏幕相对位置
	
	px = target->x + x;
	py = target->y + y;
	
	if(px>=SCREENWIDTH || py>= SCREENHIGH) return; //判断是否超出屏幕边界
	if(px<0 || py<0) return;
	
	if(x>target->width || y>target->high) return; // 判断是否超出菜单边界
	if(x<0 || y<0) return;
	
	write_point(px, py, w_b);
}

/*
	功能：画目标菜单的矩形，空指针不画
	target: 目标菜单
*/
void DrawMenuRectangle(menu_area *target)
{
	uint16_t x1=0, y1=0, x2=0, y2=0;
	
	if(target == NULL) return;
	
	x1 = target->x;
	y1 = target->y;
	x2 = x1 + target->width;
	y2 = y1 + target->high;
	
	LCD_DrawLine(x1, y1, x2, y1);
	LCD_DrawLine(x1, y1, x1, y2);
	LCD_DrawLine(x2, y2, x1, y2);
	LCD_DrawLine(x2, y2, x2, y1);
}





// ======================== 系 统 调 用 ====================

menu_area * TargetMenu = NULL; // 实时目标菜单

/*
	功能：
*/

void MenuHeartTime(void)
{
	
}

/*
	功能：菜单列表各个菜单内容循环显示
*/
static void MenuListInterface(void)
{
	menu_area *p = TargetMenu;
	
	if(p==NULL) return; //检查地址是否有效
	
	for( ; ; ) // 找到开始显示的头
	{
		if(p->id == MENUHEARDID) break; //检查当前是否为标准菜单头
		if(p->previous == NULL) break; // 检查上一个菜单是否存在
		if(p->previous->menulistend) break; // 检查本列表上一界面是否为结尾
		p = p->previous;
	}

	for( ; ; ) // 从开始显示的头显示到指定尾
	{
		if(p->menuinterface != NULL){ //是否有指定的图像函数	
			(p->menuinterface)(p);
		}
		if(p->next==NULL) return; //到达列表底部
		if(p->menulistend) return;
		if(p->next->id==MENUHEARDID) return;
		p = p->next;
	}
}


/*
	功能：设备输入状态改变实时目标菜单指针
*/
static void StateToPointer(void)
{
	switch(StatusInformation)
	{
		case Menu_up: 		TargetMenu=NextCancheMenuList(TargetMenu, -1);break;
		case Menu_down: 	TargetMenu=NextCancheMenuList(TargetMenu,  1);break;
		case Menu_confirm:  
			if(TargetMenu->subclass != NULL) StatusInformation = Menu_noaction; // 防止切换菜单列表时立即运行函数内部指令
			TargetMenu=NextCancheMenuHeard(TargetMenu, 1);
			break;
		default:break;
	}
}


/*
	功能：菜单选中风格
*/
static void MenuCheckedStyle(menu_area *target)
{
	DrawMenuRectangle(target); // 用户菜单选中
}


/*
	功能：菜单运行函数。 the end 之前，越靠后屏幕显示优先级越高
*/
void MenuRun(void)
{
	EquipmentState(); //输入设备
	
	if(ScreenPara.refresh)
	{
		ScreenPara.refresh=0;// 刷新标志复位
	
		StateToPointer(); // 设备输入状态改变实时目标菜单指针
		MenuListInterface(); //依次显示当前菜单列表
		MenuCheckedStyle(TargetMenu);//菜单选中风格
		disp_flush();// 刷新屏幕//the end
		ClearnBuff(); // 清空缓存
	}
}





