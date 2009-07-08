/*
 *  scrollbox.h
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 */
#ifndef _SCROLLBOX_H_
#define _SCROLLBOX_H_

#include "GRRLIB.h"

typedef struct ScrollBox
{
	f32 	x;
	f32 	y;
	f32 	w;
	f32 	h;
	int 	frWidth;  
	u32 	frColor;
	u32 	bkColor;
	char	ttf_text[40][40];
	int 	lines;
	float	pos_text;
	float   speed;
	bool    repeat;
} ScrollBox;


void ScrollBox_Init( ScrollBox * sb, f32 x, f32 y, f32 w, f32 h, int frWidth, u32 frameColor, u32 backColor);
void ScrollBox_SetText( ScrollBox * sb, char* sb_text[], int lines, int speed, int repeat );
void ScrollBox_Paint( struct ScrollBox* sb);


#endif //_SCROLLBOX_H_

