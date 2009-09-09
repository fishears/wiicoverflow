#ifndef _STORAGE_H_
#define _STORAGE_H

#include "coverflow.h"

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

#endif
