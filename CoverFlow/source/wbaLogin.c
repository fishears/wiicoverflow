/*
 *  wbaLogin.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/
#include "wbaLogin.h"
#include "fileParser.h"

#ifdef WBA_BY_FILE
extern s_self self;


bool wbaFileExist()
{
  	FILE *fp;
	fp = fopen(WIIBOXART_PASS, "r");
 	if(fp != NULL)
	{
		fclose(fp);
		return true;
	}
	else
		return false;
}



void getWBA_LoginData()
{
 cfg_parsefile(WIIBOXART_PASS, &storeWBA_LoginData);
}


void storeWBA_LoginData(char *name, char *val)
{
	if (strcmp(name, "username") == 0) {
		strcopy(self.wba_username, val, sizeof(self.wba_username));
		return;
	}

	if (strcmp(name, "password") == 0) {
		strcopy(self.wba_password, val, sizeof(self.wba_password));
		return;
	}
}

#endif





