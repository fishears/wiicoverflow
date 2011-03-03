#include "cover.h"
#include "utils.h"
#include "TrackedMemoryManager.h"

#define SITE_BASE "http://wiitdb.com/wiitdb/artwork/"
#define ALT_SITE_BASE "http://www.muntrue.nl/covers/ALL/"

#define COVER "cover"
#define COVERFULL "coverfull"
#define COVERFULLHQ "coverfullHQ"
#define DISK "disc"
#define CUSTOMDISK "disccustom"
#define ALT_COVER "160/224/boxart"
#define ALT_COVERFULL "512/340/fullcover"
#define ALT_DISK "160/160/disc"

#define PARAMTERISED_ARTWORK_LOCATION "/[Lang]/[GameID].png"
#define ALT_PARAMTERISED_ARTWORK_LOCATION "/[GameID].png"

extern s_self self;
extern s_settings settings;
extern s_gameSettings gameSetting;
extern int COVER_COUNT;
extern s_path dynPath;

void LoadCurrentCover(int id, struct discHdr *gameList)
{
	#ifndef TEST_MODE
	void *imgData;// = (void *)no_cover_png;

	char filepath[128];
	char titleID[7];

	s32  ret;
	struct discHdr *header = &gameList[id];

	//sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c%c%c%c.png", header->id[0],header->id[1],header->id[2],header->id[3],header->id[4],header->id[5]);
	sprintf(filepath, "%s/%c%c%c%c%c%c.png", dynPath.dir_disks, header->id[0],header->id[1],header->id[2],header->id[3],header->id[4],header->id[5]);

	ret = Fat_ReadFile(filepath, &imgData);
	
	sprintf(titleID, "%s", header->id);
	getGameSettings(titleID, &gameSetting);
	
	if (ret > 0)
	{
		BufferImageToSlot(&current_cover_texture,(const unsigned char*)imgData,10);

		CFFree(imgData);
	}
	else
	{
		//sprintf(filepath, USBLOADER_PATH "/disks/%c%c%c%c%c%c.png", header->id[0],header->id[1],header->id[2], header->id[3],header->id[4],header->id[5]);
		sprintf(filepath, "%s/%c%c%c%c%c%c.png", dynPath.dir_disks, header->id[0],header->id[1],header->id[2], header->id[3],header->id[4],header->id[5]);

		ret = Fat_ReadFile(filepath, &imgData);
		
		if (ret > 0)
		{
			BufferImageToSlot(&current_cover_texture,(const unsigned char*)imgData,10);
			CFFree(imgData);
		}
		else
		{
			BufferImageToSlot(&current_cover_texture,no_disc_png,10);
		}
	}
	
	#else
	BufferImageToSlot(&current_cover_texture,no_disc_png,10);
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
		AddCover( cover_texture );
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
		
	//struct block file;

	char region[5];
	
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
	
	char country[3];
	
	switch(id[3]){
		case 'J':
			sprintf(country, "JA");
			break;
		case 'K':
			sprintf(country, "KO");
			break;
		case 'P':
		case 'D':
		case 'F':
		case 'I':
		case 'S':
		case 'H':
		case 'X':
		case 'Y':
			switch(CONF_GetLanguage()){
				case CONF_LANG_ENGLISH:
					sprintf(country, "EN");
					break;
				case CONF_LANG_GERMAN:
					sprintf(country, "DE");
					break;
				case CONF_LANG_FRENCH:
					sprintf(country, "FR");
					break;		
				case CONF_LANG_SPANISH:
					sprintf(country, "ES");
					break;
				case CONF_LANG_ITALIAN:
					sprintf(country, "IT");
					break;
				case CONF_LANG_DUTCH:
					sprintf(country, "NL");
					break;
				default:
					sprintf(country, "EN");
					break;
			}
			break;
		case 'E':
			sprintf(country, "US");
			break;
		default:
			sprintf(country, "EN");
	}

	if(!(settings.covers3d))
		snprintf(imgPath, sizeof(imgPath), "%s/%s.png", dynPath.dir_covers, id);
	else if(settings.covers3d && settings.hq==1)
	    snprintf(imgPath, sizeof(imgPath), "%s/%s.png", dynPath.dir_HQcovers, id);
	else
	    snprintf(imgPath, sizeof(imgPath), "%s/%s.png", dynPath.dir_3dcovers, id);
	sprintf(self.debugMsg, TX.checkPresence, imgPath);
	Paint_Progress_Generic(v, max, self.debugMsg);
	
	//
	// COVER
	//
	
	FILE *fp=NULL;
	if (!BUFFER_IsCoverMissing(v)) // don't try and read the file if the cover is bad
		fp = fopen(imgPath, "rb");
		char * parsedUrl;
		
	if (fp)
	{
		fclose (fp);
		sprintf(self.debugMsg, TX.noDownload, imgPath);
		Paint_Progress_Generic(v, max, self.debugMsg);
	}
		
	else{ // cover not found, we try to download it

		if(!(settings.covers3d)) //2d mode
		{
			parsedUrl=ParseTokenedUrl(SITE_BASE,COVER,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
		}
                else if(settings.covers3d && settings.hq==1) //3D HQ mode
		{
			parsedUrl=ParseTokenedUrl(SITE_BASE,COVERFULLHQ,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
		}
                else //3D mode
		{
			parsedUrl=ParseTokenedUrl(SITE_BASE,COVERFULL,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
		}
                //if that fails try other coversite
                if(!getCoverFromServer(parsedUrl, imgPath, v, max)){
                    if(!(settings.covers3d)) //2d mode
                    {
                            parsedUrl=ParseTokenedUrl(ALT_SITE_BASE,ALT_COVER,ALT_PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
                    }
                    //else if(settings.covers3d && settings.hq==1) //3D HQ mode
                    //{
                    //	parsedUrl=ParseTokenedUrl(ALT_SITE_BASE,COVERFULLHQ,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
                    //}
                    else //3D mode
                    {
                            parsedUrl=ParseTokenedUrl(ALT_SITE_BASE,ALT_COVERFULL,ALT_PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
                    }

                }
		if(!getCoverFromServer(parsedUrl, imgPath, v, max)){
			
			//FALLBACK (ugly code)
			if(id[3] != 'E' && id[3] != 'J'){ //PAL default to EN
				if(!(settings.covers3d))
				{
					parsedUrl=ParseTokenedUrl(SITE_BASE,COVER,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword","EN",region,id);
				}
				else if(settings.covers3d && settings.hq==1) //3D HQ mode
				{
					parsedUrl=ParseTokenedUrl(SITE_BASE,COVERFULLHQ,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword","EN",region,id);
				}
                                else
				{
					parsedUrl=ParseTokenedUrl(SITE_BASE,COVERFULL,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword","EN",region,id);
				}
			}
			else{
				if(!(settings.covers3d))
				{
					parsedUrl=ParseTokenedUrl(SITE_BASE,COVER,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword","US",region,id);
				}
				else if(settings.covers3d && settings.hq==1) //3D HQ mode
				{
					parsedUrl=ParseTokenedUrl(SITE_BASE,COVERFULLHQ,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword","US",region,id);
				}
                                else
				{
					parsedUrl=ParseTokenedUrl(SITE_BASE,COVERFULL,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword","US",region,id);
				}
			}
                        getCoverFromServer(parsedUrl, imgPath, v, max);
/*
			if(!getCoverFromServer(parsedUrl, imgPath, v, max)){ //HQ failed so try normal 3D
                            if(settings.covers3d)
                            {
                                    parsedUrl=ParseTokenedUrl(SITE_BASE,COVERFULL,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
                            
                                    getCoverFromServer(parsedUrl, imgPath, v, max);
                            }
                        }
*/
                }
	}
	
	//
	// DISC ART
	//
	
	//snprintf(imgPath, sizeof(imgPath), "%s/disks/%s.png", USBLOADER_PATH, id);
	snprintf(imgPath, sizeof(imgPath), "%s/%s.png", dynPath.dir_disks, id);
	sprintf(self.debugMsg, TX.checkPresence, imgPath);
	Paint_Progress_Generic(v, max,self.debugMsg);
	
	fp = fopen(imgPath, "rb");
	if(fp)
	{
		fclose (fp);
		sprintf(self.debugMsg, TX.noDownload, imgPath);
		Paint_Progress_Generic(v, max,self.debugMsg);
	}
	else
	{
		parsedUrl=ParseTokenedUrl(SITE_BASE,DISK,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
		
		if(!getCoverFromServer(parsedUrl, imgPath, v, max))
		{

			//FALLBACK (ugly code)
			if(id[3] != 'E' && id[3] != 'J') //PAL default to EN
				parsedUrl=ParseTokenedUrl(SITE_BASE,DISK,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword","EN",region,id);
			else
				parsedUrl=ParseTokenedUrl(SITE_BASE,DISK,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword","US",region,id);
			

			if(!getCoverFromServer(parsedUrl, imgPath, v, max)) //first supplier failed so try second
                        {
                            parsedUrl=ParseTokenedUrl(ALT_SITE_BASE,ALT_DISK,ALT_PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
                        }
                        if(!getCoverFromServer(parsedUrl, imgPath, v, max)) //try custom disc image
                        {
                            parsedUrl=ParseTokenedUrl(SITE_BASE,CUSTOMDISK,PARAMTERISED_ARTWORK_LOCATION,"testUsername","testPassword",country,region,id);
                            getCoverFromServer(parsedUrl, imgPath, v, max);
                        }

		}
	}
} /* end download */


bool getCoverFromServer(char* url, char* imgPath, int v, int max){

	struct block file;
	char* pch;

	sprintf(self.debugMsg, TX.getting, url);
	Paint_Progress_Generic(v, max,self.debugMsg);

	file = downloadfile(url);

	if(file.data != NULL && file.size >= 4000){
	    char* msg = CFMalloc(20*sizeof(char));
	    strncpy(msg, (char*)file.data,20);
	    pch = strtok(msg, " ");
	    if(strcmp(pch,"<!DOCTYPE")==0) //test for a bad file
	     {
		   CFFree(msg);
		   CFFree(file.data);
		   return false;
	     }  
	    CFFree(msg);
	    unlink(imgPath);
 		saveFile(imgPath, file);
		CFFree(file.data);
		sprintf(self.debugMsg, TX.done );
		Paint_Progress_Generic(v, max,self.debugMsg);
		return true;
	}
	
	return false;
}

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

