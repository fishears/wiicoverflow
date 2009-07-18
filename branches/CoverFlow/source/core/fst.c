/*
 *  Copyright (C) 2008 Nuke (wiinuke@gmail.com)
 *
 *  this file is part of GeckoOS for USB Gecko
 *  http://www.usbgecko.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <malloc.h>
#include <sys/unistd.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include "ogc/ipc.h"
#include "fst.h"
#include "dvd_broadway.h"
#include "wiipad.h"
#include "fat.h"
#include "fatmounter.h"
#include "defines.h"

#define FSTDIRTYPE 1
#define FSTFILETYPE 0
#define ENTRYSIZE 0xC

#ifdef USB_DEVICE
	#define FILEDIR	"USB:/codes"
#else
	#define FILEDIR	"SD:/codes"
#endif

#define MAX_FILENAME_LEN	128


static vu32 dvddone = 0;


// Real basic 
u32 do_sd_code(char *filename)
{
	FILE *fp;
	u8 *filebuff;
	u32 filesize;
	u32 ret;
	char filepath[128];
	
	//ret = Fat_MountSDHC();
	ret = SDCard_Init();
	USBDevice_Init();

	if (ret < 0) {
		printf("[+] SD Error\n");
		sleep (2);
		return 0;
	}

	fflush(stdout);
	
	sprintf(filepath, FILEDIR "/%s", filename);
	filepath[16] = 0x2E;
	filepath[17] = 0x67;
	filepath[18] = 0x63;
	filepath[19] = 0x74;
	filepath[20] = 0;
	//printf("filename %s\n",filepath);
	
	fp = fopen(filepath, "rb");
	if (!fp) {
		printf("[+] No SD codes found\n");
		//Fat_UnmountSDHC();
		USBDevice_deInit();
		SDCard_deInit();
		sleep(2);
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	filebuff = (u8*) malloc (filesize);
	if(filebuff == 0){
		fclose(fp);
		sleep(2);
		return 0;
	}

	ret = fread(filebuff, 1, filesize, fp);
	if(ret != filesize){	
		printf("[+] SD Code Error\n");
		free(filebuff);
		fclose(fp);
		//Fat_UnmountSDHC();
		USBDevice_deInit();
		SDCard_deInit();
		sleep(2);
		return 0;
	}
        printf("[+] SD Codes found.\n");

        memcpy((void*)0x800027E8,filebuff,filesize);
        *(vu8*)0x80001807 = 0x01;
	
	

	free(filebuff);
	fclose(fp);
	
	//Fat_UnmountSDHC();
	
	USBDevice_deInit();
	SDCard_deInit();
	
	sleep(2);
	return 1;
}


