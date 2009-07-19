#ifndef __DEFINES_H_
#define __DEFINES_H_


//***************************************************************
#define USB_DEVICE
#ifdef USB_DEVICE
	#define USBLOADER_PATH       "USB:/usb-loader"
#else
	#define USBLOADER_PATH       "SD:/usb-loader"
#endif
//***************************************************************
// enable/disable all code for NewsReader
 #define NEWS_READER
 #ifdef NEWS_READER 	
	#ifdef USB_DEVICE
		#define NEWSFILE   		"USB:/usb-loader/CoverFloader.news" 
	#else
		#define NEWSFILE   		"SD:/usb-loader/CoverFloader.news" 
	#endif
 #endif 
//***************************************************************
// Access to wiiboxart with data from file 
//#define WBA_BY_FILE          
 #ifdef WBA_BY_FILE 
	#ifdef USB_DEVICE
		#define URLBOXART_PASS	"USB:/usb-loader/wiiboxart.pass" 
	#else
		#define URLBOXART_PASS	"SD:/usb-loader/wiiboxart.pass" 
	#endif
 #endif 
//***************************************************************


#endif

