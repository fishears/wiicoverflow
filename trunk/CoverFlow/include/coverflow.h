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
#include "wpadsvn.h"
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
//#include "ee.h"
#include "titles.h"

/*DOL TEST*/
// To test dol, build the bootloader, then copy
// the bootloader to the Coverflow/bootloader/ folder
// Uncomment the code below and then run make.
// You also will want to remove .init=0x80a00100 from this make file

//#include "dol.h" 
//#define DOL_TEST 1
//extern const u8 bootloader_dol[];

/*DOL TEST END*/
 
#define COVER_WIDTH    160
#define COVER_HEIGHT   224
//#define DEFAULT_FIFO_SIZE  (256*1024)

/* Aspect ratio fix for some image*/
#define AR_16_9 0.80F //0.85

//#define TEST_MODE 1
#define DEBUG 1
//#define TITLES_TXT_IS_SAFE_BUT_I_COMMENTED_BC_I_DON_T_WANT_TO_ADD_OTHER_FEATURES_BEFORE_ONE_POINT_ZERO_RELEASE

//#define D3_COVERS

//TTF TEST
#define TTF_TEST

#define KB_SIZE         1024.0
#define MB_SIZE         1048576.0
#define GB_SIZE         1073741824.0

/* Constants */
#define MAX_CHARACTERS	 	 30
#define BUFFER_WINDOW        15
#define BUFFER_THREAD_COUNT  1
#define USBLOADER_PATH       "SD:/usb-loader"
#define MAX_COVERS           15
#define RELEASE              "RC 2"

#define ANIMATE_TEST 1
#define ANIMATE_SPEED 0

/* Sound stuff */
#define OGG_FORMAT 0
#define RAW_FORMAT 1
#define MAX_SOUNDS   5

/* CFG stuff */
#define CFG_VIDEO_COUNT  6
#define CFG_HOOK_COUNT   3
#define CFG_LANG_COUNT   11

enum {
	FX_BG_MUSIC = 0,
	FX_COVER_FLIP,
	FX_COVER_SCROLL,
	FX_BUTTON_CLICK,
	FX_TOASTY
};

extern const u8 no_cover_png[];
extern const u8 back_cover_png[];
extern const u8 no_disc_png[];
extern const u8 BMfont5_png[];
extern const u8 loading_main_png[];
extern const u8 progress_png[];
extern const u8 gradient_bg_png[];
extern const u8 slidebar_png[];
extern const u8 generic_point_png[];
extern const u8 pointer_shadow_png[];
extern const u8 turning_point_png[];
extern const u8 menu_bg_png[];
extern const u8 menu_bg2_png[];
extern const u8 font_w14_h20_png[]; // title font
extern const u8 font_w10_h14_png[]; // title font
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
	
	/*Animate Parameters*/
	float animate_flip;
	float animate_rotate;
	float animate_load;
	int animate_load_speed;
	int animate_direction;
	
	int animate_count;
	int animate_slide_x;
	
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
	
	int dummy;

} s_self;

void initVars();

#endif
