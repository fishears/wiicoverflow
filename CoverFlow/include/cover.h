#ifndef _COVER_H_
#define _COVER_H

#include "coverflow.h"

void LoadCurrentCover(int id, struct discHdr *gameList);
void AddCover(GRRLIB_texImg tex);
void Init_Covers();
void Download_Cover(struct discHdr *gameList, struct discHdr *header, int v, int max);
void batchDownloadCover(struct discHdr *gameList);

#endif
