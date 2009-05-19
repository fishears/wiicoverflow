#ifndef _Button_H_
#define _Button_H_

//comment out for Localization-Buttons
#define LOCALBUTTON_OFF

#include "GRRLIB.h"


#ifndef LOCALBUTTON_OFF
//Button-Type
	#define BTN_NORMAL      1
	#define BTN_NORMAL_HOV  2
	#define BTN_BIG         3
	#define BTN_BIG_HOV     4
	#define BTN_TOOLTIP		5
#endif

typedef struct Button {
	int x;
	int y;
	
	struct GRRLIB_texImg texture;
	struct GRRLIB_texImg hoverTexture;
		
	bool hovering;
#ifndef 	LOCALBUTTON_OFF
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

#ifndef 	LOCALBUTTON_OFF
void Button_Overlay_Text(f32 xpos, f32 ypos, int btnType, char *text);
#endif

#endif //_BUTTON_H_

