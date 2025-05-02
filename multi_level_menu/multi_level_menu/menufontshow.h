#ifndef _menufontshow_h_
#define _menufontshow_h_

#include <stdint.h>
#include "menu.h"


//功能：在菜单中写Asc字符，target为NULL在屏幕写
void MenuShowAsc(menu_area *target, const uint8_t *font, const uint8_t *font_SizeInf, uint8_t x, uint8_t y, uint8_t asc);

//功能：在菜单中写Asc字符串，target为NULL在屏幕写
void MenuShowAscStr(menu_area *target, const uint8_t *font, const uint8_t *font_SizeInf, uint8_t x, uint8_t y, uint8_t *str);

//功能：在菜单中写Asc数字，target为NULL在屏幕写
void MenuShowNum(menu_area *target, const uint8_t *font, const uint8_t *font_SizeInf, uint8_t x, uint8_t y, uint8_t len, uint32_t num);

// 汉字显示，target为NULL在屏幕写
void MenuHZ16x16Str(menu_area *target, uint8_t x, uint8_t y, uint8_t *s_dat);

// 汉字 Asc 混合显示，target为NULL在屏幕写
void MenuHzAndAsc(menu_area *target, uint8_t x, uint8_t y, uint8_t *s_dat);

#endif
