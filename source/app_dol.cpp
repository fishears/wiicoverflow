///////////////////////////////////////////////////////////////////////////////
// Class C_App_dol                                                           //
// Created by Hell Hibou (2008)                                              //
// Part of Wii Homebrew Launcher                                             //
// based on sources of shagkur of the devkitpro team, thx                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include <gccore.h>

#include "app_dol.h"


///////////////////////////////////////////////////////////////////////////////

void * C_App_dol::Load (const void * dol, const char * CommandLine, const u8 Action)
{
	u32 i;
	C_App_dol::Header *dolfile;

	if (!dol) { return NULL; } 
	
	dolfile = (C_App_dol::Header *) dol;
	
	if (dolfile->entry_point < 0x80003F00)	{ return NULL; }
	if (dolfile->entry_point > 0x81800000)	{ return NULL; }	
	if (dolfile->bss_start < 0x80003F00)	{ return NULL; }
	if (dolfile->bss_start + dolfile->bss_size > 0x81800000) { return NULL; }
	
	for (i = 0; i < 7; i++) 
	{
		if ((!dolfile->text_size[i]) ||  (dolfile->text_start[i] < 0x100)) { continue; }
		
		if (dolfile->text_start[i] < 0x80003F00) { return NULL; }
		if (dolfile->text_start[i] + dolfile->text_size[i] > 0x81800000) { return NULL; }
		
	#ifndef APP_MODE_LITE
		if (Action == APP_ACTION_PRELOAD) { SetLibogcArg ( ((u8*)dol)+dolfile->text_pos[i], CommandLine); }
	#endif
		
		if (Action == APP_ACTION_RUN)
		{
			ICInvalidateRange ((void *) dolfile->text_start[i], dolfile->text_size[i]);
			memmove ((void *) dolfile->text_start[i], ((u8*)dol) + dolfile->text_pos[i], dolfile->text_size[i]);
		}
	}

	for(i = 0; i < 11; i++) 
	{
		if ((!dolfile->data_size[i]) || (dolfile->data_start[i] < 0x100)) { continue; }
		
		if (dolfile->data_start[i] < 0x80003F00) { return NULL; }
		if (dolfile->data_start[i] + dolfile->data_size[i] > 0x81800000) { return NULL; }
		
		if (Action == APP_ACTION_RUN)
		{
			memmove ((void*) dolfile->data_start[i], ((u8*)dol)+dolfile->data_pos[i], dolfile->data_size[i]);
			DCFlushRangeNoSync ((void *) dolfile->data_start[i], dolfile->data_size[i]);
		}
	}

	if (Action == APP_ACTION_RUN)
	{
		memset ((void *) dolfile->bss_start, 0, dolfile->bss_size);
		DCFlushRange((void *) dolfile->bss_start, dolfile->bss_size);
	}
	
    return (void*)dolfile->entry_point;
}

///////////////////////////////////////////////////////////////////////////////
