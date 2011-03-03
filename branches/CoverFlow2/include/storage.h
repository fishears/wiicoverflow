#ifndef _STORAGE_H_
#define _STORAGE_H

#include "coverflow.h"


extern s32 all_gameCnt;
extern s32 gameCnt;
bool init_usbfs();
bool reinit_usbfs();
bool saveFile(char* imgPath, struct block file);
void checkDirs();
void checkFiles();
void initWBFS();
s32 __Menu_EntryCmp(const void *a, const void *b);
s32 GetEntries();
s32 GetEntriesSilent();
bool Init_Game_List();
void initUSBFS();
bool check_write_access();
int filterFavorites(struct discHdr *list, int cnt);
bool isFavorite(u8 *id);
const char *get_dev_name(int dev);
void Menu_Partition(bool must_select);
#endif
