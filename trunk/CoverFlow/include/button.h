/*
 *  button.h
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 */
#ifndef _Button_H_
#define _Button_H_

#include "GRRLIB.h"

typedef struct Button {
	int		x;
	int		y;
	char	ttf_label[15];
	bool	selected;	
	bool	hovering;
	bool	show_reflection;
	struct	GRRLIB_texImg texture;
	struct	GRRLIB_texImg hoverTexture;
} Button;

// Button Init methods
Button Button_Init(const unsigned char normal_img[], const unsigned char hover_img[], int x, int y);
Button Button_TTF_Init(const unsigned char normal_img[], const unsigned char hover_img[], int x, int y, char *button_label);
Button Button_Flag_Init(const unsigned char normal_img[], int x, int y, char *button_label);
// Button actions
bool Button_Hover(struct Button* btn, int x, int y);
bool Button_Select(struct Button* btn, int x, int y);
bool Button_Select_Mute(struct Button* btn, int x, int y);
// Button drawing
void Button_Paint(struct Button* btn);      // this is used to print buttons with no TTF on them (-/+, logo, etc.)
void Button_Menu_Paint(struct Button* btn); // this prints the large Settings menu buttons (top and bottom headers)
void Button_TTF_Paint(struct Button* btn);  // this is used to print the TTF buttons
void Button_Flag_Paint(struct Button* btn); // this is used to print the flag buttons on the lang panel
void Button_Toggle_Paint(struct Button* btn1, struct Button* btn2, int toggle_state);
void Button_HomeMenu_Paint(struct Button* btn); // this is used to paint the big fat buttons on the 'home' button menu
void Button_TTF_Toggle_Paint(struct Button* btn1, struct Button* btn2, char *left_text, char *right_text, int toggle_state); // prints the TTG toggle buttons (yes.no, etc)
void Button_Theme_Paint(struct Button* btn, int theme_id); // Flips the state of the button based on theme

void FreeButtonResources(struct Button *btn);

#endif //_BUTTON_H_

