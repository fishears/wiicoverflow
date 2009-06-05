#include "deleteMenu.h"

extern s_self self;
extern s_title* titleList;
extern int COVER_COUNT;

bool Menu_Delete(){

	WPAD_Rumble(0,0);
	self.rumbleAmt = 0;
	
	struct discHdr *header = NULL;
 	char gameName[31]; 
	
	/* No game list */
	if (!self.gameCnt)
		return false;

	/* Selected game */
	header = &self.gameList[self.gameSelected];
	char title[MAX_TITLE_LEN];

	if(self.usingTitlesTxt){
		sprintf(title, "%s", header->title);
		getTitle(titleList, (char*)header->id, title);
	}
	else
		sprintf(title, "%s", (header->title));

	if(strlen(title) < 30) {
		sprintf(gameName, "%s", title);
	}
	else
	{
		strncpy(gameName, title, 27);
		gameName[27] = '\0';
		strncat(gameName, "...", 3);
	}

	if(WindowPrompt(TX.askDelete, gameName, &yesButton, &noButton))
	{
		if(0 > WBFS_RemoveGame(header->id))
		{
			WindowPrompt(TX.errorDelete, TX.cantDelete, &okButton, 0);
		}
		else
		{
			GetEntries();
			Sleep(300);
			BUFFER_KillBuffer();
			InitializeBuffer(self.gameList,self.gameCnt,BUFFER_WINDOW,COVER_COUNT/2.0 +self.shift);
			BUFFER_InitBuffer();
			Sleep(100);
			
			WindowPrompt(TX.successDelete, TX.pressOkContinue, &okButton, 0);
			return true;
		}
	}
	
	return false;
}
