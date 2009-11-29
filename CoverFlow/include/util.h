#ifndef _UTIL_H
#define _UTIL_H

#include <stdlib.h> // bool...
#include <gctypes.h> // bool...

typedef struct SoundInfo
{
	void *dsp_data;
	int size;
	int channels;
	int rate;
	int loop;
} SoundInfo;

void parse_banner_snd(void *data_hdr, SoundInfo *snd);

#endif

