#ifndef __BOOTLOADER_H_
#define __BOOTLOADER_H_

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
#include "libwbfs.h"
#include "disc.h"
#include "cfg.h"
#include "sys/errno.h"
#include "sys/unistd.h"
#include "fat.h"
#include "sys.h"

#define DEFAULT_FIFO_SIZE  (256*1024)

/* Constants */
#define ENTRIES_PER_PAGE 	 12
#define MAX_CHARACTERS	 	 30

#define USBLOADER_PATH       "SD:/usb-loader"

#endif
