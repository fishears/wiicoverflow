#ifndef _GAMESETTINGS_H_
#define _GAMESETTINGS_H_

#include "coverflow.h"

typedef struct{

	int ocarina;
	int hooktype;
	int language;
	int video;
	int vipatch;
	char lastPlayed[256];
	
} s_gameSettings;

bool getGameSettings(char* gameCode, s_gameSettings* gs);
void setGameSettings(char* gameCode, s_gameSettings* gs);
void setLastPlayed(char* gameCode, s_gameSettings* gs);

#endif
