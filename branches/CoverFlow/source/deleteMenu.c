#include "deleteMenu.h"

extern s_self self;

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

	if(strlen(header->title) < 30) {
		sprintf(gameName, "%s", header->title);
	}
	else
	{
		strncpy(gameName, header->title, 27);
		gameName[27] = '\0';
		strncat(gameName, "...", 3);
	}

	if(WindowPrompt(localStr("M080", "Do you want to delete:"), gameName, &yesButton, &noButton))
	{
		if(0 > WBFS_RemoveGame(header->id))
		{
			WindowPrompt(localStr("M081", "Delete Failed."), localStr("M082", "Could not delete game."), &okButton, 0);
		}
		else
		{
			GetEntries();
			Sleep(300);
			InitializeBuffer(self.gameList,self.gameCnt,BUFFER_WINDOW,COVER_COUNT/2.0 +self.shift);
			Sleep(100);
			
			WindowPrompt(localStr("M083", "Successfully deleted."), localStr("M084", "Press Ok to Continue."), &okButton, 0);
			return true;
		}
	}
	
	return false;
}
