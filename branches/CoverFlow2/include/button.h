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

#include "defines.h"
#include "GRRLIB.h"



typedef struct Button {
	int		x;
	int		y;
	char	ttf_label[15];
	bool	selected;	
	bool	hovering;
	bool	show_reflection;
	struct	GRRLIB_texImg texture;
	bool 	toFreeTexture;
	struct	GRRLIB_texImg hoverTexture;
	bool 	toFreeHoverTexture;
} Button;


#ifdef NEWS_READER
typedef struct Button3 {
	int		x;
	int		y;
	char	ttf_label[15];
	bool	selected;	
	bool	hovering;
	bool	show_reflection;
	struct	GRRLIB_texImg texture;
	struct	GRRLIB_texImg hoverTexture;
	struct	GRRLIB_texImg TriStateTexture;
	bool 	toFreeTexture;
	bool 	toFreeHoverTexture;
	bool	toFreeTriStateTexture;
} Button3;

// Button3 Init method
void Button3_Init(Button3 * new_button, const unsigned char normal_img[], const unsigned char hover_img[], const unsigned char triState_img[], int x, int y);
// Button3 actions
bool Button3_Hover(struct Button3* btn, int x, int y);
bool Button3_Select(struct Button3* btn, int x, int y);
// Button3 drawing
void Button3_Theme_Paint(struct Button3* btn, int theme_id); // Flips the state of the button based on theme
void Button3_Theme_Paint_Offset(struct Button3* btn, int theme_id, int x_offset, int y_offset); // Flips the state of the button based on theme
void Button3_Theme_Paint_Fade(struct Button3* btn, int theme_id, int fade); // Flips the state of the button based on theme

void FreeButton3Resources(struct Button3 *btn);
#endif


#ifdef OSK
void Button_Key_Init(Button * new_button, const unsigned char normal_img[], int x, int y, char *button_label);
void Duplicate_Button_Key(Button * destination_button, Button btn, int x, int y, char *button_label);
void Button_Key_Paint(struct Button* btn);
#endif


// Button Init methods
void Button_Init(Button * new_button, const unsigned char normal_img[], const unsigned char hover_img[], int x, int y);
void Button_TTF_Init(Button * new_button, const unsigned char normal_img[], const unsigned char hover_img[], int x, int y, char *button_label);
void Button_Flag_Init(Button * new_button, const unsigned char normal_img[], int x, int y, char *button_label);
void Duplicate_Button(Button * destination_button, Button btn, int x, int y);
void Duplicate_Button_TTF(Button * destination_button, Button btn, int x, int y, char *button_label);
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
void Button_Theme_Paint_Offset(struct Button* btn, int theme_id, int x_offset, int y_offset); // Flips the state of the button based on theme
void Button_Theme_Paint_Fade(struct Button* btn, int theme_id, int fade); // Flips the state of the button based on theme

void FreeButtonResources(struct Button *btn);

#endif //_BUTTON_H_


