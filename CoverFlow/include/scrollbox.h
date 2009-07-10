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

#define SB_MAXLINES 40
#define SB_BOTTOM	0
#define SB_MIDDLE	1
#define SB_TOP		2

	
typedef struct ScrollBox
{
	f32 		x;
	f32 		y;
	f32 		w;
	f32 		h;
	int 		frWidth;  
	u32 		frColor;
	u32 		bkColor;
	GXColor		txColor;
	char		ttf_text[SB_MAXLINES][40];
	int			ttf_size[SB_MAXLINES];		// 14, 16, 18, 20, 24 ( 0 -> Default = 16 )
	int 		lines;
	float		pos_text;
	float   	speed;
	uint16_t	textStyling;
	int			lineSpace;
	bool    	repeat;
	int 		startDelay;
	int			startPos;
	int		    waitTics;
	bool		selected;
} ScrollBox;


void ScrollBox_Init( ScrollBox * sb, f32 x, f32 y, f32 w, f32 h, int frWidth, u32 frameColor, u32 backColor );
void ScrollBox_Paint( struct ScrollBox* sb );
bool ScrollBox_Select(struct ScrollBox* sb, int x, int y);
void ScrollBox_SetText( ScrollBox * sb, char* sb_text[], int lines );
void ScrollBox_SetTextFormat( ScrollBox * sb, int* ttf_size, int lines );
//Properties
void ScrollBox_SetSpeed( ScrollBox * sb, int speed );
void ScrollBox_SetRepeat( ScrollBox * sb, bool repeat );
void ScrollBox_SetTextStyling( ScrollBox * sb, uint16_t	textStyling );
void ScrollBox_SetLineSpace( ScrollBox * sb, int lineSpace );
void ScrollBox_SetStartPos( ScrollBox * sb, int startPos );
void ScrollBox_SetDelay( ScrollBox * sb, int Delay );
void ScrollBox_SetTextColor( ScrollBox * sb, GXColor TXtColor );
#endif //_SCROLLBOX_H_

