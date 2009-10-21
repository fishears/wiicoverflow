#ifndef __DEFINES_H_
#define __DEFINES_H_

//***************************************************************
//       Adds Debug-Info to file
//#define DEBUG_FILE
//#ifdef DEBUG_FILE
	#define DEBUG_FILENAME "SD:/usb-loader/debug.txt"
//#endif
//***************************************************************
//       shows MemInfo in first line
//#define SHOW_MEMINFO
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
//       adds OSK Keyboard
#define OSK
//***************************************************************
//       extracts IntroSound of selected game to file
//#define EXTRACT_INTROSOUND
//***************************************************************
//       allows using edited game titles in owntitles.txt
#define OWNTITLES
//***************************************************************
#endif

