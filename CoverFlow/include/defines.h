#ifndef __DEFINES_H_
#define __DEFINES_H_


//***************************************************************
//      enable/disable all code for additional USB-Support
#define USB_SUPPORT
//***************************************************************
//         enable/disable all code for NewsReader
 #define NEWS_READER
 #ifdef NEWS_READER 	
	#define NEWSFILE   		"CoverFloader.news" 
 #endif 
//***************************************************************
// Access to wiiboxart with data from file 
//#define WBA_BY_FILE          
 #ifdef WBA_BY_FILE 
	#define URLBOXART_PASS	"wiiboxart.pass" 
 #endif 
//***************************************************************




#endif

