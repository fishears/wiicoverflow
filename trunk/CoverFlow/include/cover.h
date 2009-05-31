#ifndef _COVER_H_
#define _COVER_H

#include "coverflow.h"

void LoadCurrentCover(int id, struct discHdr *gameList);
void AddCover(GRRLIB_texImg tex);
void Init_Covers();
void Download_Cover(char* id, int v, int max);
void batchDownloadCover(struct discHdr *gameList);
bool coverLoaded;
#endif
