///////////////////////////////////////////////////////////////////////////////
// Class C_App                                                               //
// Created by Hell Hibou (2008)                                              //
// Part of Wii Homebrew Launcher                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef _CLASS_UOBIHLLEH_C_APP
#define _CLASS_UOBIHLLEH_C_APP

#include <gccore.h>

#define APP_ACTION_PRELOAD	1
#define APP_ACTION_RUN		0

// #define APP_MODE_LITE

class C_App
{
	public:
	#ifndef APP_MODE_LITE
		int Launch (const void *, const char * CommandLine = NULL);
	#endif
		virtual void * Load (const void *, const char * CommandLine = NULL, const u8 Action = APP_ACTION_RUN) = 0;
	
	protected:
	#ifndef APP_MODE_LITE
		static int SetLibogcArg (const void * Data, const char * CmdLine);
		static void * LoadDOL (const void * Dol);
	#endif
};

#endif

///////////////////////////////////////////////////////////////////////////////
