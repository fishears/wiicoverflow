/*---------------------------------------------------------------------------------

	nehe lesson 4 port to GX by WinterMute

---------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
 
#include "GRRLIB.h"
#include "pngu/pngu.h"
 
#include <vector>
 
#define DEFAULT_FIFO_SIZE	(256*1024)

float shift = -1.0;
	
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
	int CoverCount = 40;
	
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
			GRRLIB_DrawCoverImg(loc*1.2,covers[texture_id],angle,scale,0xFFFFFFFF);
	  }
	  else
	  {
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,scale,0xFFFFFFFF);
	  }
}

void draw_covers()
{
	int i = -20;
	
	for(i = -20; i < 20; i++)
	{
		GRRLIB_Cover(i+shift, i+20);
	}
}

//---------------------------------------------------------------------------------
int main( int argc, char **argv ){
//---------------------------------------------------------------------------------

	GRRLIB_Init();
	WPAD_Init();


	Init_Covers();
	cover_texture = GRRLIB_LoadTexture(RC8P7D_png);

	while(1) {

		WPAD_ScanPads();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);

		if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT)
		{
			shift -= .05;
		}
		else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT)
	    {
			shift += .05;
		}
		else
		{
			  /*if (shift >= 1) {
				shift -= (int)shift;
			  } else if (shift <= -1) {
				shift -= (int)shift;
			  }
			  */
		}

		draw_covers();

        GRRLIB_Render();
		shift += 0.025;

	}
	
	//Preview
	
    GRRLIB_Exit(); 
	
	return 0;
}
 
