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
extern s_pointer pointer;
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
        if(file.data) //nothing, so try 4-digit ID
        {
            sprintf(url, "%s%c/%C%c%c%c.txt", CODESITE, header->id[0], header->id[0], header->id[1],header->id[2],header->id[3]);
            file = downloadfile(url);
        }
        if(file.data != NULL)
        {
            WindowPrompt(titleID,"txt codes downloaded",&okButton,0);
            sprintf(imgpath,"%s%s%s.txt",USBLOADER_PATH,TXT_PATH,titleID);
            saveFile(imgpath, file);
            free(file.data);
            return true;
        }
        else
        {
//            if(file.size<204)
//                sprintf(file.error,"Cheats not available");
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
    CHEAT cheat[500]; //set an ambitious limit to the number of cheats for each game
    char buffer[LINE_LENGTH]; //dummy line for tests
    char filename[10];
    //char gctname[10];
    char titleID[7];
    char path[50];
    int i, codecounter = 0;;
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
            fgets (buffer, sizeof buffer, txtfile );
            memset(&buffer, 0, sizeof(buffer));
        }
        i = 0;
        while(!feof(txtfile)) //parse the rest of the txt file
        {
            fgets(buffer,sizeof buffer,txtfile); //get a line into buffer
            if(!is_code(buffer) && strlen(buffer)!=1) //if its not a code and not a blank line
            {
                sprintf(cheat[i].title,buffer); //write it as a title
                cheat[i-1].codelines = codecounter-1; //write the number of codelines for the previous title
                codecounter = 0; //reset the codecounter
                if(cheat[i-1].codelines > 0 || i==0) //if not first title && the previous title really WAS a title, move on
                    i++;
            }
            else
            {
                sprintf(cheat[i].codes[codecounter],buffer);
                codecounter++;
            }
            memset(&buffer, 0, sizeof(buffer));
        }


/*
        while(!feof(txtfile)) //read the rest of the txt file into buffer array
        {
            fgets(buffer[i],sizeof buffer[i],txtfile);
            if(strlen(buffer[i])==1) //take out any empty lines
                i--;
            i++;

        }
        bufflines = i;
        sprintf(tTemp,"%d",i)
*/
        char tTemp[135];
        int pages = 0;
        int currpage = 0;
;
        fclose(txtfile);
        pages = (i/LINES_PER_PAGE);


        
        while(1)
        {
            WPAD_ScanPads();
            GetWiimoteData();
            if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B))
            {
                return;
            }
            else if((WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS))
            {
                if(currpage<pages)
                    currpage++;
                else
                    currpage = 0;
            }
            else if((WPAD_ButtonsDown(0) & WPAD_BUTTON_MINUS))
            {
                if(currpage>0)
                    currpage--;
                else
                    currpage = pages;
            }

        draw_covers();
        GRRLIB_Rectangle(40, 56, 560, 376, 0xffffffdd, true); //draw a box
        GRRLIB_Rectangle(42, 58, 556, 372, 0x737373FF, true);
        int step = 20;
        sprintf(tTemp,"%d/%d",currpage,pages);
        CFreeTypeGX_DrawText(ttf14pt, 550, 80, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
        for(i=0;i<LINES_PER_PAGE;i++)
        {
            if(cheat[i+(currpage*10)].codelines >0) //got codes so it is a title
            {
                sprintf(tTemp,"%d:%s has %d codes",i+(currpage*10),cheat[i+(currpage*10)].title,cheat[i+(currpage*10)].codelines);
                CFreeTypeGX_DrawText(ttf14pt, 50, 60+step, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
                step +=28;
            }
        }
        DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF);
        GRRLIB_Render();
        }
    }
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

bool is_code(char* line)
{
    char* pch;
    char* msg = malloc(strlen(line)*sizeof(char));
    sprintf(msg, line);
    pch = strtok(msg, " ");
    while(pch!=NULL)
    {
        //WindowPrompt("true",pch,&okButton,0);
        if(strlen(pch) == 8) //test for a block of 8 characters
        {
            pch = strtok(NULL,"\n");
            if(strlen(pch)==8) //test for second block of 8 characters
            {
                free(msg);
                return true;
            }
        }
        free(msg);
        //WindowPrompt("false",pch,&okButton,0);
        return false;
    }
    return false;
}
