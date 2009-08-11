/*
 *  cheats.h
 *
 *  Wii CoverFloader
 *  Copyright 2009 Beardface April 29th, 2009
 *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11, alexcarlosantao
 *  Licensed under the terms of the GNU GPL, version 2
 *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 *
 */

#ifndef _CHEATS_H
#define	_CHEATS_H

#include "defines.h"
#ifdef	__cplusplus
extern "C" {
#endif

    //#define TXT_PATH "/txtcodes/"
    //#define GCT_PATH "sd:/codes/"

    #define CODESITE "http://geckocodes.org/codes/"
    #define LINES_PER_PAGE 9 //lines per page to display
    #define LINE_LENGTH 50 //max title line length
    #define MAX_CODES 30 //max number of cheat codes
    #define CHEAT_LINE 18 //cheat codeline length
    #define MAX_CHEATS 37 //maximum number of cheats per game

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
    
    bool check_txt(int id, struct discHdr *gameList); //check for txt cheat file on sd
    bool check_gct(int id, struct discHdr *gameList); //check for gct cheat file on sd
    bool download_txt(int id, int mode, struct discHdr *gameList); //download txt cheat file
    void batch_download_txt(struct discHdr *gameList); // download txt cheat files for all games
    void manage_cheats(int id, struct discHdr *gameList); //main cheat manager gui and parser
    int is_code(char* line); //check if line is a code line & if it is an editable code line
    bool check_download(); //check downloaded txt cheat file for validity
    void create_gct(CHEAT *cheat, int cheatcount,struct discHdr *gameList, int id, int cheatCount); //parse selected cheats into a gct
    void edit_codes(CHEAT *cheat, int cheatNum); //main code line editor gui
    void edit_variables(char* line); //lets you edit the variable values in a codeline

#ifdef	__cplusplus
}
#endif

#endif	/* _CHEATS_H */

