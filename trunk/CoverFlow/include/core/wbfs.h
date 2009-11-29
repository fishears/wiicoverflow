#ifndef _WBFS_H_
#define _WBFS_H_

#include "libwbfs/libwbfs.h"
#include "util.h"
 
/* Device list */
enum {
	WBFS_DEVICE_USB = 1,	/* USB device */
	WBFS_DEVICE_SDHC	/* SDHC device */
};

/* Macros */
#define WBFS_MIN_DEVICE		1
#define WBFS_MAX_DEVICE		2

#define DOL_LIST_MAX 64
typedef struct DOL_LIST
{
	char name[DOL_LIST_MAX][64];
	int num;
} DOL_LIST;

int WBFS_GetDolList(u8 *discid, DOL_LIST *list);

/* Prototypes */
s32 WBFS_Init(u32);
s32 WBFS_Open(void);
s32 WBFS_Close(void);
s32 WBFS_Format(u32, u32);
s32 WBFS_GetCount(u32 *);
s32 WBFS_GetHeaders(void *, u32, u32);
s32 WBFS_CheckGame(u8 *);
s32 __WBFS_ReadDVD(void *fp, u32 lba, u32 len, void *iobuf);
wbfs_t *GetHddInfo(void);
s32 WBFS_AddGame(void);
s32 WBFS_RemoveGame(u8 *);
s32 WBFS_GameSize(u8 *, f32 *);
s32 WBFS_DiskSpace(f32 *, f32 *);

s32 WBFS_RenameGame(u8 *, const void *);
s32 WBFS_ReIDGame(u8 *discid, const void *newID);
f32 WBFS_EstimeGameSize(void);

s32 __WBFS_ReadUSB(void *fp, u32 lba, u32 count, void *iobuf);
s32 __WBFS_WriteUSB(void *fp, u32 lba, u32 count, void *iobuf);

int WBFS_BannerSound(u8 *discid, SoundInfo *snd);

/*
#define DOL_LIST_MAX 64
typedef struct DOL_LIST
{
	char name[DOL_LIST_MAX][64];
	int num;
} DOL_LIST;

typedef struct SoundInfo
{
	void *dsp_data;
	int size;
	int channels;
	int rate;
	int loop;
} SoundInfo;

*/

#endif
