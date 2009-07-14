#ifndef __DEFINES_H_
#define __DEFINES_H_



//***************************************************************
// enable/disable all code for NewsReader
 #define NEWS_READER
 #ifdef NEWS_READER 	
	#define NEWSFILE   		"SD:/usb-loader/CoverFloader.news" 
 #endif 
//***************************************************************
// Access to wiiboxart with data from file 
//#define WBA_BY_FILE          
 #ifdef WBA_BY_FILE 
	#define URLBOXART_PASS	"SD:/usb-loader/wiiboxart.pass" 
 #endif 
//***************************************************************


#endif

