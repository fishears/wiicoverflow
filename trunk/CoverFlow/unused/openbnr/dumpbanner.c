/*
 *  dumpbanner.c
 *
 *  Dump opening.bnr , thanks to WiiPower
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include <gctypes.h>
#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <fat.h>

#include "fatmounter.h"
#include "wdvd.h"
#include "disc.h"
#include "fst.h"
#include "fstfile.h"
#include "dumpbanner.h"

s32 dump_openingbnr(u8 *discID, const char * dest)
{
	// Mount the disc
	Disc_SetUSB(discID);

	Disc_Open();

	u64 offset;
	s32 ret;

	ret = __Disc_FindPartition(&offset);
	if (ret < 0)
		return ret;

		ret = WDVD_OpenPartition(offset);

	if (ret < 0) {
		return ret;
	}

	// Read where to find the fst.bin
	u32 *buffer = memalign(32, 0x20);

	if (buffer == NULL)
	{
		//Out of memory
		return -1;
	}

	ret = WDVD_Read(buffer, 0x20, 0x420);
	if (ret < 0)
		return ret;

	// Read fst.bin
	void *fstbuffer = memalign(32, buffer[2]*4);
	FST_ENTRY *fst = (FST_ENTRY *)fstbuffer;

	if (fst == NULL)
	{
		//Out of memory
		free(buffer);
		return -1;
	}

	ret = WDVD_Read(fstbuffer, buffer[2]*4, buffer[1]*4);
	if (ret < 0)
		return ret;

	free(buffer);

	// Search the fst.bin
	u32 count = fst[0].filelen;
	int i;
	u32 index = 0;

	for (i=1;i<count;i++)
	{
		if (strstr(fstfiles(fst, i), "opening.bnr") != NULL)
		{
			index = i;
		}
	}

	if (index == 0)
	{
		//opening.bnr not found
		free(fstbuffer);
		return -1;
	}

	// Load the .bnr
	u8 *banner = memalign(32, fst[index].filelen);

	if (banner == NULL)
	{
		//Out of memory
		free(fstbuffer);
		return -1;
	}

	ret = WDVD_Read((void *)banner, fst[index].filelen, fst[index].fileoffset * 4);
	if (ret < 0)
		return ret;

	FILE *fp = NULL;
	fp = fopen(dest, "wb");

	fwrite(banner, 1, fst[index].filelen, fp);

	fclose(fp);

	free(fstbuffer);
	free(banner);

	return 1;
}

