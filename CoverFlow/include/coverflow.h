#ifndef __COVERFLOW_H_
#define __COVERFLOW_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <sys/dir.h>
#include <ogc/lwp_watchdog.h>   // Needed for gettime and ticks_to_millisecs
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
#include "GRRLIB.h"
#include "pngu/pngu.h"
#include "fat.h"
#include "sys.h"
#include "buffer.h"
#include "gfx.h"
#include "mxml.h"
#include "storage.h"
#include "cover.h" 

/*DOL TEST*/
// To test dol, build the bootloader, then copy
// the bootloader to the Coverflow/bootloader/ folder
// Uncomment the code below and then run make.
// You also will want to remove .init=0x80a00100 from this make file

#include "dol.h" 
#define DOL_TEST 1
extern const u8 bootloader_dol[];

/*DOL TEST END*/

#include "settings.h"
 
#define COVER_WIDTH    160
#define COVER_HEIGHT   224
#define DEFAULT_FIFO_SIZE  (256*1024)
//#define TEST_MODE 1
#define DEBUG 1

/* Constants */
#define ENTRIES_PER_PAGE 	 12
#define MAX_CHARACTERS	 	 30
#define BUFFER_WINDOW        9
#define BUFFER_THREAD_COUNT  1
#define USBLOADER_PATH       "SD:/usb-loader"
#define MAX_COVERS           19

#define ANIMATE_TEST 1

#define ANIMATE_SPEED 0

extern const u8 no_cover_png[];
extern const u8 back_cover_png[];
extern const u8 no_disc_png[];
extern const u8 BMfont5_png[];
extern const u8 loading_main_png[];
extern const u8 progress_png[];
extern const u8 gradient_bg_png[];
extern const u8 slide_bar_white_png[];
extern const u8 slide_bar_black_png[];
extern const u8 generic_point_png[];
extern const u8 menu_bg_png[];
extern const u8 gradient_bg_strip_w_png[];
extern const u8 gradient_bg_strip_b_png[];

GRRLIB_texImg covers[MAX_COVERS];
GRRLIB_texImg pointer_texture;

int COVER_COUNT;
int SCR_WIDTH;
int SCR_HEIGHT;
WPADData *wd;

typedef struct{
	float p_x;
    float p_y;
	float p_ang;
} s_pointer;

typedef struct{

	char debugMsg[1024];
	float shift;
    float select_shift;
    s32 gameCnt;
	s32 gameSelected;
	s32 gameStart;
	bool selected;
	int array_size;
	int rumbleAmt;
	float progress;
	
	/*Animate Parameters*/
	float animate_flip;
	float animate_rotate;
	float animate_load;
	int animate_load_speed;
	int animate_direction;
	
	int animate_count;
	int animate_slide_x;
	
	bool firstTimeDownload;
	bool inetOk;
	
} s_self;

void initVars();
int Net_Init(char *ip);
void UpdateBufferedImages();

#endif
