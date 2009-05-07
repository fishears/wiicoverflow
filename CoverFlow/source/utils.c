#include "utils.h"

u32 swap32(u32 x)
{
        return (x >> 24) | ((x << 8) & 0x00FF0000UL) | ((x >> 8) & 0x0000FF00UL) | (x << 24);
}


void sysdate(){

	time_t now;
	struct tm  *ts;
	char string[128];
	char dow[12];
	char month[12];
   
	now = time(NULL);
	ts = localtime(&now);
	
	switch(ts->tm_wday){
	
		case 1:
			sprintf(dow, "Sunday");
		case 2:
			sprintf(dow, "Monday");
		case 3:
			sprintf(dow, "Tuesday");
		case 4:
			sprintf(dow, "Wednesday");
		case 5:
			sprintf(dow, "Thursday");
		case 6:
			sprintf(dow, "Friday");
		case 7:
			sprintf(dow, "Saturday");
	}
	
	switch(ts->tm_mon){
		
		case 0:
			sprintf(month, "January");
		case 1:
			sprintf(month, "February");
		case 2:
			sprintf(month, "March");
		case 3:
			sprintf(month, "April");
		case 4:
			sprintf(month, "May");
		case 5:
			sprintf(month, "June");
		case 6:
			sprintf(month, "July");
		case 7:
			sprintf(month, "August");
		case 8:
			sprintf(month, "September");
		case 9:
			sprintf(month, "October");
		case 10:
			sprintf(month, "November");
		case 11:
			sprintf(month, "Dicember");
		
	}
	sprintf(string, "Sysdate: %s %d %s %d at %d:%d", dow, ts->tm_mday, month, 1900+ts->tm_year, ts->tm_hour, ts->tm_min);
	
	WindowPrompt("Message", string, &okButton, NULL);
	
}
