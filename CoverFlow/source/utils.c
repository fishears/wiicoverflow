#include "utils.h"

extern s_settings settings;
extern s_gameSettings gameSetting;
extern s_self self;

// Slot light patch - from Bool's wiilight)
static vu32 *_wiilight_reg = (u32*)0xCD0000C0;

u32 swap32(u32 x)
{
	return (x >> 24) | ((x << 8) & 0x00FF0000UL) | ((x >> 8) & 0x0000FF00UL) | (x << 24);
}


void sysdate(char* ret_string){

	time_t now;
	struct tm  *ts;
	//char string[128];
	char dow[12];
	char month[12];
   
	now = time(NULL);
	ts = localtime(&now);
	
	switch(ts->tm_wday){
	
		case 0:
			sprintf(dow, localStr("M112", "Sun") );
			break;
		case 1:
			sprintf(dow, localStr("M113", "Mon") );
			break;
		case 2:
			sprintf(dow, localStr("M114", "Tue") );
			break;
		case 3:
			sprintf(dow, localStr("M115", "Wed") );
			break;
		case 4:
			sprintf(dow, localStr("M116", "Thu") );
			break;
		case 5:
			sprintf(dow, localStr("M117", "Fri") );
			break;
		case 6:
			sprintf(dow, localStr("M118", "Sat") );
			break;
	}
	
	switch(ts->tm_mon){
		
		case 0:
			sprintf(month, localStr("M119", "Jan") );
			break;
		case 1:
			sprintf(month, localStr("M120", "Feb") );
			break;
		case 2:
			sprintf(month, localStr("M121", "Mar") );
			break;
		case 3:
			sprintf(month, localStr("M122", "Apr") );
			break;
		case 4:
			sprintf(month, localStr("M123", "May") );
			break;
		case 5:
			sprintf(month, localStr("M124", "Jun") );
			break;
		case 6:
			sprintf(month, localStr("M125", "Jul") );
			break;
		case 7:
			sprintf(month, localStr("M126", "Aug") );
			break;
		case 8:
			sprintf(month, localStr("M127", "Sep") );
			break;
		case 9:
			sprintf(month, localStr("M128", "Oct") );
			break;
		case 10:
			sprintf(month, localStr("M129", "Nov") );
			break;
		case 11:
			sprintf(month, localStr("M130", "Dec") );
			break;
		//default:
		//	sprintf(month, "xxx: %d", ts->tm_mon);
		
	}
	sprintf(ret_string, "%s %d %s %02d:%02d", dow, ts->tm_mday, month, ts->tm_hour, ts->tm_min);
	//sprintf(ret_string, "%s %d %s %d at %02d:%02d", dow, ts->tm_mday, month, 1900+ts->tm_year, ts->tm_hour, ts->tm_min);
	//sprintf(ret_string, string);
	//WindowPrompt("Message", string, &okButton, NULL);
	
}

void ios_version_check()
{
    u32 iosrev = IOS_GetRevision();
    if(iosrev <REQUIRED_IOS_REV)
    {
        char string[80];
        sprintf(string, localStr("M131", "Current: cIOS Rev%d  Required: cIOS Rev%d"),iosrev, REQUIRED_IOS_REV);
        WindowPrompt(localStr("M132", "cIOS Upgrade Required"),string, &okButton, 0);
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

int Net_Init(char *ip){
        
	s32 res;
        
	while ((res = net_init()) == -EAGAIN)
		usleep(100 * 1000); //100ms
        
	if (if_config(ip, NULL, NULL, true) < 0) {
		WindowPrompt (localStr("M003", "ERROR!"), localStr("M085", "Cannot get local IP address."), &okButton, 0);
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

	exit(0);
}

/*
char *get_title(struct discHdr *header){

	char *title = cfg_get_title(header->id);
	if (title) return title;
	return header->title;
}
*/

void WiiLight(int enable)
{             
	// Toggle wiilight (thanks Bool for wiilight source)
    u32 val = (*_wiilight_reg&~0x20);        
    if(enable) val |= 0x20;             
    *_wiilight_reg=val;            
}



