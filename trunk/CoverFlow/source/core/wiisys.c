/*
 *  wiisys.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 * Code contained in this file was originally written by Giantpune (thank you for the use)
 */
#include "wiisys.h"
#include "TrackedMemoryManager.h"

#define TITLE_ID(x,y)		(((u64)(x) << 32) | (y))

//boot HBC in either HAXX or JODI locations
//this function expects WII_Initialize() be called before it is called
s32 WII_BootHBC()
{
	u32 tmdsize;
	u64 tid = 0;
	u64 *list;
	u32 titlecount;
	s32 ret;
	u32 i;

	ret = ES_GetNumTitles(&titlecount);
	if(ret < 0)
		return WII_EINTERNAL;

	list = CFMemAlign(32, titlecount * sizeof(u64) + 32);

	ret = ES_GetTitles(list, titlecount);
	if(ret < 0) {
		CFFree(list);
		return WII_EINTERNAL;
	}
	
	for(i=0; i<titlecount; i++) {
		if (list[i]==TITLE_ID(0x00010001,0x4A4F4449) || list[i]==TITLE_ID(0x00010001,0x48415858) || list[i]==TITLE_ID(0x00010001,0xaf1bf516))
		{
			tid = list[i];
			break;
		}
	}
	CFFree(list);

	if(!tid)
		return WII_EINSTALL;

	if(ES_GetStoredTMDSize(tid, &tmdsize) < 0)
		return WII_EINSTALL;

	return WII_LaunchTitle(tid);
}




