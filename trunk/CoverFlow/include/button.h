#ifndef _Button_H_
#define _Button_H_

#include "GRRLIB.h"

typedef struct Button {
	int x;
	int y;
	
	struct GRRLIB_texImg texture;
	struct GRRLIB_texImg hoverTexture;
		
	bool hovering;
	
} Button;

Button Button_Init(const unsigned char normal_img[], const unsigned char hover_img[], int x, int y);
bool Button_Hover(struct Button* btn, int x, int y);
bool Button_Select(struct Button* btn, int x, int y);
void Button_Paint(struct Button* btn);
void Button_Theme_Paint(struct Button* btn, int theme_id);

#endif //_BUTTON_H_

