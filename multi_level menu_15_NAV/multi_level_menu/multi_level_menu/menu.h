#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h> 
#include <stdlib.h>  
#include <stdbool.h> 

#include "oled.h"
#include "menuconfig.h"
#define MENUHEARDID 1 
#ifndef ENABLE
	#define ENABLE  1 
#endif
#ifndef DISABLE
	#define DISABLE 0 
#endif
enum MenuState
{
	Menu_noaction=0, 
	Menu_up,  		
	Menu_down, 		 
	Menu_Sub,       
	Menu_Father,     
	Menu_confirm 	
};
enum SpecialInformation
{
	MenuTime = 0x01,
	EnterMenu = 0x02, 
	ExitMenu = 0x04,  
	EnterShowMenuList = 0x08, 
	ExitShowMenuList = 0x10, 

	MenuTimeForce = 0x20, 
	MenuHaveOverall = 0x40,
	MenuScrolling   = 0x80,
};
typedef struct MENU_TIMEMS
{
	uint16_t counttime;
	uint16_t timems;    
}menu_timems;
typedef struct MENU_AREA
{
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t high;
	uint16_t id;
	bool checked; 
	bool menulistend; 
	struct MENU_AREA *next;     
	struct MENU_AREA *previous; 
	struct MENU_AREA *subclass;
	struct MENU_AREA *father;   
	void (*menuinterface)(struct MENU_AREA *target);
	menu_timems *menu_time;
	uint8_t specialfeatures;
	uint8_t specfeattrigflag; 
}menu_area;
typedef struct MENULISTOVERALL 
{
	menu_area *Affiliation; 
	void (*menuinterface)(void); 
	struct MENULISTOVERALL *next;
}MenuListOverall;


menu_area *AddToMenuList(int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer);
menu_area *SetMenu(menu_area *target, int16_t x, int16_t y, uint16_t width, uint16_t high, bool checked, menu_area *transfer);
menu_area *FastSimilarMenu(menu_area *target, menu_area *source, uint8_t mod);
void BatchFastSimilarMenu(menu_area *target, menu_area source[], uint16_t n, uint8_t mod, void (*menuinterface)(struct MENU_AREA *target));
void LinkToParentClass(menu_area *father, menu_area *sub);
menu_area *MenuListAddressing(menu_area *target, bool upordown, uint16_t offset);
menu_area *MenuHeadAddressing(menu_area *target, bool upordown, uint16_t offset);
menu_area *NextCancheMenuList(menu_area *target, int16_t num);
menu_area *NextCancheMenuHeard(menu_area *target, int16_t num);
menu_area *FindMeunListHeard(menu_area *target);
menu_area *FindMeunListTail(menu_area *target);
menu_area *MenuListShowHead(menu_area *target);
menu_area *MenuListShowTail(menu_area *target);
menu_area *FindMenuOfID(menu_area *target, uint16_t id, bool mod);
void MakeMenuListRing(menu_area *target);
MenuListOverall *MenuOverall(menu_area *target);
void SetMenuTime(menu_area *target, menu_timems *menutime_obj, uint16_t function, uint16_t ms);
void AddToSpecialFunction(menu_area *target, uint16_t function, uint16_t ms);
bool TriggerCheck(menu_area *target, enum SpecialInformation function);
void ScrollingDisplay_Y(menu_area *target, int16_t showSY, uint8_t showEY, int16_t TarSY, int16_t TarEY);
extern menu_area * TargetMenu; 
extern enum MenuState StatusInformation; 
extern enum MenuState StatusInformationAlways; 
extern uint8_t InuptEnable; 
extern uint8_t ResponseEnable; 
extern uint32_t MenuMallocSize;
#define MenuCenterX(target,w) ((target->width - (w)) / 2)
#define MenuCenterY(target,h) ((target->high - (h)) / 2)
void MenuAlwaysRun_PH(void);
void MenuAlwaysRun_PL(void);
void AlwaysRun(void);
#define SCREENWIDTH 128 
#define SCREENHIGH  64  
#define DIVIDEUP(x) (x*10/8%10 ? x/8+1:x/8)
enum ScreenShowManner 
{
	ScreenNormal,  
	ScreenRollback 
};
enum GraphicsShowManner 
{
	GraphicsNormal,  
	GraphicsCover, 
	GraphicsRollColor 
};
typedef struct
{
	const uint16_t screenwidth; 
	const uint16_t screenhigh;  
	uint8_t refresh; 
}TypedefScreen;
extern enum ScreenShowManner SCREENSHOWMANNER; 
extern enum GraphicsShowManner GRAPHICSSHOWMANNER;
extern unsigned char DisplayBuff[]; 
extern TypedefScreen ScreenPara;
void MenuRefresh(bool mod);
void MenuSetPoint(menu_area *target, int16_t x, int16_t y, bool w_b); 
void ClearnBuff(void);
#define MenuScreenCenterX(width) ((SCREENWIDTH - (width)) / 2)
#define MenuScreenCenterY(high)  ((SCREENHIGH - (high)) / 2)
void MenuRun(void); 
void MenuTicker_ms(void);
extern bool MenuHeartTimeStart; 
enum FUNCTINOTICKEROPTIONS
{
	normalRun = 0,
	interruptRun,
};
typedef struct FUNCTINOTICKER
{
	uint32_t count; 
	uint32_t ms;  
	struct FUNCTINOTICKER *next;
	void (*Function)(void);
	enum FUNCTINOTICKEROPTIONS RunMod; 
	bool Flag;
	bool run;
}FunctionTicker;
FunctionTicker *SetFunctionTicker(FunctionTicker *FTtarget, uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(void));
FunctionTicker *AddToFunctionTicker(uint32_t ms, enum FUNCTINOTICKEROPTIONS RunMod, void (*Function)(void));
typedef struct RUNONE
{
	unsigned int dat;
	unsigned char floag;
}TypeRunOne; 
bool tRunOne(TypeRunOne *RunOne, bool t, unsigned int dat1,unsigned int dat2);
#endif




