#ifndef __BOOTLOADER_H_
#define __BOOTLOADER_H_

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>

#include <sys/dir.h>
#include <ogc/lwp_watchdog.h>   // Needed for gettime and ticks_to_millisecs
#include "wpadsvn.h"
#include "wbfs.h"
#include "wdvd.h"
#include "libwbfs.h"
#include "disc.h"

#include "sys/errno.h"
#include "sys/unistd.h"
#include "fat.h"
#include "sys.h"

#define DEFAULT_FIFO_SIZE  (256*1024)

#include "mxml.h" // Because Scognito rox ( == because Scognito is LAZY )

/* Constants */
#define ENTRIES_PER_PAGE 	 12
#define MAX_CHARACTERS	 	 30

#define USBLOADER_PATH       "SD:/usb-loader"

#define KB_SIZE         1024.0
#define MB_SIZE         1048576.0
#define GB_SIZE         1073741824.0

u32 swap32(u32 x);

typedef struct {
    int ocarina;
    int hooktype;
    int language;
    int video;
    int vipatch;
	char localLanguage[5];
} s_bootloader;

#endif
