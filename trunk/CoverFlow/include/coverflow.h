#ifndef __COVERFLOW_H_
#define __COVERFLOW_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiipad.h>
#include <sys/dir.h>
#include <ogc/lwp_watchdog.h>   // Needed for gettime and ticks_to_millisecs
#include "wbfs.h"
#include "wdvd.h"
#include "libwbfs/libwbfs.h"
#include "disc.h"
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
#include "utils.h"
#include "gameSettings.h"
#include "version.h"
#include "homemenu.h"
#include "subsystem.h"
#include "settings.h"
#include "localization.h"
#include "partition.h"
#include "settingsMenu.h"
#include "installMenu.h"
#include "bootMenu.h"
#include "deleteMenu.h"
#include "soundmanager.h"
#include "CFreeTypeGX.h"
#include "titles.h"
#include "cheats.h"
#include "info.h"
#include "urlLogin.h"
#include "pthread.h"
#include "updater.h"
//#include "trackedMemoryManager.h"

#include "newsreader.h"
#include "fatmounter.h"

#include "defines.h"		/* see for enable/disable definitions  */

#define RELEASE   "v1.3" 	/* shown ReleaseNo in SysInfo-Window   */

/*DOL TEST*/
// To test dol, build the bootloader, then copy
// the bootloader to the Coverflow/bootloader/ folder
// Uncomment the code below and then run make.
// You also will want to remove .init=0x80a00100 from this make file

//#include "dol.h" 
//#define DOL_TEST 1
//extern const u8 bootloader_dol[];

/*DOL TEST END*/

#define BLOCKSIZE 1024

#define COVER_WIDTH    160
#define COVER_HEIGHT   224

#define COVER_WIDTH_3D    512
#define COVER_HEIGHT_3D   340

/* Aspect ratio fix for some image*/
#define AR_16_9 0.80F //0.85

//#define TEST_MODE 1
#define DEBUG 1
#define CHEAT_MANAGER

#define LOUDTEST
 
#define _TEXT(t) L ## t /**< Unicode helper macro. */

#define KB_SIZE         1024.0
#define MB_SIZE         1048576.0
#define GB_SIZE         1073741824.0

/* Constants */
#define MAX_CHARACTERS	 	 30
#define BUFFER_WINDOW        15
#define BUFFER_THREAD_COUNT  1
#define MAX_COVERS           15


#define ANIMATE_SPEED	0
#define FLIP_SPEED  	0.016

/* Sound stuff */
#define OGG_FORMAT 	0
#define RAW_FORMAT 	1
#define MAX_SOUNDS  4

/* CFG stuff */
#define CFG_VIDEO_COUNT  	6
#define CFG_HOOK_COUNT   	7
#define CFG_LANG_COUNT   	11
#define CFG_FIX_COUNT	 	3
#define CFG_ALTDOL_COUNT 	3 
#define CFG_CIOS_COUNT	 	3
#define CFG_SYSCIOS_COUNT	2


/* Y-Offset for gameSettings-Dialogbox */
#define YOS_GSDB	-60

enum {
	FX_BG_MUSIC = 0,
	FX_COVER_FLIP,
	FX_COVER_SCROLL,
	FX_BUTTON_CLICK,
	FX_TOASTY
};

// Fonts
extern u8  font_ttf[]; // the font file (named font.ttf in /fonts/)
extern u32 font_ttf_size;
extern CFreeTypeGX *ttf14pt;
extern CFreeTypeGX *ttf16pt;
extern CFreeTypeGX *ttf18pt;
extern CFreeTypeGX *ttf20pt;
extern CFreeTypeGX *ttf24pt;
CFreeTypeGX *ttf14pt;
CFreeTypeGX *ttf16pt;
CFreeTypeGX *ttf18pt;
CFreeTypeGX *ttf20pt;
CFreeTypeGX *ttf24pt;

extern const u8 full_cover_png[];
extern const u8 no_cover_png[];
extern const u8 no_disc_png[];
extern const u8 progress_step_png[];
extern const u8 progress_bar_png[];
extern const u8 gradient_bg_png[];
extern const u8 slidebar_png[];
extern const u8 pointer_basic_png[];
extern const u8 pointer_shadow_png[];
extern const u8 pointer_turning_png[];
extern const u8 ambientlight_png[];
GRRLIB_texImg covers[MAX_COVERS];
GRRLIB_texImg pointer_texture;
GRRLIB_texImg pointer_shadow_texture;
GRRLIB_texImg turn_point_texture;

SoundFile _sounds[MAX_SOUNDS];

WPADData *wd;
Mtx GXmodelView2D;

typedef struct{
	float p_x;
    float p_y;
	float p_ang;
	int   p_type; // holds the current pointer type 0=default, 1=turning
} s_pointer;

typedef struct{
	bool flip;
	float angle;
} s_coverFlip;

enum 
{
	i249=0,
	i222,
	i223
};

enum 
{
	ios249=0,
	ios222
};


typedef struct{
	char debugMsg[1024];
	float shift;
    float select_shift;
    s32 gameCnt;
	s32 gameSelected;
	s32 lastGameSelected;
	s32 gameStart;
	bool selected;
	int array_size;
	int rumbleAmt;
	f32 gsize;
	float progress;
	int max_cover;
	int min_cover;
	int slot_glow;
	float scroll_speed;
	
	/*Animate Parameters*/
	float animate_flip;
	float animate_rotate;
	float animate_load;
	int animate_load_speed;
	int animate_direction;
	
	int animate_count;
	int animate_slide_x;
	// Single flip animation variables
	int   singleFlipSpeed; // # of frames during a single cover flip animation
	bool  movingLEFT;
	bool  movingRIGHT;
    int   L_CNT;
	int   R_CNT;
	float startingShift; // holds the starting shift position when doing a single flip animation
	
	bool dragging;
	bool twisting;
	bool scrolling;
	
	//bool firstTimeDownload;
	//bool inetOk;
	
	//Wiimote data
	ir_t ir;
	orient_t orient;
	
	s32 my_wbfsDev;
	wbfs_t *hdd;
	struct discHdr *gameList;
	
	bool usingTitlesTxt;
	int titlesTxtSize;
	
	char ipAddress[16]; // 000.000.000.000\0
	
	bool firstTimeGP;  		// First time on graphics panel, used by backup_gpSettings();
	//bool switched3dto2D;	// When installing new games
	bool waiting;			// when waiting, for showing icon
	int dummy;
	
	f32 freeSpace;
	f32 usedSpace;
	u32 deviceID; 
	
	bool enableError002Fix;
	bool enableAnti002Fix;
#ifdef URLBOXART_PASS	 
	char url_username[40];  //wiiboxart.com
	char url_password[40];
	bool url_fileexist;
#endif
	
	bool updateAvailable;
	char linebuf[300];
	
#ifdef NEWS_READER	
	int  blueID;
	bool blinkBlue;
	char newsID[10];
#endif

	char bootDevice[10];
	bool patchCountryStr;
	int  alternatedol;
	u32  alternatedoloffset;
	int  selectedDol;
	
#ifdef OSK
	char kb_buffer[256];
	bool kb_OK;
#endif	

#ifdef GH3
	u32 expType; //controller/expansion type.
#endif
} s_self;


typedef struct
{
 char bootDevice[10];
 char dir_codes[255];
 char dir_usb_loader[255];
 char dir_3dcovers[255];
 char dir_covers[255];
 char dir_disks[255];
 char dir_games[255];
 char dir_txtcodes[255];
 char dir_altdol[255];
} s_path;

void initPaths();
void initVars();

#endif
