#ifndef _GAMESETTINGS_H_
#define _GAMESETTINGS_H_

#include "coverflow.h"

typedef struct{

	int ocarina;
	int hooktype;
	int language;
	int video;
	int vipatch;
	char fuffa[256];
	
} s_gameSettings;

void getSettings(char* gameCode, s_gameSettings* gs);
void setGameSettings(char* gameCode, s_gameSettings* gs);

#endif
