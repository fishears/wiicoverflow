/*
 *  scrollbox.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by:  scognito, F1SHE4RS, afour98, blackbird399, LoudBob11
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 *  This file contains the methods for ScrollBoxes in the application:
 */
#include "scrollbox.h"
#include "coverflow.h"

void ScrollBox_Init( ScrollBox * sb, f32 x, f32 y, f32 w, f32 h, int frWidth, u32 frameColor, u32 backColor)
{
	sb->x = x;
	sb->y = y;
	sb->w = w;
	sb->h = h;
	sb->frWidth = frWidth;
	sb->frColor = frameColor;
	sb->bkColor = backColor;
	sb->lines = 0;
	sb->pos_text = (float) (sb->y + sb->h - 14);
	sb->speed = 0.5;
	sb->repeat = true;
}


void ScrollBox_SetText( ScrollBox * sb, char* sb_text[] , int lines, int speed, int repeat)
{
	int i;
	
	if (speed < 1)  sb->speed = 0.1;
	if (speed > 10) sb->speed = 1.0;
	sb->speed = (float)speed/10.0;
	sb->repeat = repeat;
	sb->lines = lines;
	
	for (i = 0; i< sb->lines; i++)
		{
		strcopy(sb->ttf_text[i], sb_text[i], 40);
	
	    }
}



void ScrollBox_Paint(struct ScrollBox* sb)
{
 int i;
 GRRLIB_Rectangle(sb->x, sb->y, sb->w, sb->h, sb->frColor, true);
 GRRLIB_Rectangle(sb->x +sb->frWidth, sb->y +sb->frWidth, sb->w -2*sb->frWidth, sb->h -2*sb->frWidth, sb->bkColor, true);

 sb->pos_text = sb->pos_text - sb->speed;

 for (i=0; i< sb->lines; i++)
	{
		
	 if(((f32)(sb->pos_text + i*16) > (sb->y +10)) && ((f32)(sb->pos_text + i*16) <  (sb->y + sb->h )))
		{
		 CFreeTypeGX_DrawText(ttf16pt, (sb->x + 10), (f32)sb->pos_text + i*16, sb->ttf_text[i], (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
		}
	 else
		{
	     if ((sb->repeat) && ((f32)(sb->pos_text + (sb->lines -1) *16) < (sb->y +10))) 
			sb->pos_text = (float) (sb->y + sb->h);
		}
	}
}	


