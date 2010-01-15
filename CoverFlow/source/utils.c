#include "utils.h"
#include "TrackedMemoryManager.h"
#include "OSK.h"

#define MAX_URL_SIZE 256


extern s_settings settings;
extern s_gameSettings gameSetting;
extern s_self self;
extern s_title* titleList;
extern s_path dynPath;
extern int COVER_COUNT;

// Slot light patch - from Bool's wiilight)
static vu32 *_wiilight_reg = (u32*)0xCD0000C0;

u32 swap32(u32 x)
{
	return (x >> 24) | ((x << 8) & 0x00FF0000UL) | ((x >> 8) & 0x0000FF00UL) | (x << 24);
}


void getSysdate(char* now, char* ret_string)
{
	char year[3];
	char month[20];
	char day[3];
	char hour[3];
	char min[2];
	char dow[20];
	
	strncpy(year, now, 2);
	//strncpy(month, now+2, 2);
	strncpy(day, now+4, 2);
	strncpy(hour, now+6, 2);
	strncpy(min, now+8, 2);
	//strncpy(dow, now+10, 2);
	
	char tmp[20];
	int tmp_i;
	
	strncpy(tmp, now+2, 2);
	tmp_i = atoi(tmp);
	
	switch(tmp_i){
		
		case 0:
			sprintf(month, TX.Jan );
			break;
		case 1:
			sprintf(month, TX.Feb );
			break;
		case 2:
			sprintf(month, TX.Mar );
			break;
		case 3:
			sprintf(month, TX.Apr );
			break;
		case 4:
			sprintf(month, TX.May );
			break;
		case 5:
			sprintf(month, TX.Jun );
			break;
		case 6:
			sprintf(month, TX.Jul );
			break;
		case 7:
			sprintf(month, TX.Aug );
			break;
		case 8:
			sprintf(month, TX.Sep );
			break;
		case 9:
			sprintf(month, TX.Oct );
			break;
		case 10:
			sprintf(month, TX.Nov );
			break;
		case 11:
			sprintf(month, TX.Dec );
			break;
		//default:
		//	sprintf(month, "xxx: %d", ts->tm_mon);
		
	}
	
	strncpy(tmp, now+10, 2);
	tmp_i = atoi(tmp);
	
	switch(tmp_i){
	
		case 0:
			sprintf(dow, TX.Sun );
			break;
		case 1:
			sprintf(dow, TX.Mon );
			break;
		case 2:
			sprintf(dow, TX.Tue );
			break;
		case 3:
			sprintf(dow, TX.Wed );
			break;
		case 4:
			sprintf(dow, TX.Thu );
			break;
		case 5:
			sprintf(dow, TX.Fri );
			break;
		case 6:
			sprintf(dow, TX.Sat );
			break;
	}
	
	sprintf(ret_string, "%s %d %s %d (%02d:%02d)", dow, atoi(day), month, 2000+atoi(year), atoi(hour), atoi(min));
	//return ret_string;
}

void setSysdate(char* ret_string)
{
	//YYMMDDHHMI+dayofweek
	
	time_t now;
	struct tm  *ts;
   
	now = time(NULL);
	ts = localtime(&now);
	
	sprintf(ret_string, "%02d%02d%02d%02d%02d%02d", ts->tm_year-100, ts->tm_mon, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_wday);
	
}

void ios_version_check()
{
    u32 iosrev = IOS_GetRevision();
    if(iosrev <REQUIRED_IOS_REV)
    {
        char string[80];
        sprintf(string, TX.currentCIOS,iosrev, REQUIRED_IOS_REV);
        WindowPrompt(TX.cIOSupgrade,string, &okButton, 0);
    }
}

/**
 * This function calculates the number of frames we render each second.
 * It must be called right after GRRLIB_Render.
 * @return The number of frames per second.
 */
u8 CalculateFrameRate()
{

    static u8 frameCount = 0;
    static u32 lastTime;
    static u8 FPS = 0;
    u32 currentTime = ticks_to_millisecs(gettime());
	
    frameCount++;
    if(currentTime - lastTime > 1000) {
        lastTime = currentTime;
        FPS = frameCount;
        frameCount = 0;
    }
    return FPS;
}



void apply_settings()
{
    //called after global settings are saved and right before game is launched
    //this overwrites global settings with the per-game settings
    if(gameSetting.ocarina != -1)
    {
		settings.ocarina = gameSetting.ocarina;
        settings.hooktype = gameSetting.hooktype;
        settings.language = gameSetting.language;
        settings.video = gameSetting.video;
        settings.vipatch = gameSetting.vipatch;
    }
	
}

void apply_fixsettings()
{
	switch (gameSetting.fixtype)
	{
		case 0:  // None
			self.enableError002Fix = false;
			self.enableAnti002Fix  = false;
			break;
		case 1:  // error_002
			self.enableError002Fix = true;
			self.enableAnti002Fix  = false;
			break;
		case 2:  // Anti_002
			self.enableError002Fix = true;
			self.enableAnti002Fix  = true;
			break;
		default:
			self.enableError002Fix = false;
			self.enableAnti002Fix  = false;
	}

	// use alternate dol if needed
	self.alternatedol = gameSetting.altdoltype;
}




bool networkInit(char *ip)
{
	s32 res;
	while ((res = net_init()) == -EAGAIN)
		usleep(100 * 1000); //100ms
        
	if (if_config(ip, NULL, NULL, true) < 0)
	{
		WindowPrompt(TX.error, TX.noLocalIP, &okButton, 0);
		usleep(1000 * 1000 * 1); //1 sec
		return false;
	}
	
	return true;
}

void quit()
{
	//we should free all allocated textures (SCO);
	BUFFER_KillBuffer();
	freeResources();
	CFFree(titleList);
	shutdownNetworkThread();
	exit(0);
}

void WiiLight(int enable)
{             
	// Toggle wiilight (thanks Bool for wiilight source)
    u32 val = (*_wiilight_reg&~0x20);        
    if(enable) val |= 0x20;             
    *_wiilight_reg=val;            
}

void UpdateBufferedImages()
{
	SetSelectedCover(self.shift);
}

void DragSlider(int xPos)
{
	int min_loc = 125; // slider range
	int max_loc = 440;
	
	if (xPos >= max_loc)
		xPos = max_loc;
	else if (xPos <= min_loc)
		xPos = min_loc;
	
	float fx = change_scale(xPos, min_loc, max_loc, self.max_cover, self.min_cover);
	
	if ((fx > self.min_cover) && (fx < self.max_cover))
	{
		self.shift = fx;
		self.scrolling = true;
	}
	else if (fx >= self.max_cover)
	{
		self.shift    = self.max_cover;
		self.scrolling = false;
	}
	else if (fx <= self.min_cover)
	{
		self.shift = self.min_cover;
		self.scrolling = false;
	}
}

int getRandomNum(int min, int max){

	int diff = max - min + 1;
	int res = (rand() % diff)+min;
	
	return res;
}

// Penner's easing equations found here http://www.robertpenner.com/easing/
float easeOutQuint(float t, float b , float c, float d)
{
	t=t/d-1;
	return c*(t*t*t*t*t + 1) + b; //Quint
	//return c*((t=t/d-1)*t*t*t*t + 1) + b; //Quint
}

float easeInQuint(float t,float b , float c, float d)
{
	t=t/d;
	return c*t*t*t*t*t + b;
	//return c*(t/=d)*t*t*t*t + b;
}

float easeOutQuad(float t, float b , float c, float d)
{
	t=t/d;
	return -c *t*(t-2) + b;
	//return -c *(t/=d)*(t-2) + b;
}

float easeInQuad(float t, float b , float c, float d)
{
	t=t/d;
	return c*t*t + b;
	//return c*(t/=d)*t + b;
}

float easeOutCubic(float t, float b , float c, float d)
{
	t=t/d-1;
	return c*(t*t*t + 1) + b;
	//return c*((t=t/d-1)*t*t + 1) + b;
}

char * GetTokenValue(char * token, char* username, char* password, char * lang, char * region, char * id, int * nChars)
{
	char * ret;
	int ovnChars=0;
	int thisnChars=0;
	if (token[strlen(token)-1]>='0' && token[strlen(token)-1]<='9') 
	{
		ovnChars=token[strlen(token)-1]-'0';
		token[strlen(token)-1]=0;
	}
	if (strcasecmp(token,"username")==0)
	{
		ret=username;
		thisnChars=strlen(username);
	}
	if (strcasecmp(token,"password")==0)
	{
		ret=password;
		thisnChars=strlen(password);
	}
	if (strcasecmp(token,"gameID")==0)
	{
		ret=id;
		thisnChars=strlen(id);
	}
	if (strcasecmp(token,"lang")==0)
	{
		ret=lang;
		thisnChars=strlen(lang);;
	}
	if (strcasecmp(token,"region")==0)
	{
		ret=region;
		thisnChars=strlen(region);
	}
	if (ovnChars>0) *nChars=ovnChars;
	else *nChars=thisnChars;
	return ret;
}

char * ParseTokenedUrl(char * sitebase,char * subdirectory, char * url, char* username, char* password, char * lang, char * region, char *id)
{
	char * ret;
	int nChars=0;
	char * token;
	ret=(char *)GetSlotBufferAddress(11);
	memset(ret,0,128);
	token=(char *)GetSlotBufferAddress(12);
	memset(token,0,128);
	int i,pos=0;
	int tokenPos=0;
	bool inToken=false;
	
	for (i=0;i<strlen(sitebase);i++)
	{
		ret[pos++]=sitebase[i];
	}

	for (i=0;i<strlen(subdirectory);i++)
	{
		ret[pos++]=subdirectory[i];
	}

	for (i=0;i<strlen(url);i++)
	{
		if (!inToken)
		{
			if (url[i]=='[')
			{
				inToken=true;
			}
			else
			{
				ret[pos++]=url[i];
			}
		}
		else
		{
			if (url[i]==']')
			{

				inToken=false;
				token[tokenPos]=0;
				char * tokenVal=GetTokenValue(token,username,password,lang,region,id,&nChars);
				int j;
				for (j=0;j<nChars;j++)
				{
					ret[pos++]=tokenVal[j];
				}
				tokenPos=0;
			}
			else
			{
				token[tokenPos++]=url[i];
			}

		}
	}
	//free(token);
	return ret;
}

#ifdef DEBTXT_ON
int DebTxt( char * Msg)
{
	char       buf[255];
	FILE       *fp;

	time_t     now;
	struct tm  *ts;

	now = time(0);
	ts = localtime(&now);

	sprintf(buf, "%d-%02d-%02d %02d:%02d:%02d    %s", ts->tm_year+1900, ts->tm_mon+1, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec, Msg);

	fp = fopen(DEBUG_FILENAME, "a");
	if (!fp) 
		return -1;
	fprintf(fp, "%s\n", buf);
	fclose(fp);
	return 1;
}
#endif

int IOS2int()
{
 int ios2;
 switch (gameSetting.ios)
 {
	case i249:
		ios2 = 249;
		break;

	case i222:
		ios2 = 222;
		break;

	case i223:
		ios2 = 223;
		break;

	default:
		ios2 = 249;
		break;
 }
 return ios2;
}


bool CombineTitleFiles()
{
	FILE *in; //owntitles.txt
	FILE *out;//titles.txt
	char ch;
	char tbuf[255];
	
	sprintf(tbuf,"%s/owntitles.txt", dynPath.dir_usb_loader);
	if((in=fopen(tbuf, "r")) == NULL)
	{
		return false;
	}

	sprintf(tbuf,"%s/titles.txt", dynPath.dir_usb_loader);
	if((out=fopen(tbuf, "a")) == NULL) 
	{
		return false;
	}
	
	while(!feof(in)) 
	{
		ch = getc(in);
        if(!feof(in)) 
			putc(ch, out);
	}
	fclose(in);
	fclose(out);

	return true;
}




int editGameID()
{
	int ret;
	s32 wert;
	int i;
	char titleID[7];
	
	struct discHdr *header = NULL;
	header = &self.gameList[self.gameSelected];
	
	strcpy(self.kb_buffer,(char*)header->id);
	ret = showOSK("Edit GameID");
	if (ret > 0 )
	{
	 strcat( self.kb_buffer, "111111" );
	 for (i=0; i<6; i++)
	 {
	   self.kb_buffer[i] = (char)toupper((int)self.kb_buffer[i]);
	 }
	 self.kb_buffer[6] = '\0';
	 
	 // ToDo:
	 // check if the new GameID already exists
	 // if exists 
	 // WindowPrompt ("GameID already exists!", self.kb_buffer, &cancelButton,0);
	 // return -1;
	 
	 wert = WBFS_ReIDGame(header->id, self.kb_buffer);

	 BUFFER_KillBuffer(); 
	 GetEntriesSilent();
	 BUFFER_InitBuffer();

	 for (i=0;i<=self.gameCnt;i++)
	 {
		struct discHdr *header = &self.gameList[i];
		sprintf(titleID, "%s", header->id);
		if(strcmp(titleID,self.kb_buffer)==0)
			self.shift = (COVER_COUNT/2)-i;
	 }
	 
	 InitializeBuffer(self.gameList, self.gameCnt,BUFFER_WINDOW,COVER_COUNT/2.0 +self.shift,(settings.covers3d+settings.hq));
	 return 1;
	}
	return 0;
}

bool bannerBlacKlist(const char *id) 
{
 //if (strcmp(id,"R9JP69") == 0) return true;  // The Beatles: Rockband
 
 return false;
}

















