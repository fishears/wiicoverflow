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

#endif
