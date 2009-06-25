#include "utils.h"

extern s_settings settings;
extern s_gameSettings gameSetting;
extern s_self self;
extern s_title* titleList;

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
	char min[3];
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
	//char string[128];
	//char dow[20];
	//char month[20];
   
	now = time(NULL);
	ts = localtime(&now);
	
	/*
	switch(ts->tm_wday){
	
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
	
	switch(ts->tm_mon){
		
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
	*/
	
	//sprintf(ret_string, "%s %d %s %02d:%02d", dow, ts->tm_mday, month, ts->tm_hour, ts->tm_min);
	sprintf(ret_string, "%02d%02d%02d%02d%02d%02d", ts->tm_year-100, ts->tm_mon, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_wday);
	
	//WindowPrompt("Message", string, &okButton, NULL);
	
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

/*
bool networkInit()
{	//if(self.firstTimeDownload == true){
		
		//char myIP[16];
		
		//sprintf(self.debugMsg, TX.iniNet );
		//Paint_Progress_Generic(v, max, self.debugMsg);
		
		if(!ifConfig(self.ipAddress))
		{
			//sprintf(self.debugMsg, TX.iniNetErr );
			//Paint_Progress_Generic(v, max, self.debugMsg);
			//WindowPrompt (TX.error, TX.iniNetErr, &okButton, 0);
			return false;
		}

		return true;
		//self.firstTimeDownload = false;
	//}
}
*/
void quit()
{
	//we should free all allocated textures (SCO);
	BUFFER_KillBuffer();
	freeResources();
	free(titleList);
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
	return c*((t=t/d-1)*t*t*t*t + 1) + b; //Quint
}

float easeInQuint(float t,float b , float c, float d)
{
	return c*(t/=d)*t*t*t*t + b;
}

float easeOutQuad(float t, float b , float c, float d)
{
	return -c *(t/=d)*(t-2) + b;
}

float easeInQuad(float t, float b , float c, float d)
{
	return c*(t/=d)*t + b;
}

float easeOutCubic(float t, float b , float c, float d)
{
	return c*((t=t/d-1)*t*t + 1) + b;
}

