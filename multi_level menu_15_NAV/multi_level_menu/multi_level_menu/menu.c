#include "menu.h"
#include "Graphicalfunctions.h"

void FunctionTickerRun(void); 
void FunctionTickerRunIRQ(void); 

enum MenuState StatusInformation = Menu_noaction; 
enum MenuState StatusInformationAlways = Menu_noaction; 

uint8_t InuptEnable = ENABLE; 
uint8_t ResponseEnable = ENABLE; 

static enum MenuState Scan(void)
{
	static enum MenuState adcstate = Menu_noaction;
	
	if(HAL_GPIO_ReadPin(K0_GPIO_Port, K0_Pin) == GPIO_PIN_RESET) return Menu_down;
	else if(HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_RESET) return Menu_Sub;
	
	if(adcbuf[0] > (1630 + 1500)){
		adcstate = Menu_Sub;
		return adcstate;
	} 
	else if(adcbuf[0]<(1630 + 200) && adcbuf[0]>(1630 - 200)) adcstate = Menu_noaction;
	else if(adcbuf[0] < (1630 - 1500)) {
		adcstate = Menu_Father;
		return adcstate;
	}
	
	if(adcbuf[1] > (1630 + 1500)){
		adcstate = Menu_down;
		return adcstate;
	} 
	else if(adcbuf[1]<(1630 + 200) && adcbuf[1]>(1630 - 200)) adcstate = Menu_noaction;
	else if(adcbuf[1] < (1630 - 1500)){
		adcstate = Menu_up;
		return adcstate;
	} 
	
	
	return adcstate;
	
	
//	return Menu_noaction;
}


#if 1
static bool KeyState(enum MenuState *k)
{
	static enum MenuState KeySt = Menu_noaction;
	
	if(KeySt != Scan())
	{
		HAL_Delay(1);
		if(KeySt != Scan())
		{
			*k = KeySt;
			KeySt = Scan();
			StatusInformationAlways = KeySt;
			return 1;
		}
	}
	return 0;
}
#else
static bool KeyState(enum MenuState *k)
{
	static enum MenuState KeySt = Menu_noaction;
	
	if(KeySt != Scan())
	{
		HAL_Delay(1);
		if(KeySt != Scan())
		{
			KeySt = Scan();
			*k = KeySt;
			StatusInformationAlways = KeySt;
			return 1;
		}
	}
	return 0;
}
#endif
static void EquipmentState(void)
{
	if(InuptEnable == DISABLE) { 
		StatusInformationAlways = Menu_noaction;
		StatusInformation = Menu_noaction;
		return;
	}
	if(KeyState(&StatusInformation)){ 
		if(StatusInformation != Menu_noaction){ 
			MenuRefresh(0); 
		}
	}
}
const unsigned int BUFFWEIGH = SCREENWIDTH;
const unsigned int BUFFHIGH  = DIVIDEUP(SCREENHIGH);
enum ScreenShowManner SCREENSHOWMANNER = ScreenNormal;
enum GraphicsShowManner GRAPHICSSHOWMANNER = GraphicsNormal; 
TypedefScreen ScreenPara = {BUFFWEIGH, BUFFHIGH, 1};
unsigned char DisplayBuff[BUFFWEIGH * BUFFHIGH] = {0};
void ClearnBuff(void)
{
	unsigned int i, j = BUFFWEIGH * BUFFHIGH;
	for(i=0; i<j; i++)
		DisplayBuff[i] = 0;
}

void MenuRefresh(bool mod)
{
	if(mod==0) {
		if(ScreenPara.refresh == 0) {
			ScreenPara.refresh += 1;
		}
	}
	else {
		ScreenPara.refresh += 1;
	}
}
uint32_t MenuMallocSize = 0;
void *MenuMalloc(uint16_t size)
{
	void *addr;
	addr = malloc(size);
	if(addr) {
		MenuMallocSize += size;
	}
	return addr;
}
menu_area *SetMenu(menu_area *target, int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p = target;
	menu_area *k;
	
	if(!p) return NULL;
	if(transfer != NULL)  
	{
		k = FindMeunListTail(transfer); 
		k->next = p;
		p->previous = k;
		p->id = k->id + 1; 
	}
	else 
	{
		p->id = MENUHEARDID;  
		p->previous = NULL; 
	}

	p->x = x;
	p->y = y;
	p->width = width;
	p->high = high;
	p->checked =  checked; 
	p->menulistend = DISABLE; 
	p->next = NULL;      
	p->subclass = NULL;  
	p->father = NULL;    
	p->menuinterface = NULL; 
	p->specialfeatures = NULL; 
	p->specfeattrigflag = NULL;
	p->menu_time = NULL; 
	return p;
}
menu_area * AddToMenuList(int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer)
{
	menu_area *p;
	
	p = (menu_area *) MenuMalloc(sizeof(menu_area)); 
	
	return SetMenu(p, x, y, width, high, checked, transfer);
}
menu_area * FastSimilarMenu(menu_area *target, menu_area *source, uint8_t mod)
{
	if(!source) return NULL;
	if(!target) {
		return SetMenu(source, 0, 0, 0, 0, ENABLE, NULL);
	}
	target = FindMeunListHeard(target);
	menu_area *p = FindMeunListTail(target); 
	
	if(mod == 0)  { 
		if(p->y+p->high*2 > SCREENHIGH) {
			p->menulistend = ENABLE;
			return SetMenu(source, target->x, target->y, target->width,target->high, target->checked, p);
		}
		else {
			return SetMenu(source, p->x, p->y+p->high, p->width,p->high, p->checked, p);
		}
	}
	else if(mod == 1){
		if(p->y+p->high >= SCREENHIGH) {
			p->menulistend = ENABLE;
			return SetMenu(source, target->x, target->y, target->width,target->high, target->checked, p);
		}
		else {
			return SetMenu(source, p->x, p->y+p->high, p->width,p->high, p->checked, p);
		}
	}
	else {
		return SetMenu(source, p->x, p->y+p->high, p->width,p->high, p->checked, p);
	}
}
void BatchFastSimilarMenu(menu_area *target, menu_area source[], uint16_t n, uint8_t mod, void (*menuinterface)(struct MENU_AREA *target))
{
	uint16_t i = 0;
	
	if(target == NULL) return;
	
	if(target==source && n!=0) {
		source = &source[1];
		n-=1;
		target->menuinterface = menuinterface;
	}
	
	if(n>=1) {
		FastSimilarMenu(target, &source[0], mod)->menuinterface=menuinterface;
		for(i=1; i<n; i++) {
			FastSimilarMenu(&source[i-1], &source[i], mod)->menuinterface=menuinterface;
		}
	}
}
void LinkToParentClass(menu_area *father, menu_area *sub)
{
	if(father==NULL || sub==NULL) return; 
	father->subclass = sub;
	sub->father = father;
}
menu_area * MenuListAddressing(menu_area *target, bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; 
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->next == NULL){  
				return NULL;
			}
			p = p->next;
		}
		else
		{
			if(p->previous == NULL){ 
				return NULL;
			}
			p = p->previous;
		}
	}
	return p;
}
menu_area * MenuHeadAddressing(menu_area *target,  bool upordown, uint16_t offset)
{
	menu_area *p = target;
	uint16_t i;
	
	if(target==NULL) return NULL; 
	
	for(i=0; i<offset; i++)
	{
		if(upordown == 0) 
		{
			if(p->subclass == NULL){ 
				return NULL;
			}
			p = p->subclass;
		}
		else
		{
			if(p->father == NULL){
				return NULL;
			}
			p = p->father;
		}
	}
	return p;
}
menu_area *NextCancheMenuList(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num;
	uint16_t i;

	if(p==NULL) return NULL;
	for(i=0; i<j;)
	{
		p=MenuListAddressing(p, num<0, 1);
		if(p==NULL || p==target) return target;
		if(p->checked == 1) i++;
	}
	
	return p;
}
menu_area *NextCancheMenuHeard(menu_area *target, int16_t num)
{
	menu_area *p = target;
	uint16_t j = num<0?-num:num; 
	uint16_t i;

	if(p==NULL) return NULL;
	for(i=0; i<j;)
	{
		p=MenuHeadAddressing(p, num<0, 1); 
		if(p==NULL) return target;
		if(p->checked == 1) i++;
	}
	return p;	
}
menu_area *FindMeunListHeard(menu_area *target) 
{
	menu_area *heard=target;
	
	if(target == NULL) return NULL;
	for( ; ; )
	{
		if(heard->id == MENUHEARDID) break; 
		heard = heard->previous;
	}
	return heard;
}
menu_area *FindMeunListTail(menu_area *target) 
{
	menu_area *tail=target;
	if(target == NULL) return NULL; 
	for( ; ; )
	{
		if(tail->next == NULL) break; 
		if(tail->next->id == MENUHEARDID) break; 
		tail = tail->next;
	}
	return tail;
}
menu_area *MenuListShowHead(menu_area *target)
{
	menu_area *targetmenu = target;
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->id == MENUHEARDID) return targetmenu;
		if(targetmenu->previous->menulistend == ENABLE) return targetmenu; 
		targetmenu = targetmenu->previous;
	}
}
menu_area *MenuListShowTail(menu_area *target)
{
	menu_area *targetmenu = target;
	
	if(targetmenu == NULL) return NULL;
	
	for( ; ; )
	{
		if(targetmenu->next == NULL) return targetmenu; 
		if(targetmenu->next->id == MENUHEARDID) return targetmenu;
		if(targetmenu->menulistend == ENABLE) return targetmenu;
		targetmenu = targetmenu->next;
	}
}
menu_area *FindMenuOfID(menu_area *target, uint16_t id, bool mod)
{
	menu_area *p = target;
	uint16_t i=0;
	if(target == NULL) return NULL;
	if(id < MENUHEARDID) return (mod?NULL:target);
	
	if(id <= target->id) {
		for(i=0; i<(target->id - id); i++) {
			p = p->previous;
		}
		return p;
	}
	else {
		for(i=0; i<(id - target->id); i++) {
			p = p->next;
			if(p==NULL || p->id==MENUHEARDID) return (mod?NULL:target);
		}
		return p;
	}
}
void MakeMenuListRing(menu_area *target)
{
	menu_area *heard, *tail;
	
	if(target == NULL) return; 
	
	heard = FindMeunListHeard(target); 
	tail = FindMeunListTail(target); 
	
	tail->next = heard; 
	heard->previous = tail;
}
void SetMenuTime(menu_area *target, menu_timems *menutime_obj, uint16_t function, uint16_t ms)
{
	if(target == NULL) return; 
	target->menu_time = menutime_obj;
	menutime_obj->counttime=0; 
	menutime_obj->timems = ms; 
	target->specialfeatures |= function; 
}
static void AddToMenuTimeList(menu_area *target, uint16_t ms)
{
	if(target == NULL) return; 
	
	target->menu_time = (menu_timems *) MenuMalloc(sizeof(menu_timems));
	if(!(target->menu_time)) return;
	target->menu_time->counttime=0; 
	target->menu_time->timems = ms; 
}
void AddToSpecialFunction(menu_area *target, uint16_t function, uint16_t ms)
{
	if(target == NULL) return;
	
	if(function & MenuTime) {
		AddToMenuTimeList(target, ms);
	}
	target->specialfeatures |= function; 
}
bool TriggerCheck(menu_area *target, enum SpecialInformation function)
{
	bool triggerflag = 0;
	
	if(!target) return 0;
	if( (target->specfeattrigflag)&function ){
		triggerflag = 1;
		target->specfeattrigflag &= ~function;
	}
	return triggerflag;
}
static MenuListOverall *MenuOverallPointer = NULL;
MenuListOverall *MenuOverall(menu_area *target)
{
	MenuListOverall *MenuOverallP;
	MenuListOverall *Tar = NULL;
	menu_area *p;
	
	p = FindMeunListHeard(target);
	if(!p) return NULL; 
	if(p->specialfeatures & MenuHaveOverall) return NULL; 
	
	Tar = (MenuListOverall *)MenuMalloc(sizeof(MenuListOverall)); 
	if( !(Tar) ) return NULL; 
	p->specialfeatures |= MenuHaveOverall; 
	
	if(MenuOverallPointer==NULL){ 
		MenuOverallPointer = Tar;
	}
	else{
		MenuOverallP=MenuOverallPointer;
		while(MenuOverallP->next){
			MenuOverallP = MenuOverallP->next;
		}
		MenuOverallP->next = Tar;
	}
	Tar->Affiliation = p;
	Tar->menuinterface = NULL;
	Tar->next = NULL;
	
	return Tar;
}
static void ChangeMenuY(menu_area *target, int16_t showSY, int16_t showEY, int16_t y_dat)
{
	menu_area *heard = NULL;
	menu_area *tail = NULL;
	
	if(!target) return;
	heard = FindMeunListHeard(target);
	tail = FindMeunListTail(target);
	for( ; ;) {
		heard->y += y_dat;
		if( heard->y < showSY  || (heard->y + heard->high) >= showEY || (heard->next ? heard->next->y + heard->next->high + y_dat > showEY : 0)) {
			heard->menulistend = ENABLE;
		}
		else{
			heard->menulistend = DISABLE;
		}
		if(heard == tail) break;
		heard = heard->next;
	}
}
void ScrollingDisplay_Y(menu_area *target, int16_t showSY, uint8_t showEY, int16_t TarSY, int16_t TarEY) 
{
	menu_area *p;
	p = FindMeunListHeard(target);
	if( !(p->specialfeatures & MenuScrolling) ) {
		p->specialfeatures |= MenuScrolling;
		ChangeMenuY(target, showSY, showEY, 0);
	}
	if(showSY > TarSY) showSY = TarSY;
	if(TarEY > showEY) TarEY = showEY;
	if(target->y < TarSY) {
		ChangeMenuY(target, showSY, showEY, TarSY-target->y);
	}
	else if(target->y+target->high-1 > TarEY) {
		ChangeMenuY(target, showSY, showEY, TarEY-target->y - target->high);
	}
}
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b)
{
	int16_t px=0, py=0; 
	if(target == NULL) return;
	px = target->x + x;
	py = target->y + y;
	
	if(x>=target->width || y>=target->high) return; 
	if(x<0 || y<0) return;
	write_point(px, py, w_b);
}
static void DrawMenuRectangle(menu_area *target)
{	
	if(target->width<=0 || target->high<=0) return;
	if(target->x>=SCREENWIDTH || target->y>=SCREENHIGH) return;
	if(target->x+target->width<0 || target->y+target->high<0) return;
	DrawfillRoundRect(target->x, target->y, target->width, target->high, 3);
}
menu_area * TargetMenu = NULL; 
static menu_area *MenuListShowHeadForHeart(menu_area *target)
{
	menu_area *targetmenu = target;
	if(targetmenu == NULL) return NULL;
	for( ; ; )
	{
		if(targetmenu->previous == NULL) return targetmenu; 
		if(targetmenu->id == MENUHEARDID) return targetmenu;
		if(targetmenu->previous->menulistend == ENABLE) return targetmenu; 
		targetmenu = targetmenu->previous;
	}
}
static menu_area *MenuListShowTailForHeart(menu_area *target)
{
	menu_area *targetmenu = target;
	if(targetmenu == NULL) return NULL;
	for( ; ; )
	{
		if(targetmenu->next == NULL) return targetmenu; 
		if(targetmenu->next->id == MENUHEARDID) return targetmenu;
		if(targetmenu->menulistend == ENABLE) return targetmenu;
		targetmenu = targetmenu->next;
	}
}
static menu_area *FindMeunListHeardForHeart(menu_area *target) 
{
	menu_area *heard=target;
	if(target == NULL) return NULL;
	for( ; ; )
	{
		if(heard->id == MENUHEARDID) break; 
		heard = heard->previous;
	}
	return heard;
}
static menu_area *FindMeunListTailForHeart(menu_area *target) 
{
	menu_area *tail=target;
	if(target == NULL) return NULL; 
	for( ; ; )
	{
		if(tail->next == NULL) break; 
		if(tail->next->id == MENUHEARDID) break; 
		tail = tail->next;
	}
	return tail;
}
bool MenuHeartTimeStart = DISABLE;
static bool RefreshFlagForHeart = DISABLE; 
static void MenuHeartTime(void)
{
	menu_area *p = TargetMenu; 
	menu_area *MenuTail = NULL; 
	menu_area *MenuShowHeard = NULL;
	menu_area *MenuShowTail = NULL;
	uint8_t flag = 0;
	if(p==NULL) return; 
	MenuShowHeard = MenuListShowHeadForHeart(p);
	MenuShowTail = MenuListShowTailForHeart(p);
	p = FindMeunListHeardForHeart(p);
	MenuTail = FindMeunListTailForHeart(p);
	for( ; ; )
	{
		if(p->menu_time != NULL){	
			if(p == MenuShowHeard) flag = 1;
			if(flag) {
				if( (p->specialfeatures)& MenuTimeForce ){
					p->menu_time->counttime++; 
					if( (p->menu_time->counttime) == p->menu_time->timems){ 
						p->menu_time->counttime=0; 
						p->specfeattrigflag |= MenuTime; 
						if(p->menuinterface){
							p->menuinterface(p); 
						}
						RefreshFlagForHeart = ENABLE;
					}
				}
				else {
					p->menu_time->counttime++; 
					if( (p->menu_time->counttime) == p->menu_time->timems){ 
						p->menu_time->counttime=0; 
						p->specfeattrigflag |= MenuTime; 
						RefreshFlagForHeart = ENABLE;
					}
				}
			}
			else {
				if( (p->specialfeatures) & MenuTimeForce ){
					p->menu_time->counttime++; 
					if( (p->menu_time->counttime) == p->menu_time->timems){ 
						p->menu_time->counttime=0; 
						p->specfeattrigflag |= MenuTime; 
						if(p->menuinterface){
							p->menuinterface(p); 
						}
						RefreshFlagForHeart = ENABLE;
					}
				}
			}
			if(p == MenuShowTail) flag = 0;
		}
		if(p==MenuTail) break;
		p=p->next;
	}
}
void MenuTicker_ms(void)
{
	if(MenuHeartTimeStart==DISABLE) return; 
	MenuHeartTime();
	FunctionTickerRunIRQ();
}
static void MenuListInterface(menu_area *target)
{
	menu_area *p = NULL;
	menu_area *pTail = NULL;
	p = MenuListShowHead(target); 
	pTail = MenuListShowTail(target);
	for( ; ; )
	{
		if(p->menuinterface){
			p->menuinterface(p);
		}
		if(p == pTail) break; 
		p = p->next;
	}
}
static void SpecialFunctionRun(menu_area *target)
{
	static menu_area *lastmenuheard = NULL; 
	menu_area *lastmenutail = FindMeunListTail(lastmenuheard); 
	static menu_area *lastshowmenuheard = NULL;
	menu_area *lastshowmenutail = MenuListShowTail(lastshowmenuheard);	
	menu_area *nowmenuheard = FindMeunListHeard(target); 
	menu_area *nowmenutail = FindMeunListTail(target);
	menu_area *nowshowmenuheard = MenuListShowHead(target);
	menu_area *nowshowmenutail = MenuListShowTail(target);
	menu_area *p = NULL;
	uint8_t flag = 0;
	if(!lastmenuheard){
		lastmenuheard = nowmenuheard;
		lastmenutail = FindMeunListTail(lastmenuheard); 
		lastshowmenuheard = nowshowmenuheard;
		lastshowmenutail = MenuListShowTail(lastshowmenuheard);	
	}
	if( lastmenuheard != nowmenuheard){ 
		for( p=lastmenuheard; ; ){ 
			if(p->specialfeatures & ExitMenu) {
				p->specfeattrigflag |= ExitMenu;
				p->menuinterface(p);
			}
			if(p==lastshowmenuheard){
				flag = 1;
			}
			if(flag) {
				if(p->specialfeatures & ExitShowMenuList) {
					p->specfeattrigflag |= ExitShowMenuList;
					p->menuinterface(p);
				}
			}
			if(p == lastshowmenutail) {
				flag = 0;
			}
			if(p == lastmenutail) break;
			p = p->next;
		}
		for(p=nowmenuheard; ; ){
			if(p->specialfeatures & EnterMenu) {
				p->specfeattrigflag |= EnterMenu;
				p->menuinterface(p);
			}
			if(p==nowshowmenuheard){
				flag = 1;
			}
			if(flag) {
				if(p->specialfeatures & EnterShowMenuList) {
					p->specfeattrigflag |= EnterShowMenuList;
					p->menuinterface(p);
				}
			}
			if(p == nowshowmenutail) {
				flag = 0;
			}
			if(p == nowmenutail) break;
			p = p->next;
		}
		lastmenuheard = nowmenuheard;
		lastshowmenuheard = nowshowmenuheard;
		ClearnBuff(); 
		return;
	}
	if(lastshowmenuheard != nowshowmenuheard){ 
		for(p=lastshowmenuheard; ; ) { 
			if(p->specialfeatures & ExitShowMenuList) {
				p->specfeattrigflag |= ExitShowMenuList;
				p->menuinterface(p);
			}
			if(p == lastshowmenutail) break;
			p = p->next;
		}
		for(p=nowshowmenuheard; ; ) { 
			if(p->specialfeatures & EnterShowMenuList) {
				p->specfeattrigflag |= EnterShowMenuList;
				p->menuinterface(p);
			}
			if(p == nowshowmenutail) break;
			p = p->next;
		}
		lastshowmenuheard = nowshowmenuheard;
		ClearnBuff(); 
	}
}
static void MenuListOverallRun(menu_area *target)
{
	menu_area *p;
	MenuListOverall *MenuListOverallP=MenuOverallPointer;
	if(!MenuListOverallP) return; 
	p = FindMeunListHeard(target);
	if( !(p->specialfeatures & MenuHaveOverall) ) return; 
	while(MenuListOverallP->Affiliation != p){ 
		if( !(MenuListOverallP->next) ) return;
		MenuListOverallP = MenuListOverallP->next;
	}
	if(MenuListOverallP->menuinterface){
		MenuListOverallP->menuinterface();
	}
}
static void StateToPointer(void)
{
	menu_area *p = NULL;
	if(ResponseEnable == DISABLE) return;
	switch(StatusInformation)
	{
		case Menu_up: 		TargetMenu=NextCancheMenuList(TargetMenu, -1);break;
		case Menu_down: 	TargetMenu=NextCancheMenuList(TargetMenu,  1);break;
		case Menu_Sub:  
			if(TargetMenu->subclass != NULL) {
				StatusInformation = Menu_noaction; 
				TargetMenu=NextCancheMenuHeard(TargetMenu, 1);
			}
			break;
		case Menu_Father:  
			#if 0
				if(TargetMenu->father != NULL) {
					StatusInformation = Menu_noaction; 
					TargetMenu=NextCancheMenuHeard(TargetMenu, -1);
				}
			#else
				p = FindMeunListHeard(TargetMenu);
				if(p->father != NULL) {
					StatusInformation = Menu_noaction; 
					TargetMenu=NextCancheMenuHeard(p, -1);
				}
			#endif
			break;
		default:break;
	}
}
static void MenuCheckedStyle(menu_area *target)
{
	GRAPHICSSHOWMANNER = GraphicsRollColor;
	
	DrawMenuRectangle(target);
	
	GRAPHICSSHOWMANNER = GraphicsNormal;
}
void MenuRun(void)
{
	EquipmentState(); 
	FunctionTickerRun();
	if(RefreshFlagForHeart == ENABLE){
		RefreshFlagForHeart = DISABLE;
		MenuRefresh(0);
	}
	if((ScreenPara.refresh) && TargetMenu)
	{
		StateToPointer(); 
		ClearnBuff(); 
		MenuAlwaysRun_PH();
		SpecialFunctionRun(TargetMenu); 
		MenuListOverallRun(TargetMenu); 
		MenuListInterface(TargetMenu); 
		MenuCheckedStyle(TargetMenu);
		MenuAlwaysRun_PL();
		disp_flush();
		
		StatusInformation = Menu_noaction; 
		if(ScreenPara.refresh != 0) ScreenPara.refresh--;
	}
	AlwaysRun();
}
static FunctionTicker *FunctionTickerPointer = NULL;
FunctionTicker *SetFunctionTicker(FunctionTicker *FTtarget, uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(void))
{
	if(FTtarget == NULL) return NULL;
	
	if(FunctionTickerPointer == NULL) {
		FunctionTickerPointer = FTtarget;
	}
	else {
		FunctionTicker *Tail = FunctionTickerPointer;
		
		while(Tail->next) {
			Tail=Tail->next;
		}
		Tail->next = FTtarget;
	}
	FTtarget->next = NULL;
	FTtarget->run = ENABLE;
	FTtarget->Flag = DISABLE;
	
	FTtarget->ms = ms;
	FTtarget->count = 0;
	FTtarget->RunMod = RunMod;
	FTtarget->Function = Function;
	
	return FTtarget;
}
FunctionTicker *AddToFunctionTicker(uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(void))
{
	FunctionTicker *FTtarget;
	
	FTtarget = MenuMalloc(sizeof(FunctionTicker));
	
	if(FTtarget == NULL) return NULL;
	
	return SetFunctionTicker(FTtarget, ms, RunMod, Function);
}
void FunctionTickerRunIRQ(void)
{
	FunctionTicker *p = FunctionTickerPointer;
	while(p)
	{
		if(p->run == ENABLE) {
			if(++p->count >= p->ms) {
				p->count=0;
				if(p->RunMod == interruptRun) {
					if(p->Function) p->Function();
				}
				else if(p->RunMod == normalRun) {
					p->Flag = ENABLE;
				}
			}
		}
		p=p->next;
	}
}
void FunctionTickerRun(void)
{
	FunctionTicker *p = FunctionTickerPointer;
	
	while(p) {
		if(p->Flag == ENABLE) {
			p->Flag = DISABLE;
			if(p->Function) p->Function();
		}
		p=p->next;
	}
}
bool tRunOne(TypeRunOne *RunOne, bool t, unsigned int dat1,unsigned int dat2)
{
	if(!t) 
	{
		if(dat1 != dat2)
		{
			RunOne->floag = 1;
		}		
		if(RunOne->floag==1 && (dat1 == dat2))
		{
			RunOne->floag = 0;
			return 1;
		}
	}
	else 
	{
		if(RunOne->dat != dat2)
		{
			RunOne->dat = dat2;
			return 1;
		}
	}
	return 0;
}









