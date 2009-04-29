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
#include "wpad.h"

#include "wbfs.h"

#include "disc.h"

//#include <iostream> 
 
#include "GRRLIB.h"
#include "pngu/pngu.h"
#include "fat.h"
#include "sys.h"
 
#define COVER_WIDTH		160
#define COVER_HEIGHT		225

//#include <vector>
 
#define DEFAULT_FIFO_SIZE	(256*1024)

//#define TEST_MODE 1

/* Constants */
#define ENTRIES_PER_PAGE	12
#define MAX_CHARACTERS		30

/* Gamelist buffer */
static struct discHdr *gameList = NULL;

/* Gamelist variables */
static s32 gameCnt = 0, gameSelected = 0, gameStart = 0;

/* WBFS device */
static s32 my_wbfsDev = WBFS_DEVICE_USB;


float shift = 0.0;
	
bool selected = false;

float progress = 0.0;

int DRAW_WINDOW = 7;

#ifdef TEST_MODE
int COVER_COUNT = 29;
#else
int COVER_COUNT = 0;
#endif

float animate_rotate = 0.0;
float animate_flip = 0.0;
float FLIP_SPEED   = 0.016;

float SCROLL_SPEED = 0.050;

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

extern const u8		back_cover_png[];
extern const u32	back_cover_png_size;

extern const u8		no_disc_png[];
extern const u32	no_disc_png_size;

extern const u8		select_menu_png[];
extern const u32	select_menu_png_size;

extern const u8     font1_png[];
extern const u32    font1_png_size;

extern const u8     loading_main_png[];
extern const u32    loading_main_png_size;

extern const u8     progress_png[];
extern const u32    progress_png_size;

extern const u8     gradient_bg_png[];
extern const u32    gradient_bg_png_size;

extern const u8     slide_png[];
extern const u32    slide_png_size;

extern const u8     slide_bar_png[];
extern const u32    slide_bar_png_size;

extern const u8     usb_error_png[];
extern const u32    usb_error_png_size;

#define USBLOADER_PATH		"sdhc:/usb-loader"

Mtx GXmodelView2D;

#define MAX_COVERS 500
int array_size = 0;
GRRLIB_texImg covers[MAX_COVERS];      //std::vector<GRRLIB_texImg> covers;

GRRLIB_texImg cover_texture;
GRRLIB_texImg back_texture;

GRRLIB_texImg empty_texture;

GRRLIB_texImg no_disc_texture;
GRRLIB_texImg current_cover_texture;

GRRLIB_texImg select_menu_texture;

GRRLIB_texImg text_font1;

GRRLIB_texImg loader_main_texture;

GRRLIB_texImg progress_texture;

GRRLIB_texImg gradient_texture;

GRRLIB_texImg slide_texture;
GRRLIB_texImg slide_bar_texture;

GRRLIB_texImg usb_error_texture;

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


void Paint_Progress(float v)
{
	int count = (int)(v*10);
	
	if(count > 40)
		count = 40;
	int i;
	
	GRRLIB_DrawImg(0, 0, gradient_texture, 0, 1, 1, 0xFFFFFFFF);
	
	for(i = 0; i < count; i++)
	{
		GRRLIB_DrawImg(165+12*i, 231, progress_texture, 0, 1, 1, 0xFFFFFFFF);
	}

	GRRLIB_DrawImg(0, 0, loader_main_texture, 0, 1, 1, 0xFFFFFFFF);
    
	GRRLIB_Render();
}

void LoadCurrentCover(int id)
{

	#ifndef TEST_MODE
	void *imgData;// = (void *)no_cover_png;

	char filepath[128];
	s32  ret;

	struct discHdr *header = &gameList[id];
	
//		substr
	
	sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c.png", header->id[0],header->id[1],header->id[2]);

	ret = Fat_ReadFile(filepath, &imgData);
	
	
	if (ret > 0) {
		current_cover_texture = GRRLIB_LoadTexture((const unsigned char*)imgData);
	}
	else
	{
		current_cover_texture = no_disc_texture;
	}
	
	#else
	current_cover_texture = no_disc_texture;
	#endif
	
}

void AddCover(GRRLIB_texImg tex)
{
	if(array_size < MAX_COVERS)
	{
		covers[array_size] = tex;
		array_size++;
	}
	
	/*
	array_size = array_size + 1;
	covers = (GRRLIB_texImg*)realloc(covers, (array_size * sizeof(GRRLIB_texImg)));

	if (covers == NULL)
	{
	   // fprintf(stdout, "ERROR: Couldn't realloc memory!");
	   return;
	}

    covers[array_size - 1] = tex;
	*/
}

void Init_Covers()
{
	int i;
	
	cover_texture = GRRLIB_LoadTexture(no_cover_png);
	back_texture = GRRLIB_LoadTexture(back_cover_png);
    no_disc_texture = GRRLIB_LoadTexture(no_disc_png);
    select_menu_texture = GRRLIB_LoadTexture(select_menu_png);
	text_font1 = GRRLIB_LoadTexture(font1_png);
	
	slide_texture = GRRLIB_LoadTexture(slide_png);
	slide_bar_texture = GRRLIB_LoadTexture(slide_bar_png);
	
    GRRLIB_InitTileSet(&text_font1, 32, 36, 32);
	
	progress+=0.05;
	Paint_Progress(progress);
	
	float max_progress = 2.1;
	
	float per_game_prog = max_progress/gameCnt;
	
	#ifndef TEST_MODE
	for(i = 0; i < gameCnt; i++)
	{
		void *imgData;// = (void *)no_cover_png;

		char filepath[128];
		s32  ret;

		struct discHdr *header = &gameList[i];
		
		sprintf(filepath, USBLOADER_PATH "/covers/%s.png", header->id);

		ret = Fat_ReadFile(filepath, &imgData);
		
		
		if (ret > 0) {

			GRRLIB_texImg tmpTex = GRRLIB_LoadTexture((const unsigned char*)imgData);
			
			if ((tmpTex.w > COVER_WIDTH) || (tmpTex.h > COVER_HEIGHT))
			{
				AddCover(cover_texture);
			}
			else
			{
				AddCover(tmpTex);
			}
		}
		else
		{
			AddCover(cover_texture);
		}
		progress+=per_game_prog;
		Paint_Progress(progress);
	}
	
	#else
	
	int CoverCount = COVER_COUNT;
	
	for(i = 0; i < CoverCount; i++)
	{
		switch(rand()%6)
		{
			case 0:
				AddCover( GRRLIB_LoadTexture(no_cover_png) );
				break;
			case 1:
				AddCover( GRRLIB_LoadTexture(R2DEAP_png) );
				break;
			case 2:
				AddCover( GRRLIB_LoadTexture(R2DEEB_png) );
				break;
			case 3:
				AddCover( GRRLIB_DuplicateTexture(GRRLIB_LoadTexture(R2FE5G_png) , 160, 225));
				break;
			case 4:
				AddCover( GRRLIB_LoadTexture(R2HE41_png) );
				break;
			default:
				AddCover( GRRLIB_LoadTexture(RC8P7D_png) );
		}
	}
	
	#endif
}

void GRRLIB_Cover(float pos, int texture_id)
{
		if(pos == 0)
			gameSelected = texture_id;

	  if((selected || animate_flip > 0) && pos == 0)
	  {
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
			
		if(animate_flip > 0.3 && animate_flip < 0.7)
		{
			animate_flip = 0.7;
		}
			
	}
	else if(!selected)
	{
		animate_flip -= FLIP_SPEED;
		
		if(animate_flip > 0.3 && animate_flip < 0.7)
		{
			animate_flip = 0.3;
		}
		
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
		GRRLIB_DrawCoverImg(loc*1.2,back_texture,angle,2.0,0xFFFFFFFF);
		
		if(scale >= 360)
		{
			int i ;
			int len;
			animate_rotate++;
			if(animate_rotate == 360) animate_rotate = 0;
			
			GRRLIB_DrawImg(220, 150, current_cover_texture, animate_rotate, 1, 1, 0xFFFFFFFF);
			//GRRLIB_DrawImg(130, 100, covers[gameSelected], 0, 1, 1,0xFFFFFFFF);
			GRRLIB_DrawImg(170, 300, select_menu_texture, 0, 1, 1, 0xFFFFFFFF);
			
			#ifndef TEST_MODE
			struct discHdr *header = NULL;
			header = &gameList[gameSelected];
			f32 size = 0.0;

			/* Get game size */
			WBFS_GameSize(header->id, &size);
			char name[64];
			
			for(i = 0; i < 64; i++)
				name[i] = toupper(header->title[i]);
				
			len = strlen(name);
			float tsize = .8;

			if(len > 20)
				tsize = .6;

			if(len > 28)
				tsize = .4;
				
			if(len > 40)
				tsize = .3;
			
			int offset = (len*10);
			
			if(offset > 240) offset = 240;
			
            GRRLIB_Printf(300 - offset, 10, text_font1, 0XFFFFFFFF, tsize, "%s", name);
            GRRLIB_Printf(210, 50, text_font1, 0XFFFFFFFF, .4, "(%c%c%c%c) (%.2fGB)", header->id[0], header->id[1], header->id[2], header->id[3], size);
			#else
            GRRLIB_Printf(90, 10, text_font1, 0XFFFFFFFF, .8, "%s", "JUSTINS GAME");
            GRRLIB_Printf(180, 50, text_font1, 0XFFFFFFFF, .5, "%s", "JUSTINS GAME");
			#endif
		}
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
			//printf("Shift: %f, %d\n", shift, i);
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

	progress+=0.05;
	Paint_Progress(progress);
	
	/* Get header list */
	ret = WBFS_GetHeaders(buffer, cnt, sizeof(struct discHdr));
	if (ret < 0)
		goto err;

	progress+=0.05;
	Paint_Progress(progress);
	
	/* Sort entries */
	qsort(buffer, cnt, sizeof(struct discHdr), __Menu_EntryCmp);

	progress+=0.05;
	Paint_Progress(progress);
	
	/* Free memory */
	if (gameList)
		free(gameList);

	/* Set values */
	gameList = buffer;
	gameCnt  = cnt;
	COVER_COUNT = gameCnt;
	
	Init_Covers();

	progress+=0.05;
	Paint_Progress(progress);
	
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
   // __Disc_SetLowMem();

	s32 ret;

	
	/* Initialize system */
	Sys_Init();

	///* Initialize subsystems */
	Wpad_Init();

	/* Mount SDHC */
	Fat_MountSDHC();
	
	progress+=0.05;
	Paint_Progress(progress);
	

	/* Initialize DIP module */
	ret = Disc_Init();
	progress+=0.05;
	Paint_Progress(progress);
	
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Could not initialize DIP module! (ret = %d)\n", ret);

		return false;
	}

	return true;
}

bool Init_Game_List(void)
{

	Paint_Progress(progress);
	
	/* Try to open device */
	if (WBFS_Open() >= 0) {
		/* Get game list */
		
		progress+=0.05;
		Paint_Progress(progress);
		GetEntries();
		return true;
	}
	else
	{
		return false;
	}
}

void DrawSlider(void)
{
	int min_loc = 0;
	int max_loc = 313;
	
	GRRLIB_DrawImg(120, 400, slide_bar_texture, 0, 1, 1, 0xFFFFFFFF);
	
	int x = change_scale(shift, -1*(COVER_COUNT/2.0), COVER_COUNT/2.0, min_loc, max_loc);
	
	
	GRRLIB_DrawImg(126+x, 416, slide_texture, 0, 1, 1, 0xFFFFFFFF);
	
	
}

bool Menu_Boot(void)
{

	struct discHdr *header = NULL;

	s32 ret;

	/* No game list */
	if (!gameCnt)
		return false;

    //__Disc_SetLowMem();
	
	/* Selected game */
	header = &gameList[gameSelected];

	//printf("\n");
	//printf("[+] Booting Wii game, please wait...\n");

    GRRLIB_FillScreen(0x000000FF);
    GRRLIB_Render();

    GRRLIB_FillScreen(0x000000FF);
    GRRLIB_Render();

	Con_Clear();

    //GRRLIB_Exit();
	
	/* Set WBFS mode */
	Disc_SetWBFS(WBFS_DEVICE_USB,header->id);
		
	/* Open disc */
	ret = Disc_Open();
	if (ret < 0) {
		//printf("    ERROR: Could not open game! (ret = %d)\n", ret);
		//goto out;
		return false;
	}

	ret = Disc_WiiBoot();
    if (ret < 0) {
        //printf("    ERROR: BOOT ERROR! (ret = %d)\n", ret);
        SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
    }


	//printf("    Returned! (ret = %d)\n", ret);

//out:
	//printf("\n");
	//printf("    Press any button to continue...\n");
	return true;
}


//---------------------------------------------------------------------------------
int main( int argc, char **argv ){
//---------------------------------------------------------------------------------
	#ifndef TEST_MODE
	//__Disc_SetLowMem();
	/* Load Custom IOS */
	int ret = IOS_ReloadIOS(249);
	/* Check if Custom IOS is loaded */
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Custom IOS could not be loaded! (ret = %d)\n", ret);

		return 0;
	}
	#endif
		
	GRRLIB_Init();
    GRRLIB_FillScreen(0x000000FF);
    GRRLIB_Render();
    GRRLIB_FillScreen(0x000000FF);
    GRRLIB_Render();
	
    gradient_texture = GRRLIB_LoadTexture(gradient_bg_png);
    loader_main_texture = GRRLIB_LoadTexture(loading_main_png);
    progress_texture = GRRLIB_LoadTexture(progress_png);
	
	usb_error_texture = GRRLIB_LoadTexture(usb_error_png);
		
	Paint_Progress(progress);
	
	#ifndef TEST_MODE
	if(!init_usbfs())
		return 0;
	
	Paint_Progress(progress);
		u32 timeout = 30;
	my_wbfsDev = WBFS_DEVICE_USB;

	/* Initialize WBFS */
	ret = WBFS_Init(my_wbfsDev, timeout);
	
	//bool flip = true;
  USB_RETRY:
	if(!Init_Game_List())
	{
		while(1)
		{
			WPAD_ScanPads();
		
			GRRLIB_DrawImg(0, 0, usb_error_texture, 0, 1, 1, 0xFFFFFFFF);
			GRRLIB_Render();
				
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
			{
				goto USB_RETRY;
			}
			
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
			{
				GRRLIB_Exit();
				exit(0);
			}
		}
	}
	
	Paint_Progress(progress);
	
	#else
	gameCnt = 29;
	Init_Covers();
	#endif
	

	Paint_Progress(1.0);

	//WPAD_Init();
	//PAD_Init();
	
	//cover_texture = GRRLIB_LoadTexture(RC8P7D_png);

	selected = false;
    //int wait_time = 0;
	
	f32 free, used;

	/* Get free space */
	//WBFS_DiskSpace(&used, &free);
	
	while(1) {

		WPAD_ScanPads();
		//PAD_ScanPads();


		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			GRRLIB_Exit(); 
			exit(0);
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			if(selected && animate_flip >= 1.0)
			{
				selected = false;
			}
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if(gameCnt)
			{
				if(!selected && animate_flip <= 0.0)
				{
					selected = true;
					LoadCurrentCover(gameSelected);
				}
				else if(selected && animate_flip == 1.0)
				{
					//TODO Prompt to boot game...
					if(!Menu_Boot())
					{
						selected = false;
						animate_flip = 0;
					}
					else
					{
						return 0;
					}
				}
			}
		}

		if(!selected && animate_flip == 0)
		{
			if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT )
			
			{	
				if(!((int)shift-1 <= (-1)*(COVER_COUNT/2.0)))
					shift -= SCROLL_SPEED;
			}
			else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT)
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
		else
		{
			DrawSlider();
		}
        GRRLIB_Render();

	}
	
	//Preview
	
    GRRLIB_Exit(); 
	
	return 0;
}
 
