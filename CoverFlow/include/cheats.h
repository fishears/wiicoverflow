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
    #define LINES_PER_PAGE 9 //lines per page to display
    #define LINE_LENGTH 50 //max title line length
    #define MAX_CODES 20 //max number of cheat codes
    #define CHEAT_LINE 18 //cheat codeline length
    #define MAX_CHEATS 30 //maximum number of cheats per game

typedef struct {
    char title[LINE_LENGTH]; //cheat description text
    bool enabled; //is the cheat active
    bool editable; //variables found in codelines
    char codes[MAX_CODES][CHEAT_LINE]; //cheat codes block
    int codelines; //number of lines of cheat codes
} CHEAT[MAX_CHEATS];

typedef struct {
    bool selectAll; //select all/de-select all stus of page
} PAGE;


//struct llCODE{
//        unsigned int FirstBit; //first hex number
//        unsigned int SecondBit; //second hex number
//        struct llCODE * parent;
//        struct llCODE * child;
//};
//
//struct llCHEAT{
//        char title[LINE_LENGTH]; //cheat description text
//        bool enabled; //is the cheat active
//        struct llCODE * CodeList; //cheat codes list
//        struct llCHEAT * parent;
//        struct llCHEAT * child;
//};

    
    bool check_txt(int id, struct discHdr *gameList);
    bool check_gct(int id, struct discHdr *gameList);
    bool download_txt(int id, int mode, struct discHdr *gameList);
    void batch_download_txt(struct discHdr *gameList);
    void manage_cheats(int id, struct discHdr *gameList);
    int is_code(char* line);
    bool check_download(char* titleID);
    void create_gct(CHEAT cheat, int cheatcount,struct discHdr *gameList, int id, int cheatCount);
    void edit_codes(CHEAT cheat, int cheatNum);

#ifdef	__cplusplus
}
#endif

#endif	/* _CHEATS_H */

