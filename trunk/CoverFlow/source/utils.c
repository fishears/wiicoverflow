#include "utils.h"

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
			sprintf(dow, "Sunday");
			break;
		case 1:
			sprintf(dow, "Monday");
			break;
		case 2:
			sprintf(dow, "Tuesday");
			break;
		case 3:
			sprintf(dow, "Wednesday");
			break;
		case 4:
			sprintf(dow, "Thursday");
			break;
		case 5:
			sprintf(dow, "Friday");
			break;
		case 6:
			sprintf(dow, "Saturday");
			break;
	}
	
	switch(ts->tm_mon){
		
		case 0:
			sprintf(month, "January");
			break;
		case 1:
			sprintf(month, "February");
			break;
		case 2:
			sprintf(month, "March");
			break;
		case 3:
			sprintf(month, "April");
			break;
		case 4:
			sprintf(month, "May");
			break;
		case 5:
			sprintf(month, "June");
			break;
		case 6:
			sprintf(month, "July");
			break;
		case 7:
			sprintf(month, "August");
			break;
		case 8:
			sprintf(month, "September");
			break;
		case 9:
			sprintf(month, "October");
			break;
		case 10:
			sprintf(month, "November");
			break;
		case 11:
			sprintf(month, "Dicember");
			break;
		//default:
		//	sprintf(month, "xxx: %d", ts->tm_mon);
		
	}
	sprintf(ret_string, "%s %d %s %d at %02d:%02d", dow, ts->tm_mday, month, 1900+ts->tm_year, ts->tm_hour, ts->tm_min);
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
