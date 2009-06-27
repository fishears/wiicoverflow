#include "cover.h"

extern s_self self;
extern s_settings settings;
extern s_gameSettings gameSetting;
extern int COVER_COUNT;

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
	
	if (coverLoaded)
	{
		free(current_cover_texture.data);  //should be freed if loading another
		coverLoaded=false;
	}
	
	if (ret > 0)
	{
		current_cover_texture = GRRLIB_LoadTexture((const unsigned char*)imgData);
		coverLoaded=true;
		free(imgData);
	}
	else
	{
		sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c.png", header->id[0],header->id[1],header->id[2]);
		ret = Fat_ReadFile(filepath, &imgData);
		
		if (ret > 0)
		{
			current_cover_texture = GRRLIB_LoadTexture((const unsigned char*)imgData);
			coverLoaded=true;
			free(imgData);
		}
		else
		{
			current_cover_texture = no_disc_texture;
			coverLoaded=false;
		}
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
	coverLoaded=false;
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

void Download_Cover(char* id, int v, int max)
{
	char imgPath[100];

	if (!id)
		return;

	/*
	if(self.firstTimeDownload == true){
		
		char myIP[16];
		
		sprintf(self.debugMsg, TX.iniNet );
		Paint_Progress_Generic(v, max, self.debugMsg);
		
		if(!Net_Init(myIP))
		{
			sprintf(self.debugMsg, TX.iniNetErr );
			Paint_Progress_Generic(v, max, self.debugMsg);
			WindowPrompt (TX.error, TX.iniNetErr, &okButton, 0);
		}
		else
		{
			sprintf(self.debugMsg, TX.iniNetReady );
			Paint_Progress_Generic(v, max, self.debugMsg);
			self.inetOk = true;
		}
		
		self.firstTimeDownload = false;
	}
	*/
	//if(self.inetOk) {
		//printf("\n    Network connection established.");
		/*try to download image */
			
		char url[100];
		struct block file;
	
		char region[4];
		switch(id[3]){
	
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

		if(!(settings.covers3d))
		{
			snprintf(imgPath, sizeof(imgPath), "%s/covers/%s.png", USBLOADER_PATH, id);
		} 
		else
		{
			snprintf(imgPath, sizeof(imgPath), "%s/3dcovers/%c%c%c%c.png", USBLOADER_PATH, id[0], id[1], id[2], id[3]);
		}
		
		sprintf(self.debugMsg, TX.checkPresence, imgPath);
		Paint_Progress_Generic(v, max,self.debugMsg);
		
		FILE *fp=NULL;
		if (!BUFFER_IsCoverMissing(v)) // don't try and read the file if the cover is bad
		{
			fp = fopen(imgPath, "rb");
		}
		if (fp)
		{
			fclose (fp);
			sprintf(self.debugMsg,  TX.noDownload, imgPath);
			Paint_Progress_Generic(v, max,self.debugMsg);
		}
		else{
#ifdef WBA_BY_FILE
			if (self.wba_fileexist)
				{
				if(!(settings.covers3d))
					{
					sprintf(url, "http://www.wiiboxart.com/beardface/%s/%s/%s.png", self.wba_username, self.wba_password, id);
					}
				else
					{
					sprintf(url, "http://www.wiiboxart.com/hibern/%s/%s/%c%c%c%c.png", self.wba_username, self.wba_password, id[0], id[1], id[2], id[3]);
					}
				}
#else
			if(!(settings.covers3d))
			{
				sprintf(url, "http://www.wiiboxart.com/resize/%s/160/224/%s.png", region, id);
			}
			else
			{
				sprintf(url, "http://www.wiiboxart.com/fullcover/%c%c%c%c.png", id[0], id[1], id[2], id[3]);
			}
#endif			
			sprintf(self.debugMsg, TX.getting, url);
			Paint_Progress_Generic(v, max,self.debugMsg);
		
			file = downloadfile(url);
			
			if(file.data != NULL && file.size != 7386 && file.size != 36864 && file.size >= 1024){ //site return a black image of 184 bytes if the image is not found
				saveFile(imgPath, file);
				free(file.data);
				sprintf(self.debugMsg, TX.done );
			    Paint_Progress_Generic(v, max,self.debugMsg);
			}
			else {
				if (file.data != NULL) free(file.data);
				sprintf(self.debugMsg, TX.someErrors );
				Paint_Progress_Generic(v, max,self.debugMsg);
			}
		}
		
		snprintf(imgPath, sizeof(imgPath), "%s/disks/%c%c%c%c.png", USBLOADER_PATH,  id[0], id[1], id[2], id[3]);
		sprintf(self.debugMsg, TX.checkPresence, imgPath);
		Paint_Progress_Generic(v, max,self.debugMsg);
		
		fp = fopen(imgPath, "rb");
		if (fp)
		{
			fclose (fp);
			sprintf(self.debugMsg, TX.noDownload, imgPath);
			Paint_Progress_Generic(v, max,self.debugMsg);
		}
		else{
		
#ifdef WBA_BY_FILE		
			if (self.wba_fileexist)
				{
				sprintf(url, "http://www.wiiboxart.com/diskartloader/160/160/%s/%s/%c%c%c%c.png", self.wba_username, self.wba_password, id[0], id[1], id[2], id[3]);
				}
#else
			sprintf(url, "http://www.wiiboxart.com/diskart/160/160/%c%c%c%c.png", id[0], id[1], id[2], id[3]);
#endif	
			sprintf(self.debugMsg, TX.getting, url);
			Paint_Progress_Generic(v, max,self.debugMsg);
			
			file = downloadfile(url);
			
			if(file.data != NULL && file.size != 7386 && file.size != 36864 && file.size >= 1024){ //site return a generic of 7386 bytes image is not found
				saveFile(imgPath, file);
				free(file.data);
				sprintf(self.debugMsg, TX.done );
			    Paint_Progress_Generic(v, max,self.debugMsg);
			}
			else { //TRY WITH 3 DIGIT COVER
				if (file.data != NULL) free(file.data);
				
				snprintf(imgPath, sizeof(imgPath), "%s/disks/%c%c%c.png", USBLOADER_PATH,  id[0], id[1], id[2]);
#ifdef WBA_BY_FILE	
				if (self.wba_fileexist)
					{
					sprintf(url, "http://www.wiiboxart.com/diskartloader/160/160/%s/%s/%c%c%c.png", self.wba_username, self.wba_password, id[0], id[1], id[2]);
					}
#else
				sprintf(url, "http://www.wiiboxart.com/diskart/160/160/%c%c%c.png", id[0], id[1], id[2]);
#endif
				sprintf(self.debugMsg, TX.getting, url);
				Paint_Progress_Generic(v, max,self.debugMsg);
				
				file = downloadfile(url);
			
				if(file.data != NULL && file.size != 7386 && file.size != 36864 && file.size >= 1024){ //site return a generic of 7386 bytes image is not found
					saveFile(imgPath, file);
					free(file.data);
					sprintf(self.debugMsg, TX.done );
					Paint_Progress_Generic(v, max,self.debugMsg);
				}
				else {
					if (file.data != NULL) free(file.data);
					sprintf(self.debugMsg, TX.someErrors );
					Paint_Progress_Generic(v, max,self.debugMsg);
				}
			}
		//else
			//donotdownload = true;
		}
	//}
	//refresh = true;				
} /* end download */


void batchDownloadCover(struct discHdr *gameList)
{
	int i;
	char id[7];
	
	for(i = 0; i < self.gameCnt; i++)
	{
		struct discHdr *header = &gameList[i];
		
		if(self.array_size < MAX_COVERS)
		{
			sprintf(id, "%s", header->id);
			sprintf(self.debugMsg, TX.checkNextCover, header->id);
			Paint_Progress_Generic(i, self.gameCnt, self.debugMsg);
			Download_Cover(id, i, self.gameCnt);
			//sprintf(filepath, USBLOADER_PATH "/covers/%s.png", header->id);
		}
	}
	WindowPrompt (TX.opFinished, TX.pressA, &okButton, 0);
}

