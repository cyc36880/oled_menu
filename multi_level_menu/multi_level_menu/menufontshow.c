#include "menufontshow.h"
#include "oled.h"

extern const unsigned char F8X16_SizeInf[2]; //下面字符编码的尺寸信息 宽x高
//阴码 列行式 逆向
extern const unsigned char F8X16[];
//汉字库索引
extern const unsigned char HZK16x16index[];

//汉字库 阴码 行列式 逆向 16x16
extern const unsigned char HZK16x16[][16];

/*
	功能：在菜单中写Asc字符，target为NULL在屏幕写
	target：目标菜单
	font：字体数组
	font_SizeInf：字体宽高数组
	x,y：在目标菜单的相对偏移
	asc：一个ASCII字符
*/
void MenuShowAsc(menu_area *target,const uint8_t *font,const uint8_t *font_SizeInf, uint8_t x, uint8_t y, uint8_t asc)
{
	uint8_t c=0;
	uint8_t h,w;
	bool w_b=0;
	uint8_t wight = font_SizeInf[0];
	uint8_t high  = font_SizeInf[1];
	
	c=asc-' ';   // ' '=32,ASCII码表
	
	for(h=0; h<high; h++)//字高
	{
		for(w=0; w<wight; w++) //字宽
		{
			w_b = ( font[c*wight*DIVIDEUP(high) + w+wight*(h/8)] >> (h%8)) & 1;
			if(w_b) {
				if(target == NULL) write_point(w+x, h+y, 1);
				else               MenuSetPoint(target, w+x, h+y, 1);
			}
		}
	}
}

//m^n函数
static uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}	

/*
	功能：在菜单中写Asc数字，target为NULL在屏幕写
	target：目标菜单
	font：字体数组
	font_SizeInf：字体宽高数组
	x,y：在目标菜单的相对偏移
	len：数字长度
	num：数字
*/
void MenuShowNum(menu_area *target, const uint8_t *font, const uint8_t *font_SizeInf, uint8_t x, uint8_t y, uint8_t len, uint32_t num)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;	
	
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				MenuShowAsc(target, font, font_SizeInf, x+font_SizeInf[0]*t, y, ' ');
				continue;
			}else enshow=1; 
		}
		MenuShowAsc(target, font, font_SizeInf, x+font_SizeInf[0]*t, y, temp+'0');
	}
} 

/*
	功能：在菜单中写Asc字符串，target为NULL在屏幕写
	target：目标菜单
	font：字体数组
	font_SizeInf：字体宽高数组
	x,y：在目标菜单的相对偏移
	str：一个ASCII字符串
*/
void MenuShowAscStr(menu_area *target, const uint8_t *font, const uint8_t *font_SizeInf, uint8_t x, uint8_t y, uint8_t *str)
{
	while(*str != '\0')
	{
		MenuShowAsc(target, font, font_SizeInf, x, y, *str);
		str++;
		x+=font_SizeInf[0];
	}
}




// ---------------- 汉 字 显 示------------------------

/*
	功能：测量以\0为结尾的字符串长度
*/
static unsigned int mystrlen(const uint8_t *str)
{
	unsigned int i=0;
	for(i=0; str[i] != '\0'; i++);
	return i;
}

/*
	功能：测量以\0为结尾的 汉字与字符 的字符串长度
*/
static unsigned int HZStrLen(uint8_t *str) 
{
	unsigned int StrLen = mystrlen(str);
	unsigned int i=0;
	unsigned int strnum=0;
	for(i=0; i<StrLen;)
	{
		if((unsigned char)str[i]>127) 
			i+=2;
		else
			i++;
		strnum++;
	}
	return strnum;
}

/*
	功能：从已知字符串中找想要的字符的位置
	formfind：目标字符串
	find：想要寻找的字符所在的字符串
	num：find中第num的字符
	返回值：寻找字符在formfind中的位置
*/
static unsigned char StrIfHaveStr(const uint8_t *formfind, uint8_t *find, unsigned int num)
{
	unsigned int findLen = mystrlen(find);
	unsigned int formfindlen = mystrlen(formfind);
	unsigned int i=0, j=0;
	unsigned int strnum=0, HZflag=0;
	
	if(num >= findLen) return 0;
	if(formfindlen==0 || findLen==0) return 0;
	for(strnum=0, i=0; strnum<num+1; )
	{
		if((unsigned char)find[i]>127){
			i+=2;
			if(strnum==num) HZflag=1;
		}
		else{
			i++;	
		}
		strnum++;
	}
	for(strnum=0, j=0; strnum<formfindlen; )
	{
		strnum++;
		if(HZflag)
		{
			if(formfind[j]==find[i-2] && formfind[j+1]==find[i-1]) 
				return strnum;
		}
		else
		{
			if(formfind[j]==find[i-1])
				return strnum;	
		}
		if((unsigned char)formfind[j]>127){
			j+=2;
		}
		else{
			j++;	
		}	
	}
	return 0;
}
// 汉字显示，target为NULL在屏幕写
void MenuHZ16x16Str(menu_area *target, uint8_t x, uint8_t y, uint8_t *s_dat)
{
	unsigned char i0=0, s_datlen = HZStrLen(s_dat);
	unsigned char j0=0;
	
	uint8_t i,j;
	
	for(i0=0; i0<s_datlen; i0++)
	{
		j0=StrIfHaveStr(HZK16x16index, s_dat, i0);
		if(j0 == 0) continue; // 没有找到

		for(j=0; j<16; j++)
		{
			for(i=0; i<16; i++)
			{
				if((HZK16x16[(j0-1)*2 + j/8][i]) & (0x01<<(j%8))){
					if(target == NULL) write_point(x+i, y+j, 1);
					else               MenuSetPoint(target,x+i, y+j, 1);
				}
			}
		}
	}
}
// 汉字 Asc 混合显示，target为NULL在屏幕写
void MenuHzAndAsc(menu_area *target, uint8_t x, uint8_t y, uint8_t *s_dat)
{
	unsigned int s_datlen = mystrlen(s_dat);
	unsigned int i=0;
	unsigned char HZShowBuf[] = {0,0,0}; // 必须三位，以0结尾
	
	for(i=0; i<s_datlen; )
	{
		if(s_dat[i] > 127) //汉字
		{
			HZShowBuf[0]=s_dat[i];
			HZShowBuf[1]=s_dat[i+1];
			MenuHZ16x16Str(target, x, y, HZShowBuf);
			x+=16; // 坐标右移 由汉字宽度决定
			i+=2;  // 字符串位置标志右移，汉字固定为2
		}
		else   //Asc 
		{
			MenuShowAsc(target, F8X16, F8X16_SizeInf, x, y, s_dat[i]);
			x+=8; //坐标右移 由Asc宽度决定
			i++;  //字符串位置标志右移，Asc固定为1
		}
	}
}




