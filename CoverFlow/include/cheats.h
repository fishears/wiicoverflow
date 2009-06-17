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
    #define LINES_PER_PAGE 10
    #define LINE_LENGTH 50
    #define MAX_CODES 100
    #define MAX_CHEATS 10 /*I Changed this from 500 to 10 just to get it working*/


/*
   Wow... lots of mem hogged here...
   Every time you call "manage_cheats" you try to reserve
   500*(100 + 500*100) bytes
   500*50K = 25000K or ~25 MB
   
   Is my math right?
   
   No wonder I'm getting code dumps on the call :) Changed max cheats to 10 for now
   
 */
typedef struct {
        char title[LINE_LENGTH]; //cheat description text
        bool enabled; //is the cheat active
        char codes[MAX_CODES][LINE_LENGTH]; //cheat codes block
        int codelines; //number of lines of cheat codes
} CHEAT[MAX_CHEATS];

    
    bool check_txt(int id, struct discHdr *gameList);
    bool check_gct(int id, struct discHdr *gameList);
    bool download_txt(int id, struct discHdr *gameList);
    void manage_cheats(int id, struct discHdr *gameList);
    bool is_code(char* line);
    bool check_download(char* titleID);
    void create_gct(CHEAT cheat, int cheatcount,char* titleID);

#ifdef	__cplusplus
}
#endif

#endif	/* _CHEATS_H */

