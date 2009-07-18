#ifndef __NEWSREADER_H__
#define __NEWSREADER_H__

#include "coverflow.h"


void showNewsWindow();
bool newsFileExist();
void getNewsFormatData();
void setNewsFormatData(char *name, char *val);
bool getNewsID(char *value);
bool blinking(int Aon, int Aoff, int Bon, int Boff, int Con, int Coff);
void setNewsDate(char* ret_string);
#endif
