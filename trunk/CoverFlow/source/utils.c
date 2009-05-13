#include "utils.h"

extern s_settings settings;
extern s_gameSettings gameSetting;
extern s_self self;

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
			sprintf(dow, "Sun");
			break;
		case 1:
			sprintf(dow, "Mon");
			break;
		case 2:
			sprintf(dow, "Tue");
			break;
		case 3:
			sprintf(dow, "Wed");
			break;
		case 4:
			sprintf(dow, "Thu");
			break;
		case 5:
			sprintf(dow, "Fri");
			break;
		case 6:
			sprintf(dow, "Sat");
			break;
	}
	
	switch(ts->tm_mon){
		
		case 0:
			sprintf(month, "Jan");
			break;
		case 1:
			sprintf(month, "Feb");
			break;
		case 2:
			sprintf(month, "Mar");
			break;
		case 3:
			sprintf(month, "Apr");
			break;
		case 4:
			sprintf(month, "May");
			break;
		case 5:
			sprintf(month, "Jun");
			break;
		case 6:
			sprintf(month, "Jul");
			break;
		case 7:
			sprintf(month, "Aug");
			break;
		case 8:
			sprintf(month, "Sep");
			break;
		case 9:
			sprintf(month, "Oct");
			break;
		case 10:
			sprintf(month, "Nov");
			break;
		case 11:
			sprintf(month, "Dec");
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
        sprintf(string, "Current: cIOS Rev%d  Required: cIOS Rev%d",iosrev, REQUIRED_IOS_REV);
        WindowPrompt("cIOS Upgrade Required",string, &okButton, 0);
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
		WindowPrompt ("ERROR!","Cannot get local IP address.", &okButton, 0);
		usleep(1000 * 1000 * 1); //1 sec
		
		return false;
	}
	
	return true;
}

