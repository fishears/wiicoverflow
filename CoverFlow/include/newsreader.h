#ifndef __NEWSREADER_H__
#define __NEWSREADER_H__

#include "coverflow.h"


void showNewsWindow();
bool newsFileExist();
void getNewsFormatData();
void setNewsFormatData(char *name, char *val);
bool getNewsID(char *value);

#endif
