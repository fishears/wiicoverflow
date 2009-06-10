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
    char filename[10];
    char titleID[7];
    char path[50];
    //int i;
    struct discHdr *header = &gameList[id];
    sprintf(titleID,"%s",header->id);
    sprintf(filename, "%s.txt", titleID);
    chdir("/");
    sprintf(path,"%s%s",USBLOADER_PATH,TXT_PATH);
    chdir(path);
    FILE *txtfile;
    txtfile = fopen(filename, "r");
    if(txtfile)
    {
        fgets ( buffer[0], sizeof buffer[0], txtfile );
        WindowPrompt(filename,"txt codes found on SD",&okButton,0);
        fclose(txtfile);
        return true;
    }
    else
    {
        WindowPrompt(filename,"txt codes not found",&okButton,0);
        return false;
    }
}

bool check_gct(int id, struct discHdr *gameList)
{
    //check to see if the game has a gct file
    char buffer[500][128]; //500 lines of cheat codes at 128 chars per line
    char filename[10];
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
        WindowPrompt(filename,"gct file found",&okButton,0);
        fclose(txtfile);
        return true;
    }
    else
    {
        WindowPrompt(filename,"gct file not found",&okButton,0);
        return false;
    }
}

bool download_txt(int id, struct discHdr *gameList)
{
    //attempt to download the game's txt file from www.usbgecko.com
    if(networkInit(self.ipAddress)){
        char url[100];
        char titleID[7];
        char imgpath[50];
        struct block file;
        struct discHdr *header = &gameList[id];
        sprintf(titleID,"%s",header->id);
        sprintf(url, "%s%c/%s.txt", CODESITE, header->id[0] , titleID); //try 6-digit ID first
        file = downloadfile(url);
        if(file.data == NULL || file.size < 204) //nothing, so try 4-digit ID
        {
            sprintf(url, "%s%c/%C%c%c%c.txt", CODESITE, header->id[0], header->id[0], header->id[1],header->id[2],header->id[3]);
            file = downloadfile(url);
        }
        if(file.data != NULL && file.size >203)
        {
            WindowPrompt(titleID,"txt codes downloaded",&okButton,0);
            sprintf(imgpath,"%s%s%s.txt",USBLOADER_PATH,TXT_PATH,titleID);
            saveFile(imgpath, file);
            free(file.data);
            return true;
        }
        else
        {
            if(file.size<204)
                sprintf(file.error,"Cheats not available");
            WindowPrompt("HTTP ERROR",file.error,&okButton,0);
            return false;
        }
    }
    WindowPrompt("ERROR",TX.errEstablishConn,&okButton,0);
    return false;
}

void manage_cheats(int id, struct discHdr *gameList)
{
    //allow enable/disable of individual codes in the gct
    char buffer[500][128]; //500 lines of cheat codes at 128 chars per line
    char filename[10];
    //char gctname[10];
    char titleID[7];
    char path[50];
    int i, bufflines;
    struct discHdr *header = &gameList[id];
    sprintf(titleID,"%s",header->id);
    sprintf(filename, "%s.txt", titleID);
    chdir("/");
    sprintf(path,"%s%s",USBLOADER_PATH,TXT_PATH);
    //sprintf(gctname,"%s.gct",titleID); this screws filename... don't see why though...
    chdir(path);
    FILE *txtfile;
    txtfile = fopen(filename, "r");
    if(txtfile)
    {
        for(i=0;i<3;i++) //read in the first three lines and discard them (id,name,blank line)
        {
            fgets (buffer[0], sizeof buffer[0], txtfile );
            memset(&buffer[0], 0, sizeof(buffer[0]));
        }
        i = 0;
        while(!feof(txtfile)) //read the rest of the txt file into buffer array
        {
            fgets(buffer[i],sizeof buffer[i],txtfile);
            i++;

        }
        bufflines = i;
        char tTemp[5];
        sprintf(tTemp,"%d",i);
        WindowPrompt("total lines",tTemp,&okButton,0);
        fclose(txtfile);
        //int descptr[200]; //pointer to the text description lines in buffer
        GRRLIB_Rectangle(40, 56, 560, 376, 0xffffffdd, true); //draw a box
        GRRLIB_Rectangle(42, 58, 556, 372, 0x737373FF, true);
        //GRRLIB_Render();
        int step = 20;
        //
        for(i=0;i<10;i++)
        {
            CFreeTypeGX_DrawText(ttf14pt, 45, 60+step, buffer[i], (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
            step +=20;
        }
        GRRLIB_Render();
        
        while(1)
        {
            WPAD_ScanPads();
            GetWiimoteData();
            if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B))
                return;
        }
    }
    fclose(txtfile);
        return;



        chdir("/");
        chdir("GCT_PATH");




        //cheat encoding by brkirch
/*
        FILE *codesFile, *gctFile;
        char tempCode[16];
        int vaildCharCount = 0, isComment = 0, currentChar, enableAll = 0, codeEnable = 0, i;
        enableAll = 1;
        codesFile = fopen("temp.txt", "r");
        gctFile = fopen(gctname, "w");
        fprintf(gctFile, "%c%c%c%c%c%c%c%c", 0x00, 0xD0, 0xC0, 0xDE, 0x00, 0xD0, 0xC0, 0xDE);
        currentChar = getc(codesFile);
        if (!enableAll)
            codeEnable = 0;
        else
            codeEnable = 1;
        while (currentChar != EOF)
        {
            if (((currentChar >= 'a') && (currentChar <= 'z')) || ((currentChar >= 'A') && (currentChar <= 'Z')) || ((currentChar >= '0') && (currentChar <= '9')) || (currentChar == 10) || (currentChar == 13) || (currentChar == ' ') || (currentChar == '*'))
            {
                if ((currentChar == 10) || (currentChar == 13))
                {
                    if (vaildCharCount != 16)
                    {
                        vaildCharCount = 0;
                        isComment = 0;
                        if (!enableAll)
                            codeEnable = 0;
                        else
                            codeEnable = 1;
                    }
                }
                else if (((currentChar >= 'a') && (currentChar <= 'f')) || ((currentChar >= 'A') && (currentChar <= 'F')) || ((currentChar >= '0') && (currentChar <= '9')))
                {
                    if (codeEnable && !isComment && (vaildCharCount < 16))
                        tempCode[vaildCharCount++] = currentChar;
                    else if (vaildCharCount == 16)
                        isComment = 1;
                }
                else if (currentChar == '*')
                {
                    codeEnable = 1;
                    if (vaildCharCount != 0)
                        isComment = 1;
                }
                else if (currentChar == ' ')
                {
                    if ((vaildCharCount != 0) && (vaildCharCount != 8) && (vaildCharCount != 16))
                        isComment = 1;
                }
                else
                {
                    if (vaildCharCount != 0)
                        isComment = 1;
                }
                if ((currentChar == ' ') || (currentChar == 10) || (currentChar == 13))
                {
                    if (vaildCharCount == 16 && !isComment)
                    {
                        if ((currentChar == 10) || (currentChar == 13))
                        {
                            vaildCharCount = 0;
                            isComment = 0;
                            if (enableAll == 0)
                                codeEnable = 0;
                            else
                                codeEnable = 1;
                        }
                        else
                            isComment = 1;
                        currentChar = 0;
                        for (i = 0; i < 16; i++)
                        {
                            if ((tempCode[i] >= 'a') && (tempCode[i] <= 'f'))
                                currentChar += 10 + tempCode[i] - 'a';
                            if ((tempCode[i] >= 'A') && (tempCode[i] <= 'F'))
                                currentChar += 10 + tempCode[i] - 'A';
                            if ((tempCode[i] >= '0') && (tempCode[i] <= '9'))
                                currentChar += tempCode[i] - '0';
                            if (i % 2 == 0)
                            {
                                currentChar *= 16;
                            }
                            else
                            {
                                fprintf(gctFile, "%c", currentChar);
                                currentChar = 0;
                            }
                        }
                    }
                }
            }
            else if ((currentChar != 0) && (vaildCharCount != 0))
                isComment = 1;
            currentChar = getc(codesFile);
        }
        if (vaildCharCount == 16 && !isComment)
        {
            currentChar = 0;
            for (i = 0; i < 16; i++)
            {
                if ((tempCode[i] >= 'a') && (tempCode[i] <= 'f'))
                    currentChar += 10 + tempCode[i] - 'a';
                if ((tempCode[i] >= 'A') && (tempCode[i] <= 'F'))
                    currentChar += 10 + tempCode[i] - 'A';
                if ((tempCode[i] >= '0') && (tempCode[i] <= '9'))
                    currentChar += tempCode[i] - '0';
                if (i % 2 == 0)
                {
                    currentChar *= 16;
                }
                else
                {
                    fprintf(gctFile, "%c", currentChar);
                    currentChar = 0;
                }
            }
        }
        fprintf(gctFile, "%c%c%c%c%c%c%c%c", 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        fclose(codesFile);
        fclose(gctFile);
*/
    
}

