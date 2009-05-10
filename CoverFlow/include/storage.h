#ifndef _STORAGE_H_
#define _STORAGE_H

#include "coverflow.h"

bool init_usbfs();
bool reinit_usbfs();
void saveFile(char* imgPath, struct block file);
void checkDirs();
void checkFiles();

#endif
