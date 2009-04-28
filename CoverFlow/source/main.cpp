/*---------------------------------------------------------------------------------

	CoverFlow by Beardface

---------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
 
#include <iostream> 
 
#include "GRRLIB.h"
#include "pngu/pngu.h"
 
#include <vector>
 
#define DEFAULT_FIFO_SIZE	(256*1024)

float shift = 0.0;
	

bool selected = false;

int DRAW_WINDOW = 5;
int COVER_COUNT = 100;

float animate_flip = 0.0;
float FLIP_SPEED   = 0.008;

float SCROLL_SPEED = 0.025;

extern const u8		RC8P7D_png[];
extern const u32	RC8P7D_png_size;

extern const u8		R2AE7D_png[];
extern const u32	R2AE7D_png_size;

extern const u8		R2DEAP_png[];
extern const u32	R2DEAP_png_size;

extern const u8		R2DEEB_png[];
extern const u32	R2DEEB_png_size;

extern const u8		R2FE5G_png[];
extern const u32	R2FE5G_png_size;

extern const u8		R2HE41_png[];
extern const u32	R2HE41_png_size;
#define USBLOADER_PATH		"sdhc:/usb-loader"

Mtx GXmodelView2D;

std::vector<GRRLIB_texImg> covers;
GRRLIB_texImg cover_texture;

float change_scale_without_containing(float val, float in_min, float in_max, 
                                      float out_min, float out_max)
{
  float percent = 0;
  if (in_min == in_max) {
    return 0;
  }
  percent = (val - in_min) / (in_max - in_min);
  return (out_min + percent * (out_max - out_min));
}

float change_scale(float val, float in_min, float in_max, 
                   float out_min, float out_max)
{
  if(val > in_max)
  {
	val = in_max;
  }
  
  if(val < in_min)
  {
     val = in_min;
  }
  
  return change_scale_without_containing(val, in_min, in_max, out_min, out_max);
}

void Init_Covers()
{
	int CoverCount = COVER_COUNT;
	
	for(int i = 0; i < CoverCount; i++)
	{
		switch(rand()%6)
		{
			case 0:
				covers.push_back( GRRLIB_LoadTexture(R2AE7D_png) );
				break;
			case 1:
				covers.push_back( GRRLIB_LoadTexture(R2DEAP_png) );
				break;
			case 2:
				covers.push_back( GRRLIB_LoadTexture(R2DEEB_png) );
				break;
			case 3:
				covers.push_back( GRRLIB_LoadTexture(R2FE5G_png) );
				break;
			case 4:
				covers.push_back( GRRLIB_LoadTexture(R2HE41_png) );
				break;
			default:
				covers.push_back( GRRLIB_LoadTexture(RC8P7D_png) );
		}
	}
}

void GRRLIB_Cover(float pos, int texture_id)
{

	  if((selected || animate_flip > 0) && pos == 0)
		return;

	  static const float SPACING = 3.5;
	  float dir = 1;
	  float loc, scale, angle;

	  if (pos < 0) {
		dir *= -1;
		pos *= -1;
	  }
	  
	  loc = SPACING * dir * (pow(pos + 1, -1) - 1);
	  scale = pow(pos + 1, -2);
	  angle = -1 * dir * change_scale(scale, 0, 1, 45, 0);
	
	  if(texture_id != -1)
	  {
			GRRLIB_DrawCoverImg(loc*1.2,covers[texture_id],angle,1.0,0xFFFFFFFF);
	  }
	  else
	  {
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
	  }
}

void draw_selected()
{
	
	if(selected && animate_flip < 1.0)
	{
		animate_flip += FLIP_SPEED;
		if(animate_flip > 1.0)
			animate_flip = 1.0;
			
	}
	else if(!selected)
	{
		animate_flip -= FLIP_SPEED;
		if(animate_flip < 0)
		{
			animate_flip = 0;
		}
	}
	
	
	  static const float SPACING = 2.8;
	  float dir = 1;
	  float loc, scale, angle;
	  
	  loc = SPACING * dir * (pow(1, -1) - 1);
	  scale = change_scale(animate_flip, 0, 1, 0, 360);
	  angle = -1 * dir * scale;
	
	  if(scale >= 180)
	  {
		//Use back art texture
		GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,2.0,0xFFFFFFFF);
	  }
	  else
	  {
		GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
	  }
}

void draw_covers()
{
	int i;
	
	for(i = (-1*(COVER_COUNT/2.0)); i < (COVER_COUNT/2.0); i++)
	{
		
		/*Some logic to avoid drawing everything*/
		if(abs(shift+i) < DRAW_WINDOW)
		{
			printf("Shift: %f, %d\n", shift, i);
			GRRLIB_Cover(i+shift, i+(COVER_COUNT/2.0));
		}
	}
}

//---------------------------------------------------------------------------------
int main( int argc, char **argv ){
//---------------------------------------------------------------------------------

	bool flip = true;

	GRRLIB_Init();
	WPAD_Init();
	PAD_Init();

	Init_Covers();
	cover_texture = GRRLIB_LoadTexture(RC8P7D_png);

	selected = false;
    int wait_time = 0;
	
	while(1) {

		WPAD_ScanPads();
		PAD_ScanPads();


		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);
		
		if (PAD_ButtonsDown(0) & PAD_BUTTON_B ||
			WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			if(selected && animate_flip >= 1.0)
			{
				selected = false;
			}
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A ||
			PAD_ButtonsDown(0) & PAD_BUTTON_A)
		{
			if(!selected && animate_flip <= 0.0)
			{
				selected = true;
			}
			
		}

		if(!selected && animate_flip == 0)
		{
			if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT ||
				PAD_ButtonsHeld(0) & PAD_BUTTON_LEFT)
			
			{	
				
				shift -= SCROLL_SPEED;
			}
			else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT ||
				PAD_ButtonsHeld(0) & PAD_BUTTON_RIGHT)
			{
				shift += SCROLL_SPEED;
			}
			else
			{
				if(abs(((int)shift * 10000.0) - (shift*10000.0))/10000.0 > (SCROLL_SPEED+SCROLL_SPEED/2.0))
				{
					if((int)((int)(shift+0.5) - (int)shift) == 0)
					{
						shift -= SCROLL_SPEED;
					}
					else
					{
						shift += SCROLL_SPEED;
					}
				}
				else
				{
					shift = (int)shift;
				}
					
			}
		}
		draw_covers();

		if(selected || animate_flip != 0)
		{
			draw_selected();
		}
		
        GRRLIB_Render();
		
		/*Demo stuff*/
	/*	if(animate_flip >= 1.0)
		{
			if(wait_time > 100)
				selected = false;
			else
				wait_time++;
		}
		
		if(shift > COVER_COUNT/2.0)
		{
			flip = false;
		}
		else if(shift < (-1)*COVER_COUNT/2.0)
		{
			flip = true;
		}
		
		
		if(selected || animate_flip != 0)
		{
			draw_selected();
		}
		else
		{
		  if(flip)
			shift += SCROLL_SPEED;
		  else
		 	shift -= SCROLL_SPEED;
		}
		*/

	}
	
	//Preview
	
    GRRLIB_Exit(); 
	
	return 0;
}
 
