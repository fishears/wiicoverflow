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
    //check to see if the game has a txt cheat file
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
    //check to see if the game has a gct cheat file
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
    //attempt to download the game's txt cheat file from www.usbgecko.com
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
        if(file.data != NULL) // if we got data back, save that sucka
        {
            WindowPrompt(titleID,"txt codes downloaded",&okButton,0);
            sprintf(imgpath,"%s%s%s.txt",USBLOADER_PATH,TXT_PATH,titleID);
            saveFile(imgpath, file);
            free(file.data);
            return true;
        }
        else //no data so report the HTTP error
        {
//            if(file.size<204)
//                sprintf(file.error,"Cheats not available");
            WindowPrompt("HTTP ERROR",file.error,&okButton,0);
            return false;
        }
    }
    //network failed to initialize
    WindowPrompt("ERROR",TX.errEstablishConn,&okButton,0);
    return false;
}

void manage_cheats(int id, struct discHdr *gameList)
{
    //parses the txt file and allows user to enable/disable cheats
    //then turns enabled codes into a gct file to be used with ocarina
    CHEAT cheat[MAX_CHEATS]; //set an ambitious limit to the number of cheats for each game
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
                cheat[i-1].enabled = false;
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

        char tTemp[135];
        int pages = 0;
        int currpage = 1;
        int display;
        int n;
        for(n=0;n<LINES_PER_PAGE;n++) //create the buttons
        {
            cheatEnabled[n] = Duplicate_Button(cheatEnabled[0],44,64+(n*28));
            cheatDisabled[n] = Duplicate_Button(cheatDisabled[0],44,64+(n*28));
        }
        fclose(txtfile);
        pages = (i/LINES_PER_PAGE)+1;

        while(1) //cheat manager GUI loop
        {
            WPAD_ScanPads();
            GetWiimoteData();
            if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)) //b or home to exit
            {
                //int x;
                //for(x=0;x<MAX_CHEATS;x++)
                //{
                //    memset(&cheat[x], 0, sizeof(cheat[x])); //clear the cheats
                //}
                return;
            }
            else if((WPAD_ButtonsDown(0) & WPAD_BUTTON_A)||(PAD_ButtonsDown(0) & PAD_BUTTON_A))
            {
                int buttcheck; //great name for a variable!
                for(n=0;n<LINES_PER_PAGE;n++) //test the cheat buttons
                {
                    buttcheck = n+((currpage-1)*LINES_PER_PAGE); //so that we check the right cheat status
                    if(Button_Select(&cheatEnabled[n], pointer.p_x, pointer.p_y) || Button_Select(&cheatDisabled[n], pointer.p_x, pointer.p_y))
                        cheat[buttcheck].enabled = (cheat[buttcheck].enabled) ? false : true;
                }
            }
            else if((WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS)) //page forward
            {
                if(currpage<pages)
                    currpage++;
                else
                    currpage = 1;
            }
            else if((WPAD_ButtonsDown(0) & WPAD_BUTTON_MINUS)) //page back
            {
                if(currpage>1)
                    currpage--;
                else
                    currpage = pages;
            }

        draw_covers();
        GRRLIB_Rectangle(40, 56, 560, 376, 0xffffffdd, true); //draw a big boring box
        GRRLIB_Rectangle(42, 58, 556, 372, 0x737373FF, true);
        int step = 0;
        sprintf(tTemp,"%d/%d",currpage,pages); //report page x of xx
        CFreeTypeGX_DrawText(ttf14pt, 550, 80, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
        for(i=0;i<LINES_PER_PAGE;i++)
        {
            display = i+((currpage-1)*LINES_PER_PAGE);
            if(cheat[display].codelines >0) //got codes so it is a title
            {
                sprintf(tTemp,"%d:%s has %d codes",display,cheat[(display)].title,cheat[display].codelines);
                CFreeTypeGX_DrawText(ttf14pt, 90, 80+step, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
                step +=28;
                if(cheat[display].enabled) //paint the appropriate cheat button state
                    Button_Toggle_Paint(&cheatEnabled[i],&cheatDisabled[i],0);
                else
                    Button_Toggle_Paint(&cheatEnabled[i],&cheatDisabled[i],1);
            }
        }
        for(n=0;n<display;n++) //hover the buttons
        {
            Button_Hover(&cheatEnabled[n],cheatEnabled[n].x,cheatEnabled[n].y);
            Button_Hover(&cheatDisabled[n],cheatDisabled[n].x,cheatDisabled[n].y);
        }
        DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF); //HAND!
        GRRLIB_Render();
        }
    }
    memset(&cheat, 0, sizeof(cheat));
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
    //checks the line to see if it's a code line
    char* pch;
    char* msg = malloc(strlen(line)*sizeof(char));
    sprintf(msg, line);
    pch = strtok(msg, " ");
    while(pch!=NULL)
    {
        if(strlen(pch) == 8) //test for a block of 8 characters to SPACE
        {
            pch = strtok(NULL,"\n");
            if(strlen(pch)==8) //test for second block of 8 characters to NEWLINE
            {
                free(msg);
                return true; //it's a code line (or a good copy)
            }
        }
        free(msg);
        return false; 
    }
    return false;
}

