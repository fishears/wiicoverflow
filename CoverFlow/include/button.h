#ifndef _Button_H_
#define _Button_H_

//comment out for Localization-Buttons
#define LOCALBUTTON 

#include "GRRLIB.h"


typedef struct Button {
	int x;
	int y;
	
	struct GRRLIB_texImg texture;
	struct GRRLIB_texImg hoverTexture;
		
	bool hovering;
#ifndef 	LOCALBUTTON
	char label[15];  //add for Localization
#endif	
} Button;

Button Button_Init(const unsigned char normal_img[], const unsigned char hover_img[], int x, int y);
bool Button_Hover(struct Button* btn, int x, int y);
bool Button_Select(struct Button* btn, int x, int y);
bool Button_Select_Mute(struct Button* btn, int x, int y);
void Button_Paint(struct Button* btn);
void Button_Theme_Paint(struct Button* btn, int theme_id);
void Button_Toggle_Paint(struct Button* btn1, struct Button* btn2, int toggle_state);

#endif //_BUTTON_H_

