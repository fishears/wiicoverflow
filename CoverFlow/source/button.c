/*
 *  button.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 *  This file contains the methods for buttons in the application:
 */
#include "button.h"
#include "soundmanager.h"

Button Button_Flag_Init(const unsigned char normal_img[], int x, int y, char *button_label)
{
	Button new_button;
	new_button.texture		= GRRLIB_LoadTexture(normal_img);
	new_button.x = x;
	new_button.y = y;
	new_button.hovering = false;
	new_button.selected = false;
	new_button.show_reflection = false;
	strcpy(new_button.ttf_label, button_label);
	return new_button;
}

Button Button_TTF_Init(const unsigned char normal_img[], const unsigned char hover_img[], int x, int y, char *button_label)
{
	Button new_button;
	new_button.texture		= GRRLIB_LoadTexture(normal_img);
	new_button.hoverTexture = GRRLIB_LoadTexture(hover_img);
	new_button.x = x;
	new_button.y = y;
	new_button.hovering = false;
	new_button.selected = false;
	new_button.show_reflection = false;
	strcpy(new_button.ttf_label, button_label);
	return new_button;
}

Button Button_Init(const unsigned char normal_img[], const unsigned char hover_img[], int x, int y)
{
	Button new_button;
	new_button.texture = GRRLIB_LoadTexture(normal_img);
	new_button.hoverTexture = GRRLIB_LoadTexture(hover_img);
//	new_button.texture = BufferStaticImage(normal_img);
//	new_button.hoverTexture = BufferStaticImage(hover_img);
	new_button.x = x;
	new_button.y = y;
	new_button.hovering = false;
	new_button.show_reflection = false;
	return new_button;
}

bool Button_Hover(struct Button* btn, int x, int y)
{
	x += 40; // adjusts to the tip of the cursor fingertip
	y += 37;
	
	if(btn == 0)
		return false;
		
	if(x > (btn->x) && x < (btn->texture.w + (btn->x)) && y > (btn->y) && y < (btn->texture.h + (btn->y)))
		btn->hovering = true;
	else
		btn->hovering = false;
	
	return btn->hovering;
}

bool Button_Select(struct Button* btn, int x, int y)
{
	x += 40; // adjusts to the tip of the cursor fingertip
	y += 37;
	
	if(btn == 0)
		return false;
		
	if(x > (btn->x) && x < (btn->texture.w + (btn->x)) && y > (btn->y) && y < (btn->texture.h + (btn->y)))
		btn->hovering = true;
	else
		btn->hovering = false;
	
	if(btn->hovering)
		SOUND_PlaySound(FX_BUTTON_CLICK, 0);
	
	return btn->hovering;
}

void Button_Flag_Paint(struct Button* btn)
{
	if(btn == 0)
		return;
	
	if(btn->hovering)
	{
		GRRLIB_DrawImg(btn->x, btn->y, btn->texture, 0, 1.2, 1.2, 0xFFFFFFFF); // Adjust the scale here to make the flag 'bubble up' more
		// Draw the text centered below the flag
		CFreeTypeGX_DrawText(ttf16pt, (btn->x + (btn->texture.w / 2)), (btn->y -10), btn->ttf_label, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
	}
	else
		GRRLIB_DrawImg(btn->x, btn->y, btn->texture, 0, 1, 1, 0xFFFFFFFF);
	
}

void Button_TTF_Paint(struct Button* btn)
{
	if(btn == 0)
		return;
	
	if(btn->hovering || btn->selected)
	{
		GRRLIB_DrawImg(btn->x, btn->y, btn->hoverTexture, 0, 1, 1, 0xFFFFFFFF);
		if (strlen(btn->ttf_label) > 0)
		{
			CFreeTypeGX_DrawText(ttf16pt, (btn->x + (btn->texture.w / 2)), (btn->y + (btn->texture.h / 2)), btn->ttf_label, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		}
	}
	else
	{
		GRRLIB_DrawImg(btn->x, btn->y, btn->texture, 0, 1, 1, 0xFFFFFFFF);
		if (strlen(btn->ttf_label) > 0)
		{
			CFreeTypeGX_DrawText(ttf16pt, (btn->x + (btn->texture.w / 2)), (btn->y + (btn->texture.h / 2)), btn->ttf_label, (GXColor){0xee, 0xee, 0xee, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		}
	}

}

void Button_Menu_Paint(struct Button* btn)
{
	if(btn == 0)
		return;
	
	if(btn->hovering || btn->selected)
		GRRLIB_DrawImg(btn->x, btn->y, btn->hoverTexture, 0, 1, 1, 0xFFFFFFFF);
	else
		GRRLIB_DrawImg(btn->x, btn->y, btn->texture, 0, 1, 1, 0xFFFFFFFF);
	// Draw the text centered to the middle of the button FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE
		CFreeTypeGX_DrawText(ttf18pt, (btn->x + (btn->texture.w / 2)), (btn->y + (btn->texture.h / 2)), btn->ttf_label, (GXColor){0xee, 0xee, 0xee, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
}

void Button_Paint(struct Button* btn)
{
	if(btn == 0)
		return;
		
	if(btn->hovering)
	{
		GRRLIB_DrawImg(btn->x, btn->y, btn->hoverTexture, 0, 1, 1, 0xFFFFFFFF);
		if(btn->show_reflection)
		{
			GRRLIB_DrawImgReflection(btn->x, (btn->y + btn->texture.h), btn->hoverTexture, 0, 1, 1, 0.7);
		}
	}
	else
	{
		GRRLIB_DrawImg(btn->x, btn->y, btn->texture, 0, 1, 1, 0xFFFFFFFF);
		if(btn->show_reflection)
		{
			GRRLIB_DrawImgReflection(btn->x, (btn->y + btn->texture.h), btn->texture, 0, 1, 1, 0.7);
		}
	}	
}

void Button_Theme_Paint(struct Button* btn, int theme_id)
{
	if(btn == 0)
		return;
		
	if(btn->hovering)
	{
		switch (theme_id)
		{
			case 0: // black theme
				GRRLIB_DrawImg(btn->x, btn->y, btn->hoverTexture, 0, 1, 1, 0xFFFFFFFF);
				break;
			case 1: // white theme
				GRRLIB_DrawImg(btn->x, btn->y, btn->texture, 0, 1, 1, 0xFFFFFFFF);
				break;
			default:
				GRRLIB_DrawImg(btn->x, btn->y, btn->hoverTexture, 0, 1, 1, 0xFFFFFFFF);
				break;
		}
	}
	else
	{
		switch (theme_id)
		{
			case 0: // black theme
				GRRLIB_DrawImg(btn->x, btn->y, btn->texture, 0, 1, 1, 0xFFFFFFFF);
				break;
			case 1: // white theme
				GRRLIB_DrawImg(btn->x, btn->y, btn->hoverTexture, 0, 1, 1, 0xFFFFFFFF);
				break;
			default:
				GRRLIB_DrawImg(btn->x, btn->y, btn->texture, 0, 1, 1, 0xFFFFFFFF);
				break;
		}
		
	}	
}

// When passed two buttons and a toggle int, 0=draw the first , 1=draws the second
void Button_Toggle_Paint(struct Button* btn1, struct Button* btn2, int toggle_state)
{
	if (toggle_state >=1)
	{
		if(btn2 == 0)
			return;
		GRRLIB_DrawImg(btn2->x, btn2->y, btn2->texture, 0, 1, 1, 0xFFFFFFFF);
	}
	else
	{
		if(btn1 == 0)
			return;
		GRRLIB_DrawImg(btn1->x, btn1->y, btn1->texture, 0, 1, 1, 0xFFFFFFFF);
	}
}


// When passed two buttons and a toggle int, 0=draw the first , 1=draws the second
void Button_TTF_Toggle_Paint(struct Button* btn1, struct Button* btn2, char *left_text, char *right_text, int toggle_state)
{
	if (toggle_state >=1)
	{
		if(btn2 == 0)
			return;
		
		GRRLIB_DrawImg(btn2->x, btn2->y, btn2->texture, 0, 1, 1, 0xFFFFFFFF);
		if( (strlen(left_text) > 0) && (strlen(right_text) > 0) )
		{
			// Draw the Left side text in white
			CFreeTypeGX_DrawText(ttf16pt, (btn2->x + ((btn2->texture.w/2)/2)), (btn2->y + (btn2->texture.h/2) + 1), left_text, (GXColor){0xee, 0xee, 0xee, 0xee}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
			// Draw the Right side text in black
			CFreeTypeGX_DrawText(ttf16pt, (btn2->x + ((btn2->texture.w/4)*3)), (btn2->y + (btn2->texture.h/2)), right_text, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		}
	}
	else
	{
		if(btn1 == 0)
			return;
		
		GRRLIB_DrawImg(btn1->x, btn1->y, btn1->texture, 0, 1, 1, 0xFFFFFFFF);
		if( (strlen(left_text) > 0) && (strlen(right_text) > 0) )
		{
			// Draw the Left side text in black
			CFreeTypeGX_DrawText(ttf16pt, (btn1->x + ((btn1->texture.w/2)/2)), (btn1->y + (btn1->texture.h/2)), left_text, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
			// Draw the Right side text in white
			CFreeTypeGX_DrawText(ttf16pt, (btn1->x + ((btn1->texture.w/4)*3)), (btn1->y + (btn1->texture.h/2) + 1), right_text, (GXColor){0xee, 0xee, 0xee, 0xff}, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		}
	}
	
	
}

