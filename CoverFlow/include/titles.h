#ifndef __TITLES__
#define __TITLES__

#include "coverflow.h"

#define MAX_TITLE_LEN 256

typedef struct{
	char id[5];
    char name[MAX_TITLE_LEN];
} s_title;

int initTitle();
int getNumLines(FILE* fp);
void fillTitleStruct(s_title* titleList, int len);
void getTitle(s_title* titleList, char* id, char* title);

#endif
