///////////////////////////////////////////////////////////////////////////////
// Class C_App_dol                                                           //
// Created by Hell Hibou (2008)                                              //
// Part of Wii Homebrew Launcher                                             //
// based on sources of shagkur of the devkitpro team, thx                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#ifndef _CLASS_UOBIHLLEH_C_APP_DOL
#define _CLASS_UOBIHLLEH_C_APP_DOL

#include "app.h"

#define APP_TYPE_DOL	1


class C_App_dol: public C_App
{
	public:
		void * Load (const void * dol, const char * CommandLine = NULL, u8 Action = 0);
	
	private:
		typedef struct Header 
		{
			u32 text_pos   [7];
			u32 data_pos   [11];
			u32 text_start [7];
			u32 data_start [11];
			u32 text_size  [7];
			u32 data_size  [11];
			u32 bss_start;
			u32 bss_size;
			u32 entry_point;
		};
};

#endif

///////////////////////////////////////////////////////////////////////////////
