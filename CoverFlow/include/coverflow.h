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
 
#define COVER_WIDTH        160
#define COVER_HEIGHT       224
#define DEFAULT_FIFO_SIZE  (256*1024)
//#define TEST_MODE 1
#define DEBUG 1

/* Constants */
#define ENTRIES_PER_PAGE 	 12
#define MAX_CHARACTERS	 	 30
#define BUFFER_TEST          1
#define BUFFER_WINDOW        9
#define BUFFER_THREAD_COUNT  1
#define USBLOADER_PATH		 "SD:/usb-loader"
#define MAX_COVERS           19
#define DRAW_WINDOW          7


int COVER_COUNT;
//static struct discHdr *gameList = NULL;
GRRLIB_texImg covers[MAX_COVERS];      //std::vector<GRRLIB_texImg> covers;

typedef struct{
	char debugMsg[1024];
	float p_x;
    float p_y;
	float shift;
    float select_shift;
    s32 gameCnt;
	s32 gameSelected;
	s32 gameStart;
	bool selected;
	float animate_flip;
	int array_size;
} s_self;

void initVars();

#endif
