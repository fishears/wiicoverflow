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

Button Button_Init(u8* normal_img, u8* hover_img, int x, int y);
bool Button_Hover(struct Button* btn, int x, int y);
bool Button_Select(struct Button* btn, int x, int y);
void Button_Paint(Button btn);

#endif //_BUTTON_H_

