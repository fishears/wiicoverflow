/*
 *  cheats.h
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 *  This file contains the main() entry point to the application
 *  Build configuration settings can be found in "coverflow.h"
 */

#ifndef _CHEATS_H
#define	_CHEATS_H

#ifdef	__cplusplus
extern "C" {
#endif

    #define TXT_PATH "/txtcodes/"
    #define GCT_PATH "sd:/codes/"
    #define CODESITE "http://www.usbgecko.com/codes/codes/"
    
    bool check_txt(int id, struct discHdr *gameList);
    bool check_gct(int id, struct discHdr *gameList);
    bool download_txt(int id, struct discHdr *gameList);
    void manage_cheats(int id, struct discHdr *gameList);

#ifdef	__cplusplus
}
#endif

#endif	/* _CHEATS_H */

