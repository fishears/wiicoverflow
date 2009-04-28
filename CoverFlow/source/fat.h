#ifndef _FAT_H_
#define _FAT_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Prototypes */
s32 Fat_MountSDHC(void);
s32 Fat_UnmountSDHC(void);
s32 Fat_ReadFile(const char *, void **);

#ifdef __cplusplus
}
#endif

#endif
