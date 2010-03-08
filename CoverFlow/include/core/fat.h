#ifndef _FAT_H_
#define _FAT_H_
#include "fat.h"
/* Prototypes */
s32 Fat_ReadFile(const char *, void **);
s32 Fat_ReadFileToBuffer(const char *filepath, void *outbuf, int maxsize);

#endif
