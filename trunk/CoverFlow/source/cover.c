#include "cover.h"

extern s_self self;
extern s_gameSettings gameSetting;

void LoadCurrentCover(int id, struct discHdr *gameList)
{
	#ifndef TEST_MODE
	void *imgData;// = (void *)no_cover_png;

	char filepath[128];
	char titleID[7];

	s32  ret;
	struct discHdr *header = &gameList[id];

	sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c%c.png", header->id[0],header->id[1],header->id[2],header->id[3]);

	ret = Fat_ReadFile(filepath, &imgData);
	
	sprintf(titleID, "%s", header->id);
	getGameSettings(titleID, &gameSetting);
	
	if (ret > 0)
		current_cover_texture = GRRLIB_LoadTexture((const unsigned char*)imgData);
	else{
		sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c.png", header->id[0],header->id[1],header->id[2]);
		ret = Fat_ReadFile(filepath, &imgData);
		
		if (ret > 0)
			current_cover_texture = GRRLIB_LoadTexture((const unsigned char*)imgData);
		else
			current_cover_texture = no_disc_texture;
	}
	
	#else
	current_cover_texture = no_disc_texture;
	#endif
}

void AddCover(GRRLIB_texImg tex)
{
	if(self.array_size < MAX_COVERS)
	{
		covers[self.array_size] = tex;
		self.array_size++;
	}
}



void Init_Covers()
{
	#ifdef TEST_MODE
	self.progress+=0.05;
	Paint_Progress(self.progress, "Initializing Covers...");
	
	int i;
	int CoverCount = COVER_COUNT;
	float max_progress = 1.7;
	float per_game_prog = max_progress/self.gameCnt;
	
	for(i = 0; i < CoverCount; i++)
	{
		AddCover( GRRLIB_LoadTexture(no_cover_png) );
		self.progress+=per_game_prog;
		Paint_Progress(self.progress, "Initializing Covers...");
	}
	
	#endif
}

void Download_Cover(struct discHdr *gameList, struct discHdr *header, int v, int max)
{
	char imgPath[100];

	if (!header)
		return;

	if(self.firstTimeDownload == true){
		
		char myIP[16];
		
		sprintf(self.debugMsg, localStr("M001", "Initializing Network") );
		Paint_Progress_Generic(v, max, self.debugMsg);
		
		if(!Net_Init(myIP))
		{
			sprintf(self.debugMsg, localStr("M002", "Error Initializing Network") );
			Paint_Progress_Generic(v, max, self.debugMsg);
			WindowPrompt (localStr("M003", "ERROR!"), localStr("M002", "Error initializing network"), &okButton, 0);
		}
		else
		{
			sprintf(self.debugMsg, localStr("M004", "Network Initialized") );
			Paint_Progress_Generic(v, max, self.debugMsg);
			self.inetOk = true;
		}

		self.firstTimeDownload = false;
	}

	if(self.inetOk) {
		//printf("\n    Network connection established.");
		/*try to download image */
			
		char url[100];
		struct block file;
	
		char region[4];
		switch(header->id[3]){
	
		case 'E':
			sprintf(region,"ntsc");
			break;

		case 'J':
			sprintf(region,"ntscj");
			break;

		case 'P':
			sprintf(region,"pal");
			break;
		}

		snprintf(imgPath, sizeof(imgPath), "%s/covers/%s.png", USBLOADER_PATH, header->id);
		
		sprintf(self.debugMsg, localStr("M005", "Checking presence of %s"), imgPath);
		Paint_Progress_Generic(v, max,self.debugMsg);
		
		FILE *fp;
		fp = fopen(imgPath, "rb");
		if (fp)
		{
			fclose (fp);
			sprintf(self.debugMsg,  localStr("M006", "%s present, not downloading"), imgPath);
			Paint_Progress_Generic(v, max,self.debugMsg);
		}
		else{
			/*
			if (CFG.widescreen)
				sprintf(url, "http://www.theotherzone.com/wii/widescreen/%s/%s.png", region, header->id);
			else
				sprintf(url, "http://www.theotherzone.com/wii/%s/%s.png", region, header->id);
			*/
			
			sprintf(url, "http://www.theotherzone.com/wii/resize/%s/160/224/%s.png", region, header->id);
			sprintf(self.debugMsg, localStr("M007", "Getting %s"), url);
			Paint_Progress_Generic(v, max,self.debugMsg);
		
			file = downloadfile(url);
			
			if(file.data != NULL){
				saveFile(imgPath, file);
				free(file.data);
				sprintf(self.debugMsg, localStr("M008", "done") );
			    Paint_Progress_Generic(v, max,self.debugMsg);
				//else
					//donotdownload = true;
			}
			else {
				sprintf(self.debugMsg, localStr("M009", "some error occurred") );
				Paint_Progress_Generic(v, max,self.debugMsg);
			}
		}
		
		snprintf(imgPath, sizeof(imgPath), "%s/disks/%c%c%c%c.png", USBLOADER_PATH,  header->id[0], header->id[1], header->id[2], header->id[3]);
		sprintf(self.debugMsg, localStr("M005", "Checking presence of %s"), imgPath);
		Paint_Progress_Generic(v, max,self.debugMsg);
		
		fp = fopen(imgPath, "rb");
		if (fp)
		{
			fclose (fp);
			sprintf(self.debugMsg,localStr("M006", "%s present, not downloading"), imgPath);
			Paint_Progress_Generic(v, max,self.debugMsg);
		}
		else{
			sprintf(url, "http://www.theotherzone.com/wii/diskart/160/160/%c%c%c%c.png", header->id[0], header->id[1], header->id[2], header->id[3]);
			sprintf(self.debugMsg, localStr("M007", "Getting %s"), url);
			Paint_Progress_Generic(v, max,self.debugMsg);
			
			file = downloadfile(url);
			
			if(file.data != NULL){
				saveFile(imgPath, file);
				free(file.data);
				sprintf(self.debugMsg, localStr("M008", "done") );
			    Paint_Progress_Generic(v, max,self.debugMsg);
				//else
					//donotdownload = true;
			}
			else { //TRY WITH 3 DIGIT COVER
				
				snprintf(imgPath, sizeof(imgPath), "%s/disks/%c%c%c.png", USBLOADER_PATH,  header->id[0], header->id[1], header->id[2]);
				sprintf(url, "http://www.theotherzone.com/wii/diskart/160/160/%c%c%c.png", header->id[0], header->id[1], header->id[2]);
				sprintf(self.debugMsg, localStr("M007", "Getting %s"), url);
				Paint_Progress_Generic(v, max,self.debugMsg);
				
				file = downloadfile(url);
			
				if(file.data != NULL){
					saveFile(imgPath, file);
					free(file.data);
					sprintf(self.debugMsg, localStr("M008", "done") );
					Paint_Progress_Generic(v, max,self.debugMsg);
				}
				else {
					sprintf(self.debugMsg, localStr("M009", "some error occurred") );
					Paint_Progress_Generic(v, max,self.debugMsg);
				}
			}
		//else
			//donotdownload = true;
		}
	}
	//refresh = true;				
} /* end download */


void batchDownloadCover(struct discHdr *gameList)
{
	int i;
	
	for(i = 0; i < self.gameCnt; i++)
	{
		struct discHdr *header = &gameList[i];
		
		if(self.array_size < MAX_COVERS)
		{
			sprintf(self.debugMsg, localStr("M010", "Checking next cover...%s"), header->id);
			Paint_Progress_Generic(i, self.gameCnt, self.debugMsg);
			Download_Cover(gameList, header, i, self.gameCnt);
			//sprintf(filepath, USBLOADER_PATH "/covers/%s.png", header->id);
		}
	}
//	BUFFER_ClearCovers();
	Sleep(300);
//	UpdateBufferedImages();
	Sleep(100);
	WindowPrompt (localStr("M011", "Operation finished!"), localStr("M012", "Press A to continue"), &okButton, 0);
}

