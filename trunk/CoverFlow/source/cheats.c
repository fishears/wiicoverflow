/*
 *  cheats.c
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
#include "coverflow.h"
#include "cheats.h"

extern s_self self;

bool check_txt(int id, struct discHdr *gameList)
{
    //check to see if the game has a txt file
    char buffer[500][128]; //500 lines of cheat codes at 128 chars per line
    char filename[128];
    char titleID[7];
    //int i;
    struct discHdr *header = &gameList[id];
    sprintf(titleID,"%s",header->id);
    sprintf(filename, "%s.txt", titleID);
    chdir("/");
    chdir(TXT_PATH);
    FILE *txtfile;
    txtfile = fopen(filename, "r");
    if(txtfile)
    {
        fgets ( buffer[0], sizeof buffer[0], txtfile );
        WindowPrompt("FOUND TXT",buffer[0],&okButton,0);
        fclose(txtfile);
        return true;
    }
    else
    {
        WindowPrompt("NO DICE","TXT FILE NOT FOUND",&okButton,0);
        return false;
    }

    //for(i=0;i<3;i++)
    //{
    //    fgets ( buffer[i], sizeof buffer[i], txtfile );
    //}
    //WindowPrompt("FOUND TXT",buffer[1],&okButton,0);
}

bool check_gct(int id, struct discHdr *gameList)
{
    //check to see if the game has a gct file
        char buffer[500][128]; //500 lines of cheat codes at 128 chars per line
    char filename[128];
    char titleID[7];
    //int i;
    struct discHdr *header = &gameList[id];
    sprintf(titleID,"%s",header->id);
    sprintf(filename, "%s.gct", titleID);
    chdir("/");
    chdir(GCT_PATH);
    FILE *txtfile;
    txtfile = fopen(filename, "r");
    if(txtfile)
    {
        fgets ( buffer[0], sizeof buffer[0], txtfile );
        WindowPrompt(filename,"FOUND CHEATS",&okButton,0);
        fclose(txtfile);
        return true;
    }
    else
    {
        WindowPrompt(filename,"CHEATS NOT FOUND",&okButton,0);
        return false;
    }
}

bool download_txt(int id, struct discHdr *gameList)
{
    //attempt to download the game's txt file from usbgecko
    char url[100];
    char titleID[7];
    char imgpath[50];
    struct block file;
    struct discHdr *header = &gameList[id];
    sprintf(titleID,"%s",header->id);
    sprintf(url, "%s%c/%s.txt", CODESITE, header->id[0] , titleID);
    WindowPrompt("URL",url,&okButton,0);
    file = downloadfile(url); //TODO figure out why this returns no data....
    if(file.data != NULL)
    {
        WindowPrompt("HEY","We got DATA",&okButton,0);
        sprintf(imgpath,"%s%s.txt",TXT_PATH,titleID);
        saveFile(imgpath, file);
        free(file.data);
        return true;
    }
    else
        return false;
}

void manage_cheats(int id, struct discHdr *gameList)
{
    //allow enable/disable of individual codes in the gct
}

