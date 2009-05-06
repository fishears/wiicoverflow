
#include "button.h"

Button Button_Init(const unsigned char normal_img[], const unsigned char hover_img[], int x, int y)
{
	Button new_button;
	new_button.texture = GRRLIB_LoadTexture(normal_img);
	new_button.hoverTexture = GRRLIB_LoadTexture(hover_img);
	new_button.x = x;
	new_button.y = y;
	new_button.hovering = false;
	
	return new_button;
}

bool Button_Hover(struct Button* btn, int x, int y)
{
	if(x > (btn->x) && x < (btn->texture.w + (btn->x)) &&
		y > (btn->y) && y < (btn->texture.h + (btn->y)))
	{
		btn->hovering = true;
	}
	else
	{
		btn->hovering = false;
	}
	
	return btn->hovering;
}

bool Button_Select(struct Button* btn, int x, int y)
{
	if(x > (btn->x) && x < (btn->texture.w + (btn->x)) &&
		y > (btn->y) && y < (btn->texture.h + (btn->y)))
	{
		btn->hovering = true;
	}
	else
	{
		btn->hovering = false;
	}
	
	return btn->hovering;
}

void Button_Paint(struct Button* btn)
{
	if(btn->hovering)
	{
		GRRLIB_DrawImg(btn->x, btn->y, btn->hoverTexture, 0, 1, 1, 0xFFFFFFFF);
	}
	else
	{
		GRRLIB_DrawImg(btn->x, btn->y, btn->texture, 0, 1, 1, 0xFFFFFFFF);
	}	
}

void Button_Theme_Paint(struct Button* btn, int theme_id)
{
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
