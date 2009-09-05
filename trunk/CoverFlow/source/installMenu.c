#include "installMenu.h"
#include "settings.h"

extern s_settings settings;
extern s_self self;
extern s_title* titleList;

extern int COVER_COUNT;
bool Menu_Install()
{

	WPAD_Rumble(0,0);
	self.rumbleAmt = 0;
    static struct discHdr headerdisc ATTRIBUTE_ALIGN(32);
	
	if(!WindowPrompt (TX.installNewGame, TX.diskInDrive, &okButton,&cancelButton))
		return false;
		
	//WindowPrompt (TX.initDVD, TX.pleaseWait,0,0);
	
	/* Disable WBFS mode */
	//Disc_SetWBFS(0, NULL);
	Disc_SetUSB(NULL);
	
    int ret, choice = 0;
	char name[MAX_TITLE_LEN];
	//static char buffer[MAX_CHARACTERS + 4];

	ret = Disc_Wait();
	if (ret < 0) {
		WindowPrompt (TX.error,TX.errorReadDisc,&cancelButton,0);
		return false;
	}
	ret = Disc_Open();
	if (ret < 0) {
		WindowPrompt (TX.error,TX.errorOpenDisc,&cancelButton,0);
		return false;
	}

	ret = Disc_IsWii();
	
	if (ret < 0) {
		choice = WindowPrompt (TX.notWiiDisc, TX.insertWiiDisc,0,&cancelButton);
/*
		
		if (!choice) {
			return false;
		}
		else
		{
			//don't like
			return Menu_Install();
		}
		*/
		return false; //
	}
	
	Disc_ReadHeader(&headerdisc);
	sprintf(name, headerdisc.title);

	if(self.usingTitlesTxt){
		char id[7];
		sprintf(id, "%s",headerdisc.id);
		getTitle(titleList, id, name);
	}
	
	
	//WindowPrompt("SUCCA", headerdisc.title, 0, &cancelButton);
	
	/*
	if (strlen(name) < (22 + 3)) {
			memset(buffer, 0, sizeof(buffer));
			sprintf(name, "%s", name);
		} else {
			strncpy(buffer, name,  MAX_CHARACTERS);
			strncat(buffer, "...", 3);
			sprintf(name, "%s", buffer);
	}
	*/
	
	ret = WBFS_CheckGame(headerdisc.id);
	if (ret) {
		WindowPrompt (TX.alreadyInstalled, name, &cancelButton,0);
		return false;
	}
	self.hdd = GetHddInfo();
	if (!self.hdd) {
		WindowPrompt (TX.error, TX.noHDD, &cancelButton,0);
		return false;
		}

	f32 freespace, used;

	WBFS_DiskSpace(&used, &freespace);
	u64 estimation = wbfs_estimate_disc(self.hdd, __WBFS_ReadDVD, NULL, ONLY_GAME_PARTITION);
	f32 gamesize = ((f32) estimation)/1073741824;
	char gametxt[50];
	
	sprintf(gametxt, TX.installingGame, gamesize);
	
	//char ttext[50];
	char tsize[50];
	//sprintf(ttext, "Installing game: %s", name); // FIX M074
	sprintf(tsize, TX.gameSize, gamesize);
	
	//if(WindowPrompt(headerdisc.id, ttext,tsize,&okButton,&cancelButton))
	if(WindowPromptInstall((char*)headerdisc.id, name, tsize,&okButton,&cancelButton,0))
	{
		if (gamesize > freespace) {
			char errortxt[50];
			sprintf(errortxt, TX.gameSizeSpace, gamesize, freespace);
			WindowPrompt(TX.noFreeSpace, errortxt, &cancelButton, 0);
			return false;
		}
		else {
			BUFFER_KillBuffer();
			ret = ProgressWindow(self.hdd, gametxt, name);
			if (ret != 0) {
				InitializeBuffer(self.gameList,self.gameCnt,BUFFER_WINDOW,COVER_COUNT/2.0 +self.shift,settings.covers3d);
				BUFFER_InitBuffer();
				WindowPrompt (TX.error, TX.errorInstall, &cancelButton, 0);
				return false;
			} else {
				GetEntries();
				InitializeBuffer(self.gameList,self.gameCnt,BUFFER_WINDOW,COVER_COUNT/2.0 +self.shift,settings.covers3d);
				BUFFER_InitBuffer();
				
				WiiLight(1);
				WindowPromptInstall((char*)headerdisc.id, name, TX.successInstall, &okButton,0, 0);
				WBFS_DiskSpace(&self.usedSpace, &self.freeSpace);
				//WindowPrompt (TX.successInstall, name,&okButton,0);
				WiiLight(0);
				return true;
			}
		}
	}
	
	return false;
}
