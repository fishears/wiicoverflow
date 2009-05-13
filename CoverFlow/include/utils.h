#ifndef _UTILS_H_
#define _UTILS_H_

#include "coverflow.h"

/* Constants */
#define KB_SIZE         1024.0
#define MB_SIZE         1048576.0
#define GB_SIZE         1073741824.0

#define REQUIRED_IOS_REV 10 //this is the minimum cIOS Revision required for full loader support

/* Macros */
#define round_up(x,n)   (-(-(x) & -(n)))

/* Prototypes */
u32 swap32(u32);
void sysdate();
void ios_version_check();
u8 CalculateFrameRate();
void apply_settings();
int Net_Init(char *ip);

#endif
