#include "installMenu.h"

extern s_self self;

bool Menu_Install(){

	WPAD_Rumble(0,0);
	self.rumbleAmt = 0;
	okButton.y = 290;
	cancelButton.y = 290;
    static struct discHdr headerdisc ATTRIBUTE_ALIGN(32);
	
	if(!WindowPrompt (localStr("M064", "Install new Game?"), localStr("M065", "Place disk in drive and hit ok."),&okButton,&cancelButton))
		return false;
		
	WindowPrompt (localStr("M066", "Initializing DVD Drive") , localStr("M067", "Please Wait..."),0,0);
	
	/* Disable WBFS mode */
	Disc_SetWBFS(0, NULL);
	
    int ret, choice = 0;
	char *name;
	static char buffer[MAX_CHARACTERS + 4];

	ret = Disc_Wait();
	if (ret < 0) {
		WindowPrompt (localStr("M068", "Error reading Disc"),0,&cancelButton,0);
		return false;
	}
	ret = Disc_Open();
	if (ret < 0) {
		WindowPrompt (localStr("M069", "Could not open Disc"),0,&cancelButton,0);
		return false;
	}

	ret = Disc_IsWii();
	
	if (ret < 0) {
		choice = WindowPrompt (localStr("M070", "Not a Wii Disc"), localStr("M071", "Insert a Wii Disc!"),&okButton,&cancelButton);

		if (!choice) {
			return false;
		}
		else
		{
			return Menu_Install();
		}
	}
	
	Disc_ReadHeader(&headerdisc);
	name = headerdisc.title;
	if (strlen(name) < (22 + 3)) {
			memset(buffer, 0, sizeof(buffer));
			sprintf(name, "%s", name);
		} else {
			strncpy(buffer, name,  MAX_CHARACTERS);
			strncat(buffer, "...", 3);
			sprintf(name, "%s", buffer);
	}

	ret = WBFS_CheckGame(headerdisc.id);
	if (ret) {
		WindowPrompt (localStr("M072", "Game is already installed:"),name,&cancelButton,0);
		return false;
	}
	self.hdd = GetHddInfo();
	if (!self.hdd) {
		WindowPrompt (localStr("M073", "No HDD found!"), localStr("M003", "Error!!"),&cancelButton,0);
		return false;
		}

	f32 freespace, used;

	WBFS_DiskSpace(&used, &freespace);
	u32 estimation = wbfs_estimate_disc(self.hdd, __WBFS_ReadDVD, NULL, ONLY_GAME_PARTITION);
	f32 gamesize = ((f32) estimation)/1073741824;
	char gametxt[50];
	
	sprintf(gametxt, localStr("M074", "Installing game %.2fGB:"), gamesize);
	
	char ttext[50];
	char tsize[50];
	sprintf(ttext, localStr("M075", "Install %s?"), name);
	sprintf(tsize, localStr("M076", "Game Size: %.2fGB"), gamesize);
	
	if(WindowPrompt (ttext,tsize,&okButton,&cancelButton))
	{
		if (gamesize > freespace) {
			char errortxt[50];
			sprintf(errortxt, localStr("M077", "Game Size: %.2fGB, Free Space: %.2fGB"), gamesize, freespace);
			WindowPrompt(localStr("M133", "Not enough free space!"),errortxt,&cancelButton, 0);
			return false;
		}
		else {
			ret = ProgressWindow(self.hdd, gametxt, name);
			if (ret != 0) {
				WindowPrompt (localStr("M078", "Install error!"),0,&cancelButton,0);
				return false;
			} else {
				InitializeBuffer(self.gameList, self.gameCnt,BUFFER_WINDOW,COVER_COUNT/2.0 +self.shift);
				Sleep(300);
				GetEntries();
				UpdateBufferedImages();
				Sleep(100);
				
				WindowPrompt (localStr("M079", "Successfully installed:"),name,&okButton,0);
				return true;
			}
		}
	}
	
	return false;
}
