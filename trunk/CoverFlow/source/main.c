/*---------------------------------------------------------------------------------

	CoverFlow by Beardface

---------------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "wpad.h"

#include "wbfs.h"
#include "wdvd.h"
#include "libwbfs/libwbfs.h"

#include "disc.h"
#include "cfg.h"
#include "network.h"
#include "sys/errno.h"
#include "sys/unistd.h"
#include "http.h"

//#include <iostream> 
 
#include "GRRLIB.h"
#include "pngu/pngu.h"
#include "fat.h"
#include "sys.h"
 
#define COVER_WIDTH		160
#define COVER_HEIGHT		224

//#include <vector>
 
#define DEFAULT_FIFO_SIZE	(256*1024)

//#define TEST_MODE 1
#define DEBUG 1

/* Constants */
#define ENTRIES_PER_PAGE	12
#define MAX_CHARACTERS		30

//---------------------------------------------
/*Buffering Code [untested]*/
#define BUFFER_TEST 1
#define BUFFER_WINDOW 8
#define BUFFER_THREAD_COUNT 1
#include "buffer.h"


//---------------------------------------------


static char prozent[MAX_CHARACTERS + 16];
static char timet[MAX_CHARACTERS + 16];
	
/* Gamelist buffer */
static struct discHdr *gameList = NULL;


static wbfs_t *hdd = NULL;

/* Gamelist variables */
static s32 gameCnt = 0, gameSelected = 0, gameStart = 0;

/* WBFS device */
static s32 my_wbfsDev = WBFS_DEVICE_USB;

#define BACK_BUTTON   1000
#define CANCEL_BUTTON 1001
#define OK_BUTTON     1002


float shift = 0.0;
float select_shift = 0.0;

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

bool firstTimeDownload = true;
bool inetOk = false;
bool imageNotFound = false;

extern const u8		no_cover_png[];

extern const u8		back_cover_png[];

extern const u8		no_disc_png[];

extern const u8     font1_png[];

extern const u8     BMfont5_png[];

extern const u8     loading_main_png[];

extern const u8     progress_png[];

extern const u8     gradient_bg_png[];

extern const u8     slide_bar_png[];

extern const u8     usb_error_png[];

extern const u8     generic_point_png[];

extern const u8     menu_bg_png[];

GRRLIB_texImg cover_texture;
GRRLIB_texImg back_texture;

GRRLIB_texImg empty_texture;

GRRLIB_texImg no_disc_texture;
GRRLIB_texImg current_cover_texture;

GRRLIB_texImg text_font1;
GRRLIB_texImg helvetica;

GRRLIB_texImg loader_main_texture;

GRRLIB_texImg progress_texture;

GRRLIB_texImg gradient_texture;
GRRLIB_texImg menu_bg_texture;

GRRLIB_texImg slide_bar_texture;

GRRLIB_texImg usb_error_texture;
GRRLIB_texImg tex_BMfont5;

/*--------------------------------------
  Button Textures
---------------------------------------*/
#include "button.h"

extern const u8     add_button_png[];
extern const u8     add_button_hover_png[];

extern const u8     slide_png[];
extern const u8     slide_hover_png[];

extern const u8     load_png[];
extern const u8     load_hover_png[];

extern const u8     back_png[];
extern const u8     back_hover_png[];

extern const u8     ok_png[];
extern const u8     ok_hover_png[];

extern const u8     cancel_png[];
extern const u8     cancel_hover_png[];

extern const u8     yes_png[];
extern const u8     yes_hover_png[];

extern const u8     no_png[];
extern const u8     no_hover_png[];

extern const u8     cheatson_png[];
extern const u8     cheatson_hover_png[];
extern const u8     cheatsoff_png[];
extern const u8     cheatsoff_hover_png[];

extern const u8     delete_png[];
extern const u8     delete_hover_png[];

Button addButton;
Button slideButton;
Button okButton;
Button backButton;
Button cancelButton;
Button loadButton;
Button cheatonButton;
Button cheatoffButton;
Button yesButton;
Button noButton;
Button deleteButton;

char* _title;
char* _msg;

/*--------------------------------------*/

#define USBLOADER_PATH		"SD:/usb-loader"

Mtx GXmodelView2D;

u8 ocarinaChoice = 0;

#define MAX_COVERS 19

int array_size = 0;
GRRLIB_texImg covers[MAX_COVERS];      //std::vector<GRRLIB_texImg> covers;

float p_ang = 0;
float p_x   = 0;
float p_y   = 0;

WPADData *wd;


char debugMsg[1024];
GRRLIB_texImg pointer_texture;

void Download_Cover(struct discHdr *header);
int WindowPrompt(char* title, char* txt, struct Button* choice_a, struct Button* choice_b);


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

#ifdef BUFFER_TEST
int buffer_window_min = 0;
int buffer_window_max = 0;
int oldmin = 0;
int oldmax = 0;

void DrawBufferedCover(int i, float loc, float angle)
{
	if(i < MAX_BUFFERED_COVERS || i >= 0)
	{
		if(BUFFER_IsCoverReady(i))
		{
			pthread_mutex_lock(&buffer_mutex[i]);
			if(_texture_data[i].data)
			{
				GRRLIB_DrawCoverImg(loc*1.2,_texture_data[i],angle,1.0,0xFFFFFFFF);
			}
			else
			{
				GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
			}
			pthread_mutex_unlock(&buffer_mutex[i]);
		}
		else
		{
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
		}	
	}
	else
	{
		GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
	}	

}


void UpdateBufferedImages()
{
	int i;
	int index = 0;
	
	for(i = (-1*(COVER_COUNT/2.0)); i < (COVER_COUNT/2.0); i++)
	{
		index = i+(COVER_COUNT/2.0);
		
		/*Some logic to avoid drawing everything*/
		if(abs(shift+i) < BUFFER_WINDOW)
		{
			//Is this cover already loaded?
			if(!BUFFER_IsCoverReady(index))
			{
				//Is this cover already queued up?
				if(!BUFFER_IsCoverQueued(index))
				{
					//Request this cover
					if(index < gameCnt)
					{
						struct discHdr *header = &gameList[index];
						//char buff[200];
						//sprintf(buff, "%s min=%d max=%d i=%d shift=%f", get_title(header), buffer_window_min, buffer_window_max, index, shift);
						//WindowPrompt ("Requesting Cover...", buff ,&okButton,0);
		
						BUFFER_RequestCover(index, header);
					}
				}
			}
		}
		else
		{
		//	BUFFER_RemoveCover(index);
		}
	}
	
}


#endif

void quit()
{
	exit(0);
}

void Init_Buttons()
{

    addButton   = Button_Init(add_button_png, add_button_hover_png, 580, 417);
	slideButton = Button_Init(slide_png,  slide_hover_png, 580, 400);
	okButton    = Button_Init(ok_png,   ok_hover_png, 220, 250);
	loadButton  = Button_Init(load_png,   load_hover_png, 220, 300);
	deleteButton  = Button_Init(delete_png,   delete_hover_png, 220, 400);
	backButton  = Button_Init(back_png,   back_hover_png, 340, 300);
	cancelButton = Button_Init(cancel_png, cancel_hover_png, 340, 250);
    cheatonButton = Button_Init(cheatson_png, cheatson_hover_png, 30,420);
    cheatoffButton = Button_Init(cheatsoff_png, cheatsoff_hover_png, 30,420);
	
	yesButton  = Button_Init(yes_png, yes_hover_png, 220, 250);
	noButton = Button_Init(no_png, no_hover_png, 340, 250);
}

void Hover_Buttons()
{
	Button_Hover(&addButton,    p_x, p_y);
	Button_Hover(&okButton,     p_x, p_y);
	Button_Hover(&slideButton,  p_x, p_y);
	Button_Hover(&loadButton,   p_x, p_y);
	Button_Hover(&backButton,   p_x, p_y);
	Button_Hover(&cancelButton, p_x, p_y);
	Button_Hover(&yesButton,    p_x, p_y);
	Button_Hover(&noButton,     p_x, p_y);
	Button_Hover(&deleteButton,     p_x, p_y);
	if (ocarinaChoice)
	{
		Button_Hover(&cheatonButton,  p_x, p_y);
	}
	else Button_Hover(&cheatoffButton,  p_x, p_y);
        
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
	#ifdef DEBUG
	GRRLIB_Printf(160, 255, tex_BMfont5,  0x444444FF, 1, "%s", debugMsg);
    #endif
    
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
	
	//sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c.png", header->id[0],header->id[1],header->id[2]);
	sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c%c.png", header->id[0],header->id[1],header->id[2],header->id[3]);

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

void ClearCovers()
{
	#ifndef BUFFER_TEST
	int i;
	for(i = 0; i < array_size; i++)
	{
		free(covers[array_size].data);
	}
	
	array_size = 0;
	#else
	BUFFER_ClearCovers();
	#endif
}

void Init_Covers()
{
	int i;
	
	ClearCovers();
	
	progress+=0.05;
	Paint_Progress(progress);
	
	float max_progress = 1.7;
	
	float per_game_prog = max_progress/gameCnt;
	
	#ifndef TEST_MODE
	
	#ifndef BUFFER_TEST
	for(i = 0; i < gameCnt; i++)
	{
		void *imgData;// = (void *)no_cover_png;

		char filepath[128];
		s32  ret;

		struct discHdr *header = &gameList[i];
		
		if(array_size < MAX_COVERS)
		{
			sprintf(debugMsg, "Checking next cover...%s", header->id);
			Paint_Progress(1);
			Download_Cover(header);
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
		}
		progress+=per_game_prog;
		Paint_Progress(progress);
	}
	#endif
	
	#else
	
	int CoverCount = COVER_COUNT;
	
	for(i = 0; i < CoverCount; i++)
	{
		AddCover( GRRLIB_LoadTexture(no_cover_png) );
		progress+=per_game_prog;
		Paint_Progress(progress);
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

	  static const float SPACING = 2.8;
	  float dir = 1;
	  float loc, scale, angle;

	  if (pos < 0) {
		dir *= -1;
		pos *= -1;
	  }
	  
	  loc = SPACING * dir * (pow(pos + 1, -1) - 1);
	  scale = pow(pos + 1, -2);
	  angle = -1 * dir * change_scale(scale, 0, 1, 90, 0);
	
	  #ifdef BUFFER_TEST
	  DrawBufferedCover(texture_id, loc, angle);
	  #else
	
	  if(texture_id != -1 && texture_id < array_size)
	  {
			GRRLIB_DrawCoverImg(loc*1.2,covers[texture_id],angle,1.0,0xFFFFFFFF);
	  }
	  else
	  {
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
	  }
	  #endif
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
		GRRLIB_DrawCoverImg(loc*1.2,back_texture,angle,1.4,0xFFFFFFFF);
		
		if(scale >= 360)
		{
			int i ;
			int len;
			animate_rotate++;
			if(animate_rotate == 360) animate_rotate = 0;
			
			GRRLIB_DrawImg(230,100, current_cover_texture, animate_rotate, 1, 1, 0xFFFFFFFF);
			
			Button_Paint(&loadButton);
			Button_Paint(&backButton);
			Button_Paint(&deleteButton);
			
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
	    #ifdef BUFFER_TEST
		DrawBufferedCover(gameSelected, loc, angle);
		#else
	    if(gameSelected < array_size)
		{
			GRRLIB_DrawCoverImg(loc*1.2,covers[gameSelected],angle,1.0,0xFFFFFFFF);
		}
		else
		{
			GRRLIB_DrawCoverImg(loc*1.2,cover_texture,angle,1.0,0xFFFFFFFF);
		}
		#endif
	  }
}

void draw_game_title(int index)
{
	if(index != -1)
	{
			int len = 0;
			struct discHdr *header = NULL;
			header = &gameList[index];
			
			float tsize = 1;

			len = strlen(header->title);
			
			int offset = (len*5);
			
			if(offset > 240) offset = 240;
			
            GRRLIB_Printf(340 - offset, 400, tex_BMfont5, 0xFFFFFFFF, tsize, "%s", header->title);
				
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
	
	GRRLIB_DrawImg(120, 410, slide_bar_texture, 0, 1, 1, 0xFFFFFFFF);
	
	int x = change_scale(shift, -1*(COVER_COUNT/2.0), COVER_COUNT/2.0, min_loc, max_loc);
	
	slideButton.x = 126+x;
	slideButton.y = 426;
	
	//GRRLIB_DrawImg(126+x, 426, slide_texture, 0, 1, 1, 0xFFFFFFFF);
	
	Button_Paint(&slideButton);
	
}

void DragSlider(int xPos)
{

	int min_loc = 126;
	int max_loc = 439;
		
	shift = change_scale(xPos, min_loc, max_loc, -1*(COVER_COUNT/2.0), COVER_COUNT/2.0);

}

int DiscWait()
{
    u32 cover = 0;
	s32 ret = 0;

	while(!(cover & 0x2))
	{
		//TODO Add GUI For Cancel Button
		
		ret = WDVD_GetCoverStatus(&cover);
		if (ret < 0)
			return ret;
	}


	return ret;
}

int WindowPrompt(char* title, char* txt, struct Button* choice_a, struct Button* choice_b)
{
	/*TODO Create Graphical Prompt*/
	
	bool doloop = true;
	
	if(choice_a == 0 && choice_b == 0)
	{
		doloop = false;
	}
	else
	{
		doloop = true;
	}
	
	
	do{

		WPAD_ScanPads();
		
		ir_t ir; // The struct for infrared
		
		WPAD_IR(WPAD_CHAN_0, &ir); // Let's get our infrared data
		wd = WPAD_Data(WPAD_CHAN_0);

		p_x = ir.sx-200;
		p_y = ir.sy-250;
		p_ang = ir.angle/2; // Set angle/2 to translate correctly

		Hover_Buttons();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			quit();
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
		{
			return false;
		}
		
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
		{
			if(choice_a != 0)
			{
				if(Button_Select(choice_a, p_x, p_y))
				{
					return true;
				}
			}
			
			if(choice_b != 0)
			{
				if(Button_Select(choice_b, p_x, p_y))
				{
					return false;
				}
			}
		}
		
		Hover_Buttons();
		
		GRRLIB_DrawImg(0, 0,    gradient_texture, 0, 1, 1, 0xFFFFFFFF);
		GRRLIB_DrawImg(120, 60, menu_bg_texture, 0, 1, 1, 0xFFFFFFFF);
		
		if(choice_a != 0)
		{
			Button_Paint(choice_a);
		}
		
		if(choice_b != 0)
		{
			Button_Paint(choice_b);
		}
		
		/*Draw Text*/
        GRRLIB_Printf(140, 70, tex_BMfont5,  0xFFFFFFFF, 1.5, "%s", title);
        GRRLIB_Printf(160, 110, tex_BMfont5, 0xFFFFFFFF, 1, "%s", txt);
		
		if(doloop)
			GRRLIB_DrawImg(p_x, p_y, pointer_texture, p_ang, 1, 1, 0xFFFFFFFF);
		
		GRRLIB_Render();
		
	}while(doloop);
	
	return false;
}
/****************************************************************************
 * ShowProgress
 *
 * Updates the variables used by the progress window for drawing a progress
 * bar. Also resumes the progress window thread if it is suspended.
 ***************************************************************************/
void
ShowProgress (s32 done, s32 total)
{

    static time_t start;
	static u32 expected;

    f32 percent; //, size;
	u32 d, h, m, s;

	//first time
	if (!done) {
		start    = time(0);
		expected = 300;
	}

	//Elapsed time
	d = time(0) - start;

	if (done != total) {
		//Expected time
		if (d)
			expected = (expected * 3 + d * total / done) / 4;

		//Remaining time
		d = (expected > d) ? (expected - d) : 0;
	}

	//Calculate time values
	h =  d / 3600;
	m = (d / 60) % 60;
	s =  d % 60;

	//Calculate percentage/size
	percent = (done * 100.0) / total;

	sprintf(prozent, "%s%0.2f%%", "Installing Game...", percent);

    sprintf(timet,"Time left: %d:%02d:%02d",h,m,s);

	/*Update and Draw Progress Window Here*/
	WindowPrompt(prozent, timet, 0, 0);
}


int ProgressWindow(char* title, char* msg)
{
	/*TODO Draw Window*/
	_title = title;
	_msg   = msg;
	
	int ret = wbfs_add_disc(hdd, __WBFS_ReadDVD, NULL, ShowProgress, ONLY_GAME_PARTITION, 0);
	
	progress = 0.0;
	
	return ret;

}

bool Menu_Install(void)
{

    static struct discHdr headerdisc ATTRIBUTE_ALIGN(32);
	
	WindowPrompt ("Initializing DVD Drive", "Please Wait...",0,0);
	
	/* Disable WBFS mode */
	Disc_SetWBFS(0, NULL);
	
    int ret, choice = 0;
	char *name;
	static char buffer[MAX_CHARACTERS + 4];

	ret = Disc_Wait();
	if (ret < 0) {
		WindowPrompt ("Error reading Disc",0,&cancelButton,0);
		return false;
	}
	ret = Disc_Open();
	if (ret < 0) {
		WindowPrompt ("Could not open Disc",0,&cancelButton,0);
		return false;
	}

	ret = Disc_IsWii();
	
	if (ret < 0) {
		choice = WindowPrompt ("Not a Wii Disc","Insert a Wii Disc!",&okButton,&cancelButton);

		if (!choice) {
			return false;
		}
		else
		{
			return Menu_Install();
		}
	}
	
	Disc_ReadHeader(&headerdisc);
	name = headerdisc.title;
	if (strlen(name) < (22 + 3)) {
			memset(buffer, 0, sizeof(buffer));
			sprintf(name, "%s", name);
		} else {
			strncpy(buffer, name,  MAX_CHARACTERS);
			strncat(buffer, "...", 3);
			sprintf(name, "%s", buffer);
	}

	ret = WBFS_CheckGame(headerdisc.id);
	if (ret) {
		WindowPrompt ("Game is already installed:",name,&cancelButton,0);
		return false;
	}
	hdd = GetHddInfo();
	if (!hdd) {
		WindowPrompt ("No HDD found!","Error!!",&cancelButton,0);
		return false;
		}

	f32 freespace, used;

	WBFS_DiskSpace(&used, &freespace);
	u32 estimation = wbfs_estimate_disc(hdd, __WBFS_ReadDVD, NULL, ONLY_GAME_PARTITION);
	f32 gamesize = ((f32) estimation)/1073741824;
	char gametxt[50];
	
	sprintf(gametxt, "Installing game %.2fGB:", gamesize);
	
	char ttext[50];
	char tsize[50];
	sprintf(ttext, "Install %s?", name);
	sprintf(tsize, "Game Size: %.2fGB", gamesize);
	
	
	if(WindowPrompt (ttext,tsize,&okButton,&cancelButton))
	{
		if (gamesize > freespace) {
			char errortxt[50];
			sprintf(errortxt, "Game Size: %.2fGB, Free Space: %.2fGB", gamesize, freespace);
			WindowPrompt("Not enough free space!",errortxt,&cancelButton, 0);
			return false;
		}
		else {
			ret = ProgressWindow(gametxt, name);
			if (ret != 0) {
				WindowPrompt ("Install error!",0,&cancelButton,0);
				return false;
			} else {
				GetEntries();
				WindowPrompt ("Successfully installed:",name,&okButton,0);
				return true;
			}
		}
	}
	
	return false;
}

void AddGame(void)
{
	Menu_Install();
}

bool Menu_Delete(void)
{
	struct discHdr *header = NULL;
 	char gameName[31]; 
	
	/* No game list */
	if (!gameCnt)
		return false;

	/* Selected game */
	header = &gameList[gameSelected];

	if(strlen(get_title(header)) < 30) {
		sprintf(gameName, "%s", get_title(header));
	}
	else
	{
		strncpy(gameName, get_title(header), 27);
		gameName[27] = '\0';
		strncat(gameName, "...", 3);
	}

	if(WindowPrompt("Do you want to delete:", gameName, &yesButton, &noButton))
	{
		if(0 > WBFS_RemoveGame(header->id))
		{
			WindowPrompt("Delete Failed.", "Could not delete game.", &okButton, 0);
		}
		else
		{
			GetEntries();
			WindowPrompt("Successfully deleted.", "Press Ok to Continue.", &okButton, 0);
			return true;
		}
	}
	
	return false;
}
		

bool Menu_Boot(void)
{
	#ifndef TEST_MODE
	struct discHdr *header = NULL;
	int i = 0;
	s32 ret;

	/* No game list */
	if (!gameCnt)
		return false;

	/* Selected game */
	header = &gameList[gameSelected];

    GRRLIB_Exit();
	
	#ifndef BUFFER_TEST
	for(i = 0; i < array_size; i++)
	{
		free(covers[i].data);
	}
	#endif
	
	free(cover_texture.data);
	free(back_texture.data);
	free(empty_texture.data);
	free(no_disc_texture.data);
	free(current_cover_texture.data);
	free(text_font1.data);

	BUFFER_ClearCovers();
	BUFFER_KillBuffer();
	Sleep(300);
	
	/* Set WBFS mode */
	Disc_SetWBFS(WBFS_DEVICE_USB,header->id);
		
	/* Open disc */
	ret = Disc_Open();
	if (ret < 0) {
		return false;
	}

	ret = Disc_WiiBoot();
    if (ret < 0) {
        SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
    }

	#endif
	
	return true;
}


int Net_Init(char *ip){
	
	s32 res;
    while ((res = net_init()) == -EAGAIN)
	{
		usleep(100 * 1000); //100ms
	}
	
    if (if_config(ip, NULL, NULL, true) < 0) {
		printf("      Error reading IP address, exiting");
		usleep(1000 * 1000 * 1); //1 sec
		return false;
	}
	return true;
}

void saveFile(char* imgPath, struct block file){
	//printf("\n\n    Size: %d byte", file.size);
			
	/* save png to sd card for future use*/
			
	FILE *f;
	f = fopen(imgPath, "wb");
	if (f)
	{
		fwrite(file.data,1,file.size,f);
		fclose (f);
	}
}

void Download_Cover(struct discHdr *header)
{
	char imgPath[100];

	if (!header)
		return;
		
	//the first time no image is found, attempt to init network
	/* Initialize Network <<<TO BE THREADED>>> */
	if(firstTimeDownload == true){
		char myIP[16];
		//printf("\n[+] Initializing Network.");
		sprintf(debugMsg, "Initializing Network");
		Paint_Progress(1);
		//GRRLIB_Printf(100, 100, text_font1, 0XFFFFFFFF, 1.0, "Initializing Network");
		if( Net_Init(myIP) ){
			//GRRLIB_Printf(100, 120, tex_BMfont5, 0XFFFFFFFF, 1.0, "Error Initializing Network");
			sprintf(debugMsg, "Error Initializing Network");
			Paint_Progress(1);
			inetOk = true;
		}
		else{
			sprintf(debugMsg, "Network Initialized");
			Paint_Progress(1);
		}

		firstTimeDownload = false;
	}
	
	/*
	GRRLIB_Render();
	usleep(50000);
	GRRLIB_Exit(); 
    free(tex_BMfont5.data);
	exit (0);
	*/
		
	if(inetOk) {
		//printf("\n    Network connection established.");
		/*try to download image */
			
		char url[100];
		struct block file;
	
		char region[4];
		switch(header->id[3]){
	
		case 'E':
			sprintf(region,"ntsc");
			break;

		case 'J':
			sprintf(region,"ntscj");
			break;

		case 'P':
			sprintf(region,"pal");
			break;
		}
		//printf("\n    Downloading cover...");

		snprintf(imgPath, sizeof(imgPath), "%s/covers/%s.png", CFG.images_path, header->id);
		
		sprintf(debugMsg, "Checking presence of %s", imgPath);
		Paint_Progress(1);
		
		FILE *fp;
		fp = fopen(imgPath, "rb");
		if (fp)
		{
			fclose (fp);
			sprintf(debugMsg, "%s present, not downloading", imgPath);
			Paint_Progress(1);
		}
		else{
			
			/*
			if (CFG.widescreen)
				sprintf(url, "http://www.theotherzone.com/wii/widescreen/%s/%s.png", region, header->id);
			else
				sprintf(url, "http://www.theotherzone.com/wii/%s/%s.png", region, header->id);
			*/
			
			sprintf(url, "http://www.theotherzone.com/wii/resize/%s/160/224/%s.png", region, header->id);
			sprintf(debugMsg, "Getting %s", url);
			Paint_Progress(1);
		
			file = downloadfile(url);
			
			if(file.data != NULL){
				saveFile(imgPath, file);
				free(file.data);
				sprintf(debugMsg, "done");
			    Paint_Progress(1);
				//else
					//donotdownload = true;
			}
			else {
				sprintf(debugMsg, "some error occurred");
				Paint_Progress(1);
			}
		}
		
		snprintf(imgPath, sizeof(imgPath), "%s/disks/%c%c%c%c.png", CFG.images_path,  header->id[0], header->id[1], header->id[2], header->id[3]);
		sprintf(debugMsg, "Checking presence of %s", imgPath);
		Paint_Progress(1);
		
		fp = fopen(imgPath, "rb");
		if (fp)
		{
			fclose (fp);
			sprintf(debugMsg, "%s present, not downloading", imgPath);
			Paint_Progress(1);
		}
		else{
			sprintf(url, "http://www.theotherzone.com/wii/diskart/160/160/%c%c%c%c.png", header->id[0], header->id[1], header->id[2], header->id[3]);
			sprintf(debugMsg, "Getting %s", url);
			Paint_Progress(1);
			
			file = downloadfile(url);
			
			if(file.data != NULL){
				saveFile(imgPath, file);
				free(file.data);
				sprintf(debugMsg, "done");
			    Paint_Progress(1);
				//else
					//donotdownload = true;
			}
			else {
				sprintf(debugMsg, "some error occurred");
				Paint_Progress(1);
			}
		//else
			//donotdownload = true;
		}
	}
	//refresh = true;				
} /* end download */

void LoadTextures()
{
	usb_error_texture = GRRLIB_LoadTexture(usb_error_png);
	pointer_texture   = GRRLIB_LoadTexture(generic_point_png);
	menu_bg_texture   = GRRLIB_LoadTexture(menu_bg_png);
	
	cover_texture = GRRLIB_LoadTexture(no_cover_png);
	back_texture = GRRLIB_LoadTexture(back_cover_png);
        no_disc_texture = GRRLIB_LoadTexture(no_disc_png);
	text_font1 = GRRLIB_LoadTexture(font1_png);
	
	slide_bar_texture = GRRLIB_LoadTexture(slide_bar_png);
	
    GRRLIB_InitTileSet(&helvetica, 22, 40, 0);
    GRRLIB_InitTileSet(&text_font1, 32, 36, 32);
	
    tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);
    GRRLIB_InitTileSet(&tex_BMfont5, 8, 16, 0);
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
	
    gradient_texture    = GRRLIB_LoadTexture(gradient_bg_png);
    loader_main_texture = GRRLIB_LoadTexture(loading_main_png);
    progress_texture    = GRRLIB_LoadTexture(progress_png);
	
	sprintf(debugMsg, "Loading textures");
	Paint_Progress(progress);
	
	LoadTextures();
	
	progress += .1;
	sprintf(debugMsg, "Init USB");
	Paint_Progress(progress);
	
	#ifndef TEST_MODE
	if(!init_usbfs())
		return 0;
		
	//LOAD CONFIG
	strcpy(CFG.images_path, USBLOADER_PATH);
	CFG.widescreen = 0;
	CFG.download = 1;
	//HARDCODED FOR NOW
	
	sprintf(debugMsg, "Initializing WBFS");
	Paint_Progress(progress);
	
	my_wbfsDev = WBFS_DEVICE_USB;

  INIT_RETRY:
	/* Initialize WBFS */
	ret = WBFS_Init(my_wbfsDev);
	
	if(ret < 0)
	{
		while(1)
		{
			WPAD_ScanPads();
		
			GRRLIB_DrawImg(0, 0, usb_error_texture, 0, 1, 1, 0xFFFFFFFF);
			GRRLIB_Render();
				
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
			{
				goto INIT_RETRY;
			}
			
			if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
			{
				GRRLIB_Exit();
				SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
			}
		}
	}
	
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
				SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
			}
		}
	}
	
	#else
	gameCnt = 29;
	Init_Covers();
	#endif
	
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	Init_Buttons();
	
	#ifdef BUFFER_TEST
	BUFFER_InitBuffer(BUFFER_THREAD_COUNT);
	UpdateBufferedImages();
	
	int wait = 300; //ms
	float prog = 2.1/300.0;
	
	sprintf(debugMsg, "Initializing Threaded Image Buffer...");
	while(wait > 0)
	{
		wait--;
		progress += prog;
		Paint_Progress(progress);
		Sleep(1);
	}
	
	#endif
	
	
	sprintf(debugMsg, "Freeing unused textures");
	Paint_Progress(progress);
	
	free(gradient_texture.data);
	free(loader_main_texture.data);
	free(progress_texture.data);
	free(usb_error_texture.data);
	
	Sleep(300);
	
	selected = false;
	
	bool select_ready = false;
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	
	#ifdef TEST_MODE
	PAD_Init();
	#endif
	
	bool dragging = false;
	
	while(1) {

		WPAD_ScanPads();
		
		#ifdef TEST_MODE
		PAD_ScanPads();
		#endif
		
		ir_t ir; // The struct for infrared
		
		WPAD_IR(WPAD_CHAN_0, &ir); // Let's get our infrared data
		wd = WPAD_Data(WPAD_CHAN_0);

		p_x = ir.sx-200;
		p_y = ir.sy-250;
		p_ang = ir.angle/2; // Set angle/2 to translate correctly

		Hover_Buttons();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			BUFFER_ClearCovers();
			BUFFER_KillBuffer();
			Sleep(300);
			quit();
		}
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B ||
			PAD_ButtonsDown(0) & PAD_BUTTON_B)
		{
			if(selected && animate_flip >= 1.0)
			{
				selected = false;
			}
		}
		
		if(dragging)
		{
			if(WPAD_ButtonsHeld(0) & WPAD_BUTTON_A)
			{
				DragSlider(p_x);
			}
			else
			{
				dragging = false;
			}
		}
				
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A ||
			PAD_ButtonsDown(0) & PAD_BUTTON_A)
		{
			//First Check Buttons
			if(Button_Select(&addButton, p_x, p_y))
			{
				AddGame();
			}
			else if(Button_Select(&cheatonButton, p_x, p_y) || Button_Select(&cheatoffButton, p_x, p_y))
			{
				ocarinaChoice = (ocarinaChoice) ? 0 : 1;
			}
			else if(Button_Select(&slideButton, p_x, p_y))
			{
				dragging = true;
			}
			else
			{
				if(gameCnt)
				{
					if(!selected && animate_flip <= 0.0)
					{
						if(p_x < 400 && p_x > 200 &&
							p_y > 60 && p_y < 380)
						{
							if(select_ready && select_shift == 0.0)
							{
								selected = true;
								LoadCurrentCover(gameSelected);
							}
						}
						else if(p_x < 200 &&
							p_y > 60 && p_y < 380)
						{
							select_shift = (-4)*((200-p_x)/200.0);
						}
						else if(p_x > 400 &&
							p_y > 60 && p_y < 380)
						{
							select_shift = 5*(p_x-345.0)/280.0;
						}
					}
					
					if(selected && animate_flip == 1.0)
					{
						if(Button_Select(&loadButton, p_x, p_y))
						{
							#ifdef BUFFER_TEST
							//BUFFER_ClearCovers();
							//BUFFER_KillBuffer();
							#endif
							
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
						else if(Button_Select(&deleteButton, p_x, p_y))
						{
							Menu_Delete();
							selected = false;
						}
						else if(Button_Select(&backButton, p_x, p_y))
						{
							selected = false;
						}
					}
				}
			}
		}

		if(!selected && animate_flip == 0)
		{
			if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT  ||
				PAD_ButtonsDown(0) & PAD_BUTTON_LEFT)
			
			{	
				select_ready = false;
					
				if(!((int)shift-1 <= (-1)*(COVER_COUNT/2.0)))
					shift -= SCROLL_SPEED;
			}
			else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT ||
				PAD_ButtonsDown(0) & PAD_BUTTON_RIGHT)
			{
				select_ready = false;
					
				if(!((int)shift+.5 >= (COVER_COUNT/2.0)))
					shift += SCROLL_SPEED;
			}
			else
			{
				if(abs(select_shift) > SCROLL_SPEED)
				{
					int mult = abs((int)select_shift);
					if(select_shift > 0)
					{
					
						select_shift -= mult*SCROLL_SPEED;
						if(!((int)shift-1 <= (-1)*(COVER_COUNT/2.0)))
							shift -= mult*SCROLL_SPEED;
					}
					else
					{
						select_shift += mult*SCROLL_SPEED;
						if(!((int)shift+.5 >= (COVER_COUNT/2.0)))
							shift += mult*SCROLL_SPEED;
					}
					
				}
				else if(abs(((int)shift * 10000.0) - (shift*10000.0))/10000.0 > (SCROLL_SPEED+SCROLL_SPEED/2.0))
				{
					select_ready = false;
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
					select_shift = 0;
					shift = (int)shift;
					select_ready = true;
					
					/*Draw Game Title*/
					draw_game_title(gameSelected);
				}
					
			}
		}
		
		#ifdef BUFFER_TEST
		UpdateBufferedImages();
		#endif
		
		draw_covers();


		if(selected || animate_flip != 0)
		{
			draw_selected();
		}
		else
		{
			DrawSlider();
			Button_Paint(&addButton);
			if (ocarinaChoice)
			{
				Button_Paint(&cheatonButton);
			}
			else Button_Paint(&cheatoffButton);
                        
		}

		GRRLIB_DrawImg(p_x, p_y, pointer_texture, p_ang, 1, 1, 0xFFFFFFFF);
        GRRLIB_Render();

		Sleep(1);
	}
	
	//Preview
	
    GRRLIB_Exit(); 
	
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	
	return 0;
} 
