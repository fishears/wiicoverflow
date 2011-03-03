#ifndef _UTIL_H
#define _UTIL_H

#include <stdlib.h> // bool...
#include <gctypes.h> // bool...

#define STRCOPY(DEST,SRC) strcopy(DEST,SRC,sizeof(DEST))

#define STRAPPEND(DST,SRC) strappend(DST,SRC,sizeof(DST))
char *strappend(char *dest, char *src, int size);

bool str_replace(char *str, char *olds, char *news, int size);
bool str_replace_all(char *str, char *olds, char *news, int size);
bool trimsplit(char *line, char *part1, char *part2, char delim, int size);
char* split_tokens(char *dest, char *src, char *delim, int size);

typedef struct SoundInfo
{
	void *dsp_data;
	int size;
	int channels;
	int rate;
	int loop;
} SoundInfo;

void parse_banner_snd(void *data_hdr, SoundInfo *snd);

static inline u32 _be32(const u8 *p)
{
	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

static inline u32 _le32(const void *d)
{
	const u8 *p = d;
	return (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
}

static inline u32 _le16(const void *d)
{
	const u8 *p = d;
	return (p[1] << 8) | p[0];
}
#endif

