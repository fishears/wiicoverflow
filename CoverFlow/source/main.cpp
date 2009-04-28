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
 
#include "wbfs.h"

#include "disc.h"

#include <iostream> 
 
#include "GRRLIB.h"
#include "pngu/pngu.h"
#include "fat.h"
#include "sys.h"
#include "subsystem.h" 
 
#define COVER_WIDTH		160
#define COVER_HEIGHT		225

#include <vector>
 
#define DEFAULT_FIFO_SIZE	(256*1024)

#define TEST_MODE 1

/* Constants */
#define ENTRIES_PER_PAGE	12
#define MAX_CHARACTERS		30

/* Gamelist buffer */
static struct discHdr *gameList = NULL;

/* Gamelist variables */
static s32 gameCnt = 0, gameSelected = 0, gameStart = 0;

/* WBFS device */
static s32 wbfsDev = WBFS_MIN_DEVICE;


float shift = 0.0;
	
bool selected = false;

int DRAW_WINDOW = 5;

#ifdef TEST_MODE
int COVER_COUNT = 29e;
#else
int COVER_COUNT = 0;
#endif

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

extern const u8		no_cover_png[];
extern const u32	no_cover_png_size;
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
	cover_texture = GRRLIB_LoadTexture(no_cover_png);
	#ifndef TEST_MODE
	for(int i = 0; i < gameCnt; i++)
	{
		void *imgData = (void *)no_cover_png;

		char filepath[128];
		s32  ret;

		struct discHdr *header = &gameList[i];
		
		sprintf(filepath, USBLOADER_PATH "/covers/%s.png", header->id);

		ret = Fat_ReadFile(filepath, &imgData);
		
		GRRLIB_texImg tmpTex = GRRLIB_LoadTexture((const unsigned char*)imgData);
		
		if (ret > 0) {

			if ((tmpTex.w > COVER_WIDTH) || (tmpTex.h > COVER_HEIGHT))
			{
				covers.push_back(cover_texture);
			}
			else
			{
				covers.push_back(tmpTex);
			}
		}
		else
		{
			covers.push_back(tmpTex);
		}
	}
	
	#else
	
	int CoverCount = COVER_COUNT;
	
	for(int i = 0; i < CoverCount; i++)
	{
		switch(rand()%6)
		{
			case 0:
				covers.push_back( GRRLIB_LoadTexture(no_cover_png) );
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
	#endif
}

void GRRLIB_Cover(float pos, int texture_id)
{

	  if((selected || animate_flip > 0) && pos == 0)
	  {
		gameSelected = texture_id;
		return;
	  }

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
		GRRLIB_DrawCoverImg(loc*1.2,covers[gameSelected],angle,1.0,0xFFFFFFFF);
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

s32 __Menu_EntryCmp(const void *a, const void *b)
{
	struct discHdr *hdr1 = (struct discHdr *)a;
	struct discHdr *hdr2 = (struct discHdr *)b;

	/* Compare strings */
	return strcmp(hdr1->title, hdr2->title);
}

s32 GetEntries(void)
{
	struct discHdr *buffer = NULL;

	u32 cnt, len;
	s32 ret;

	/* Get list length */
	ret = WBFS_GetCount(&cnt);
	if (ret < 0)
		return ret;

	/* Buffer length */
	len = sizeof(struct discHdr) * cnt;

	/* Allocate memory */
	buffer = (struct discHdr *)memalign(32, len);
	if (!buffer)
		return -1;

	/* Clear buffer */
	memset(buffer, 0, len);

	/* Get header list */
	ret = WBFS_GetHeaders(buffer, cnt, sizeof(struct discHdr));
	if (ret < 0)
		goto err;

	/* Sort entries */
	qsort(buffer, cnt, sizeof(struct discHdr), __Menu_EntryCmp);

	/* Free memory */
	if (gameList)
		free(gameList);

	/* Set values */
	gameList = buffer;
	gameCnt  = cnt;
	COVER_COUNT = gameCnt;
	
	Init_Covers();

	/* Reset variables */
	gameSelected = gameStart = 0;

	return 0;

err:
	/* Free memory */
	if (buffer)
		free(buffer);

	return ret;
}

bool init_usbfs()
{
	s32 ret;

	/* Load Custom IOS */
	ret = IOS_ReloadIOS(249);
	
	/* Initialize system */
	Sys_Init();

	/* Initialize subsystems */
	Subsystem_Init();
	
	
	/* Check if Custom IOS is loaded */
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Custom IOS could not be loaded! (ret = %d)\n", ret);

		return false;
	}

	/* Initialize DIP module */
	ret = Disc_Init();
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Could not initialize DIP module! (ret = %d)\n", ret);

		return false;
	}

	return true;
}

bool Init_Game_List(void)
{
	u32 timeout = 30;
	s32 ret;

	//char *devname = "USB Mass Storage Device";

	wbfsDev = WBFS_DEVICE_USB;

	/* Initialize WBFS */
	ret = WBFS_Init(wbfsDev, timeout);

	/* Try to open device */
	if (WBFS_Open() >= 0) {
		/* Get game list */
		GetEntries();
		return true;
	}
	else
	{
		return false;
	}
}


void Menu_Boot(void)
{
	struct discHdr *header = NULL;

	s32 ret;

	/* No game list */
	if (!gameCnt)
		return;

	/* Selected game */
	header = &gameList[gameSelected];

	printf("\n");
	printf("[+] Booting Wii game, please wait...\n");

	/* Set WBFS mode */
	Disc_SetWBFS(wbfsDev, header->id);

	/* Open disc */
	ret = Disc_Open();
	if (ret < 0) {
		printf("    ERROR: Could not open game! (ret = %d)\n", ret);
		goto out;
	}

	/* Boot Wii disc */
	Disc_WiiBoot();

	printf("    Returned! (ret = %d)\n", ret);

out:
	printf("\n");
	printf("    Press any button to continue...\n");
}

//---------------------------------------------------------------------------------
int main( int argc, char **argv ){
//---------------------------------------------------------------------------------

	#ifndef TEST_MODE
	if(!init_usbfs())
		return 0;
	
	//bool flip = true;

	Init_Game_List();
	#else
	Init_Covers();
	#endif
	GRRLIB_Init();
	WPAD_Init();
	PAD_Init();

	//cover_texture = GRRLIB_LoadTexture(RC8P7D_png);

	selected = false;
    //int wait_time = 0;
	
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
			if(!gameCnt)
			{
				if(!selected && animate_flip <= 0.0)
				{
					selected = true;
				}
				else if(selected && animate_flip == 1.0)
				{
					//TODO Prompt to boot game...
					Menu_Boot();
				}
			}
		}

		if(!selected && animate_flip == 0)
		{
			if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT ||
				PAD_ButtonsHeld(0) & PAD_BUTTON_LEFT)
			
			{	
				if(!((int)shift-1 <= (-1)*(COVER_COUNT/2.0)))
					shift -= SCROLL_SPEED;
			}
			else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT ||
				PAD_ButtonsHeld(0) & PAD_BUTTON_RIGHT)
			{
				if(!((int)shift+.5 >= (COVER_COUNT/2.0)))
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
 
