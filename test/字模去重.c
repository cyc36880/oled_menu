#include <stdio.h> 

unsigned char font[] = "我的世界我的可口一个怎的，12"; //字符 

void ShowHz(unsigned char *font); 

int main()
{
	unsigned int fontnum = sizeof(font); //字体文件总大小 
	
	for(unsigned int i=0; i<fontnum; i+=2){
		
		if(font[i] <= 0x7f){
			i++;
			continue;
		}
		for(unsigned int j=i+2; j<fontnum;){
			if(font[j] <= 0x7f){
				j++;
				continue;
			}
			if(font[i]==font[j] && font[i+1]==font[j+1]){
				font[j] = 0;
				font[j+1] = 0;
			}
			j+=2;
		}
	}
	
	for(unsigned int i=0; i<fontnum;){
		if(font[i]>0x7f) {
			ShowHz(font + i);
			i+=2;
		}
		else{
			i++;
		}
	}
	
	return 0;
}

void ShowHz(unsigned char *font)
{
	unsigned char buf[3] = {0, 0, 0};
	
	buf[0] = font[0];
	buf[1] = font[1];
	
	printf("%s", buf);
}
