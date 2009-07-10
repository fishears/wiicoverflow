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
	int i;
	
	sb->x = x;
	sb->y = y;
	sb->w = w;
	sb->h = h;
	sb->frWidth = frWidth;
	sb->frColor = frameColor;
	sb->bkColor = backColor;
	sb->txColor = (GXColor){0x00, 0x00, 0x00, 0xff};
	sb->lines = 0;
	sb->pos_text = (float) (sb->y + sb->h - 14);
	sb->textStyling = FTGX_JUSTIFY_LEFT;
	sb->speed = 0.4;
	sb->repeat = true;
	sb->lineSpace = 0;
	sb->startDelay = 0;
	sb->waitTics = 0;
	sb->selected = false;
	for(i=0;i<SB_MAXLINES;i++) {
		strcpy(sb->ttf_text[i], "");
		sb->ttf_size[i] = 16;
	}
}


void ScrollBox_SetTextFormat( ScrollBox * sb, int* ttf_size, int lines)
{
 	int i;

	if (lines > SB_MAXLINES) lines = SB_MAXLINES;
	if (sb->lines == 0) sb->lines = lines;
	
	for (i = 0; i< sb->lines; i++)
		{
		switch (ttf_size[i])
			{
			  case 14:
				sb->ttf_size[i] = 14;
				break;
			  case 16:
				sb->ttf_size[i] = 16;
				break;
			  case 18:
				sb->ttf_size[i] = 18;
				break;
			  case 20:
				sb->ttf_size[i] = 20;
				break;
			  case 24:
				sb->ttf_size[i] = 24;
				break;
			  default:
				sb->ttf_size[i] = 16;
			}
	    }
}


void ScrollBox_SetText( ScrollBox * sb, char* sb_text[], int lines)
{
	int i;
    
    if (lines > SB_MAXLINES) lines = SB_MAXLINES;
	sb->lines = lines;
	
	for (i = 0; i< sb->lines; i++)
		{
		strcopy(sb->ttf_text[i], sb_text[i], 40);
	    }
}


void ScrollBox_Paint(struct ScrollBox* sb)
{
 int i, j, ysum;
 f32 xp;
 
 GRRLIB_Rectangle(sb->x, sb->y, sb->w, sb->h, sb->frColor, true);
 GRRLIB_Rectangle(sb->x +sb->frWidth, sb->y +sb->frWidth, sb->w -2*sb->frWidth, sb->h -2*sb->frWidth, sb->bkColor, true);
 
 if (!sb->selected)
	 {
	 if ((sb->startDelay > 0) && ( sb->waitTics < ( sb->startDelay * 50 )))
		sb->waitTics++;
	 else	
		sb->pos_text = sb->pos_text - sb->speed;
	 }
	 
 switch(sb->textStyling){
	case FTGX_JUSTIFY_LEFT:
		xp = sb->x + 10;
		break;
	case FTGX_JUSTIFY_CENTER:
		xp = sb->x + (sb->w /2);
		break;
	case FTGX_JUSTIFY_RIGHT:
		xp = sb->x + sb->w -10;
		break;
	default:
		xp = sb->x + 10;
	}
 
 for (i=0; i< sb->lines; i++)
	{
	 ysum = 0;
	 for(j =0; j<i; j++)
	  {
		ysum = ysum + sb->ttf_size[j] + sb->lineSpace;
	  }
	 
	 if(((f32)(sb->pos_text + ysum) > (sb->y +sb->ttf_size[i]-sb->frWidth)) && ((f32)(sb->pos_text + ysum) <  (sb->y + sb->h )))
		{
		 switch (sb->ttf_size[i]) {
			case 16:
				CFreeTypeGX_DrawText(ttf16pt, xp, (f32)sb->pos_text + ysum, sb->ttf_text[i], sb->txColor, sb->textStyling);
				break;
			case 14:
				CFreeTypeGX_DrawText(ttf14pt, xp, (f32)sb->pos_text + ysum, sb->ttf_text[i], sb->txColor, sb->textStyling);
				break;
			case 18:
				CFreeTypeGX_DrawText(ttf18pt, xp, (f32)sb->pos_text + ysum, sb->ttf_text[i], sb->txColor, sb->textStyling);
				break;
			case 20:
				CFreeTypeGX_DrawText(ttf20pt, xp, (f32)sb->pos_text + ysum, sb->ttf_text[i], sb->txColor, sb->textStyling);
				break;
			case 24:
				CFreeTypeGX_DrawText(ttf24pt, xp, (f32)sb->pos_text + ysum, sb->ttf_text[i], sb->txColor, sb->textStyling);
				break;
			}
		}
	 else
		{
	     ysum = 0;
		 for(j =0; j<sb->lines-1; j++)
		  {
		   ysum = ysum + sb->ttf_size[j] + sb->lineSpace;
		  }
		 
		 if ((sb->repeat) && ((f32)(sb->pos_text + ysum) < (sb->y +14))) 
			sb->pos_text = (float) (sb->y + sb->h);
		}
	}
}	


void ScrollBox_SetSpeed( ScrollBox * sb, int speed)
{
	if (speed < 1)  sb->speed = 0.1;
	if (speed > 10) sb->speed = 1.0;
	sb->speed = (float)speed/10.0;
}

void ScrollBox_SetRepeat( ScrollBox * sb, bool repeat)
{
	sb->repeat = repeat;
}


void ScrollBox_SetTextStyling( ScrollBox * sb, uint16_t	textStyling)
{
	sb->textStyling = textStyling;
}

void ScrollBox_SetLineSpace( ScrollBox * sb, int lineSpace)
{
	sb->lineSpace = lineSpace;
}

void ScrollBox_SetStartPos( ScrollBox * sb, int startPos )
{
	switch (startPos)
	{
		case SB_BOTTOM:
			sb->pos_text = (float) (sb->y + sb->h - 14);
			break;
		case SB_MIDDLE:
			sb->pos_text = (float) (sb->y + sb->h/2 );
			break;
		case SB_TOP:
			sb->pos_text = (float) (sb->y + 24);
		    break;
		default:
			sb->pos_text = (float) (sb->y + sb->h - 14);
	}
}

void ScrollBox_SetDelay( ScrollBox * sb, int startDelay )
{
	sb->startDelay = startDelay;
}

void ScrollBox_SetTextColor( ScrollBox * sb, GXColor TXtColor )
{
	sb->txColor = TXtColor;
}




bool ScrollBox_Select(struct ScrollBox* sb, int x, int y)
{
	x += 40; // adjusts to the tip of the cursor fingertip
	y += 37;
	
	if(sb == 0)
		return false;
		
	if(x > (sb->x) && x < ((sb->w) + (sb->x)) && y > (sb->y) && y < ((sb->h) + (sb->y)))
		return true;
	else
		return false;

}
