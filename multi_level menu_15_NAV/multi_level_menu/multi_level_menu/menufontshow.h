#ifndef _menufontshow_h_
#define _menufontshow_h_

#include "menu.h"
#include "menuconfig.h"

extern const uint8_t *font_SizeInf;
void SetFont(const uint8_t *xfont);
void MenuShowAsc(menu_area *target, int16_t x, int16_t y, uint8_t asc);
void MenuShowAscStr(menu_area *target, int16_t x, int16_t y, const uint8_t *str);
void MenuHzAndAsc(menu_area *target, int16_t x, int16_t y, const uint8_t *s_dat);
void CharacterText(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str);
void CharacterTextC(menu_area *target, int16_t x, int16_t y, uint16_t ascw, const uint8_t *str);
void MenuShowNum(menu_area *target, int16_t x, int16_t y, uint8_t len, uint32_t num);
#define MENUSERIALWINDOWWIDTH   10 
#define MENUSERIALWINDOWHIGH   4 
#define MenuSerialBufSzie  (MENUSERIALWINDOWWIDTH * MENUSERIALWINDOWHIGH + 1) 
extern uint16_t MenuSerialRxNum; 
void MenuPaddingSerialBuf(uint8_t dat); 
void SerialCharacterText(menu_area *target, int16_t x, int16_t y);
void ClearnSerialShowBuf(void);
uint8_t SerStrMatching(const uint8_t *str);
#endif


