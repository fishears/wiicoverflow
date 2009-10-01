/*
 *  discbrowser.c
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  This file contains code originally written by giantpune, lustar.mii & dimok
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "coverflow.h"
#include "menuMultiDOL.h"
 extern s_self 		self; 

int autoSelectDol(const char *id) 
{
	int ret;
	if (strcmp(id,"R3MP01") == 0)				// Metroid Prime Trilogy PAL
	{
		switch(self.selectedDol)
		{
			case 1:
				ret = 782;
				break;
			case 2:
				ret = 783;
				break;
			case 3:
			    ret = 784;
				break;
			default:
				ret = 782;
				break;
		}
		return ret;
	}
    if (strcmp(id,"R3ME01") == 0)				// Metroid Prime Trilogy NTSC
	{
		switch(self.selectedDol)
		{
			case 1:
				ret = 780;
				break;
			case 2:
				ret = 781;
				break;
			case 3:
				ret = 782;
				break;
			default:
				ret = 780;
				break;
		}
		return ret;
	}
	if (strcmp(id,"RF8E69") == 0) return 439;  // Fifa 08 NTSC - thanks to isostar
    if (strcmp(id,"RF8P69") == 0) return 463;  // Fifa 08 PAL - thanks to isostar
    if (strcmp(id,"RF8X69") == 0) return 464;	// Fifa 08 PAL - thanks to isostar

    if (strcmp(id,"RZTP01") == 0) return 952;  // Wii Sports Resort PAL - thanks to isostar
    if (strcmp(id,"RZTE01") == 0) return 674;  // Wii Sports Resort NTSC - thanks to starstremr

    if (strcmp(id,"RMZX69") == 0) return 492;  // Medal of Honor Heroes PAL - thanks to isostar
    if (strcmp(id,"RMZP69") == 0) return 492;  // Medal of Honor Heroes PAL - thanks to isostar
    if (strcmp(id,"RMZE69") == 0) return 492;  // Medal of Honor Heroes NTSC - thanks to starstremr
	
	if (strcmp(id,"RM2X69") == 0) return 601;  // Medal Of Honor Heroes 2 PAL - thanks to dj_skual
	if (strcmp(id,"RM2P69") == 0) return 517;	// Medal Of Honor Heroes 2 PAL - thanks to MZottel
	if (strcmp(id,"RM2E69") == 0) return 492;	// Medal Of Honor Heroes 2 NTSC - thanks to Old8oy
	
    if (strcmp(id,"REDP41") == 0) return 1957; // Red Steel PAL - thanks to isostar
    if (strcmp(id,"REDE41") == 0) return 1957; // Red Steel NTSC - thanks to starstremr

    if (strcmp(id,"RSXP69") == 0) return 377;  // SSX Blur PAL
    if (strcmp(id,"RSXE69") == 0) return 377;  // SSX Blur NTSC

    if (strcmp(id,"RNBX69") == 0) return 964;  // NBA Live 08 PAL - thanks to isostar

    if (strcmp(id,"RMDP69") == 0) return 39;   // Madden NFL 07 PAL - thanks to isostar

	if (strcmp(id,"RNFP69") == 0) return 1079; // Madden NFL 08 PAL - thanks to isostar

    if (strcmp(id,"RMLP7U") == 0) return 56;   // Metal Slug Anthology PAL - thanks to isostar

    if (strcmp(id,"RKMP5D") == 0) return 290;  // Mortal Kombat Armageddon PAL - thanks to isostar
    if (strcmp(id,"RKME5D") == 0) return 290;  // Mortal Kombat Armageddon NTSC - thanks to starstremr

    if (strcmp(id,"R5TP69") == 0) return 1493; // Grand Slam Tennis PAL - thanks to isostar
    if (strcmp(id,"R5TE69") == 0) return 1493; // Grand Slam Tennis NTSC - thanks to starstremr

    if (strcmp(id,"R9OP69") == 0) return 1991; // Tiger Woods PGA Tour '10 PAL - thanks to isostar
    if (strcmp(id,"R9OE69") == 0) return 1973; // Tiger Woods PGA Tour '10 NTSC - thanks to starstremr

    if (strcmp(id,"RVUP8P") == 0) return 16426;// Virtua Tennis 2009 PAL - thanks to isostar
    if (strcmp(id,"RVUE8P") == 0) return 16405;// Virtua Tennis 2009 NTSC - thanks to isostar

    if (strcmp(id,"RHDP8P") == 0) return 149;  // The House Of The Dead 2 & 3 Return PAL - thanks to isostar
    if (strcmp(id,"RHDE8P") == 0) return 149;  // The House Of The Dead 2 & 3 Return NTSC - thanks to starstremr


    if (strcmp(id,"RJ8P64") == 0) return 8;    // Indiana Jones And The Staff Of Kings PAL - thanks to isostar
    if (strcmp(id,"RJ8E64") == 0) return 8;    // Indiana Jones And The Staff Of Kings NTSC - thanks to starstremr

    if (strcmp(id,"RBOP69") == 0) return 675;  // Boogie PAL - thanks to isostar
    if (strcmp(id,"RBOE69") == 0) return 675;  // Boogie NTSC - thanks to starstremr

    if (strcmp(id,"RPYP9B") == 0) return 12490;// Pangya Golf With Style PAL - thanks to isostar
    
	return -1;
}



int checkMultiDol(const char *id)
{
 int ret = 0;
 
 // Metroid Prime Trilogy
 if ((strcmp(id,"R3ME01") == 0) || (strcmp(id,"R3MP01") == 0))
   {
     ret = showDOLWindow();
	 return ret;
   }
 return ret;
}


