#ifndef _menufontshow_h_
#define _menufontshow_h_

#include <stdint.h>
#include "menu.h"

//设置英文显示字体
void SetFont(const uint8_t *xfont,const uint8_t *xfont_SizeInf);

//功能：在菜单中写Asc字符，target为NULL在屏幕写
void MenuShowAsc(menu_area *target, int16_t x, int16_t y, uint8_t asc);

//功能：在菜单中写Asc字符串，target为NULL在屏幕写
void MenuShowAscStr(menu_area *target, int16_t x, int16_t y, uint8_t *str);

//功能：在菜单中写Asc数字，target为NULL在屏幕写
void MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num);

// 汉字 Asc 混合显示，target为NULL在屏幕写
void MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, uint8_t *s_dat);

#endif



