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
            //WindowPrompt(filename,"txt codes found on SD",&okButton,0);
            fclose(txtfile);
            return true;
    }
    else
    {
        //WindowPrompt(filename,"txt codes not found",&okButton,0);
        return false;
    }
}

bool check_gct(int id, struct discHdr *gameList)
{
    //check to see if the game has a gct cheat file
    char filename[10];
    char titleID[7];
    struct discHdr *header = &gameList[id];
    sprintf(titleID,"%s",header->id);
    sprintf(filename, "%s.gct", titleID);
    chdir("/");
    chdir(GCT_PATH);
    FILE *txtfile;
    txtfile = fopen(filename, "r");
    if(txtfile)
    {
        //WindowPrompt(filename,"gct file found",&okButton,0);
        fclose(txtfile);
        return true;
    }
    else
    {
        //WindowPrompt(filename,"gct file not found",&okButton,0);
        return false;
    }
}

bool download_txt(int id, int mode, struct discHdr *gameList)
{
    //attempt to download the game's txt cheat file from www.usbgecko.com
    //TODO tidy this up a bit
    //TODO add a batch download function like for covers
    //TODO get the 4-digit bit to kick in after a bad download on 6-digit not just after NULL data
    if(mode==0)
    {
        if(!(WindowPrompt(TX.downloadB, TX.downloadCheatFile,&okButton, &cancelButton)))
            {
                    return false;
            }
    }
    if(check_write_access())
    {
        if(networkInit(self.ipAddress)){
            char url[100];
            char titleID[7];
            char imgpath[50];
            struct block file;
            struct discHdr *header = &gameList[id];
            sprintf(titleID,"%s",header->id);
            if(mode==0)
                WindowPrompt(titleID, TX.pleaseWait, 0, 0);
            sprintf(url, "%s%c/%s.txt", CODESITE, header->id[0] , titleID); //try 6-digit ID first
            file = downloadfile(url);
            if(file.data == NULL) //nothing, so try 4-digit ID
            {
                sprintf(url, "%s%c/%c%c%c%c.txt", CODESITE, header->id[0], header->id[0], header->id[1],header->id[2],header->id[3]);
                file = downloadfile(url);
            }
            if(file.data != NULL) // if we got data back, save that sucka
            {
                sprintf(imgpath,"%s%s%s.txt",USBLOADER_PATH,TXT_PATH,titleID);
                saveFile(imgpath, file);
                free(file.data);
                if(check_download(titleID))
                {
                    if(mode==0)
                        WindowPrompt(titleID, TX.downloadComplete, &okButton,0);
                    return true;
                }
                else
                {
                    if(mode==0)
                        WindowPrompt(TX.error, TX.noTxtCodes, &okButton,0);
                    return false;
                }
            }
            else //no data so report the HTTP error
            {
                WindowPrompt(TX.errorHTTP, file.error, &okButton,0);
                return false;
            }
        }
    }
    return false;
}

void batch_download_txt(struct discHdr *gameList)
{
	int i;
	char id[7];

	for(i = 0; i < self.gameCnt; i++)
	{
		struct discHdr *header = &gameList[i];

		if(self.array_size < MAX_COVERS)
		{
			sprintf(id, "%s", header->id);
			sprintf(self.debugMsg, "Check next cheat: %s", header->id);
			Paint_Progress_Generic(i, self.gameCnt, self.debugMsg);
			download_txt(i, 1, self.gameList);
		}
	}
	WindowPrompt (TX.opFinished, TX.pressA, &okButton, 0);
}

 #ifdef CHEAT_MANAGER
void manage_cheats(int id, struct discHdr *gameList)
{
    //parses the txt file and allows user to enable/disable cheats
    //then turns enabled codes into a gct file to be used with ocarina
    //TODO: editable codelines, viewable comment lines, heading lines... so much work!!!
    CHEAT cheat;
    char buffer[128]; //dummy line for tests
    char lastline[128]; //hold the game name (which also appears at end of file)
    char filename[10];
    char titleID[7];
    char path[50];
    int i, codecounter = 0;
    struct discHdr *header = &gameList[id];
    sprintf(titleID,"%s",header->id);
    sprintf(filename, "%s.txt", titleID);
    chdir("/");
    sprintf(path,"%s%s",USBLOADER_PATH,TXT_PATH);
    chdir(path);
    FILE *txtfile=NULL;
    txtfile = fopen(filename, "r");
	
    if(txtfile)
    {
        fgets (buffer, sizeof buffer, txtfile ); //discard 1st line -> titleID
        memset(buffer, 0, sizeof(buffer));
        memset(lastline,0,sizeof lastline);
        fgets(lastline, sizeof lastline, txtfile); //keep the 2nd line for testing against last line
        fgets (buffer, sizeof buffer, txtfile ); //discard 3rd line -> ""
        memset(buffer, 0, sizeof(buffer));
        i = 0;
        while(!feof(txtfile) && strcmp(lastline,buffer)!=0 && i<MAX_CHEATS) //parse the rest of the txt file
        {
            fgets(buffer,sizeof(buffer),txtfile); //get a line into buffer
            if(!is_code(buffer) && strlen(buffer)!=1) //if its not a code and not a blank line
            {
                memset(cheat[i].title, 0, LINE_LENGTH);
                buffer[strlen(buffer)-1]='\0'; //get rid of the end of line
                sprintf(cheat[i].title,buffer); //write a title line
                    //WindowPrompt("title1",cheat[i].title,&okButton,0);
                
                cheat[i].codelines = 0; //set new title codelines to zero
                if(i>0) //only write codelines if this isn't the first title
                {
                        cheat[i-1].codelines = codecounter; //write the number of codelines for the previous title
                        cheat[i-1].enabled = false;

                        if(cheat[i-1].codelines<1) //if previous title has no codelines then it wasn't a title after all
                        {
                                i--;
                                memset(cheat[i].title, 0, LINE_LENGTH);
                                sprintf(cheat[i].title,buffer); //write THIS title over THAT title
                                    //WindowPrompt("title2",cheat[i].title,&okButton,0);
                                cheat[i].codelines = 0; //set new title codelines to zero
                        }
                }
                codecounter = 0; //reset the codecounter
                i++;
            }
            else if(strlen(buffer)!=1)//it must be a code
            {
                    sprintf(cheat[i-1].codes[codecounter],"%18s",buffer); //write the codeline
                    //WindowPrompt("code",cheat[i-1].codes[codecounter],&okButton,0);
                    codecounter++; //we got another codeline
                    cheat[i-1].codelines = codecounter; //write the number of codelines
            }
            memset(buffer, 0, 128);
        }
        memset(&lastline,0,LINE_LENGTH);

        char tTemp[135];
        int pages = 0;
        int currpage = 1;
        int display;
        int n;
        int maxlines = i;
        for(n=0;n<LINES_PER_PAGE;n++) //create the buttons
        {
            cheatEnabled[n] = Duplicate_Button(cheatEnabled[0],84,148+(n*28));
            cheatDisabled[n] = Duplicate_Button(cheatDisabled[0],84,148+(n*28));
        }
        fclose(txtfile);
        if(maxlines<=LINES_PER_PAGE)
            pages = 1;
        else
            pages = (maxlines/LINES_PER_PAGE)+1;
        PAGE page[pages]; //initialize the pages
        for(n=1;n<=pages;n++)
        {
            page[n].selectAll = false;
        }

        while(1) //cheat manager GUI loop
        {
            WPAD_ScanPads();
            GetWiimoteData();
            if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)) //b or home to exit
            {
                for(n=0;n<maxlines;n++)
                {
                   if(cheat[n].enabled==true) //check for any enabled cheats to use
                   {
                       if(WindowPrompt(TX.ocarina, TX.useCodes, &yesButton, &noButton))
                       {
                            create_gct(cheat, maxlines, gameList, id); //go and make the gct file for the enabled cheats
                       }
                       return;
                    }
                }
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
                 if(pages>1)
                 {
                    if(Button_Select(&pageUpButton, pointer.p_x, pointer.p_y))
                    {
                        if(currpage<pages)
                            currpage++;
                        else
                            currpage = 1;
                    }
                    else if(Button_Select(&pageDownButton, pointer.p_x, pointer.p_y))
                    {
                        if(currpage>1)
                            currpage--;
                        else
                            currpage = pages;
                    }
                }
                if(Button_Select(&cheatDoneButton, pointer.p_x, pointer.p_y))
                {
                   for(n=0;n<maxlines;n++)
                    {
                       if(cheat[n].enabled==true) //check for any enabled cheats to use
                       {
                           if(WindowPrompt(TX.ocarina, TX.useCodes,&yesButton,&noButton))
                           {
                                create_gct(cheat, maxlines, gameList, id); //go and make the gct file for the enabled cheats
                           }
                           return;
                        }
                    }
                    return;
                }
                else if(Button_Select(&selectAllButton,pointer.p_x,pointer.p_y) || Button_Select(&deselectAllButton,pointer.p_x,pointer.p_y))
                {
                    page[currpage].selectAll = page[currpage].selectAll ? false : true;
                    for(n=(currpage-1)*LINES_PER_PAGE;n<(currpage)*LINES_PER_PAGE && n<maxlines;n++)
                    {
                        cheat[n].enabled = page[currpage].selectAll;
                    }
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
            GRRLIB_Rectangle(70, 76, 500, 346, 0xffffffdd, true); //draw a big boring box
            GRRLIB_Rectangle(72, 78, 496, 342, 0x737373FF, true);
            int step = 0;
            CFreeTypeGX_DrawText(ttf18pt, 320, 100, "Cheat Manager", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);
            if(pages>1)
            {
                sprintf(tTemp,"%d/%d",currpage,pages); //report page x of xx
                CFreeTypeGX_DrawText(ttf14pt, 520, 100, tTemp, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
            }
            for(i=0;i<LINES_PER_PAGE;i++)
            {
                display = i+((currpage-1)*LINES_PER_PAGE);
                if(display < (maxlines)-1) //only show up to the number of lines available
                {
                    //sprintf(tTemp,"%s:%d",cheat[display].title, cheat[display].codelines);
                    sprintf(tTemp,"%s",cheat[display].title);
                    CFreeTypeGX_DrawText(ttf14pt, 130, 164+step, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
                    step +=28;
                    if(cheat[display].enabled) //paint the appropriate cheat button state
                        Button_Toggle_Paint(&cheatEnabled[i],&cheatDisabled[i],0);
                    else
                        Button_Toggle_Paint(&cheatEnabled[i],&cheatDisabled[i],1);
                }
            }
            if(page[currpage].selectAll) //paint the "de/select all" button
                Button_Toggle_Paint(&selectAllButton,&deselectAllButton,1);
            else
                Button_Toggle_Paint(&selectAllButton,&deselectAllButton,0);
            Button_Hover(&selectAllButton,pointer.p_x,pointer.p_y);
            Button_Hover(&deselectAllButton,pointer.p_x,pointer.p_y);
            Button_TTF_Paint(&cheatDoneButton);
            if(pages>1)
            {
                Button_Paint(&pageUpButton);
                Button_Paint(&pageDownButton);
                Button_Hover(&pageDownButton, pointer.p_x, pointer.p_y);
                Button_Hover(&pageUpButton, pointer.p_x, pointer.p_y);
            }
            Button_Hover(&cheatDoneButton, pointer.p_x, pointer.p_y);
            for(n=0;n<display;n++) //hover the selection buttons
            {
                Button_Hover(&cheatEnabled[n], pointer.p_x, pointer.p_y);
                Button_Hover(&cheatDisabled[n], pointer.p_x, pointer.p_y);
            }
            DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF); //HAND!
            GRRLIB_Render();
        }
    }

    return;    
}
#endif
bool is_code(char* line)
{
    //checks the line to see if it's a code line
    //TODO: parse the code at this stage to tag it as having editable values if any non-hex chars found
    if(strlen(line)>16) //don't mess about if it's just too short to be a code then it't NOT a code
    {
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
    }
    return false;
}

bool check_download(char* titleID)
{
    //checks txt file to see if it's valid
    char buffer[128];
    char filename[20];
    char path [100];
    chdir("/");
    sprintf(path,"%s%s",USBLOADER_PATH,TXT_PATH);
    chdir(path);
    sprintf(filename, "%s.txt", titleID);
    FILE *txtfile;
    txtfile = fopen(filename, "r");
    fgets( buffer, sizeof buffer, txtfile );
    char* pch;
    char* msg = malloc(strlen(buffer)*sizeof(char));
    sprintf(msg, buffer);
    pch = strtok(msg, " ");
    while(pch!=NULL)
    {
        if(strcmp(pch,"<!DOCTYPE")==0) //test for a bad file
        {
            remove(filename); //it's bad so delete it
			free(msg);
            return false;
        }
        else
        {
            fclose(txtfile); //it's good so close it
			free(msg);
            return true;
        }
    }
    fclose(txtfile); //it's good so close it
	free(msg);
    return true;
}

void create_gct(CHEAT cheat,int cheatcount, struct discHdr *gameList, int id)
{
    //this is where we parse the selected cheat data into a gct file for ocarina
    //char to hex by brkirch
    char gctname[10];
    char titleID[7];
    struct discHdr *header = &gameList[id];
    sprintf(titleID,"%s",header->id);
    sprintf(gctname,"%s.gct",titleID);
    chdir("/");
    chdir(GCT_PATH);
    FILE *gctFile;
    gctFile = fopen(gctname,"wb");
    if(!gctFile)
    {
            WindowPrompt(TX.error,TX.errOpenGCT, &okButton,0);
            return;
    }
    char tempCode[17];
    memset(tempCode, 0, sizeof(tempCode));
    int i,n;
    fprintf(gctFile, "%c%c%c%c%c%c%c%c", 0x00, 0xD0, 0xC0, 0xDE, 0x00, 0xD0, 0xC0, 0xDE); //gct header

    for(i=0;i<cheatcount;i++) //main loop for checking each cheat
    {
        if(cheat[i].enabled) //if this cheat is enabled...
        {
            for(n=0;n<cheat[i].codelines;n++) //sub loop for getting the enabled codes
            {
                if(cheat[i].codes[n] !=NULL)
                {
                    int x;
                    char* pch;
                    //sprintf(tempCode,cheat[i].codes[n]);
                    char* msg = malloc(strlen(cheat[i].codes[n])*sizeof(char));
                    sprintf(msg, cheat[i].codes[n]);

                    pch = strtok(msg, " |\n"); //chomp out the space from the middle
                    while (pch != NULL)
                    {
                            strcat(tempCode,pch);
                            pch  = strtok(NULL, " |\n");
                    }
                    free(msg);
                    //WindowPrompt("debug",tempCode, &okButton,0);
                    int currentChar = 0;
                    
                    for (x = 0; x < 16; x++) //HEXify the codeline
                    {
                            if ((tempCode[x] >= 'a') && (tempCode[x] <= 'f'))
                                currentChar += 10 + tempCode[x] - 'a';
                            if ((tempCode[x] >= 'A') && (tempCode[x] <= 'F'))
                                currentChar += 10 + tempCode[x] - 'A';
                            if ((tempCode[x] >= '0') && (tempCode[x] <= '9'))
                                currentChar += tempCode[x] - '0';
                            if(tempCode[x] == 'X' || tempCode[x] == 'x')
                                currentChar += '0';
                            if (x % 2 == 0)
                            {
                                currentChar *= 16;
                            }
                            else
                            {
                                fprintf(gctFile, "%c", currentChar); //write out the HEXified character
                                currentChar = 0;
                            }
                    }
                }
                memset(tempCode, 0, sizeof(tempCode)); //clear the line
            }
        }
    }
    fprintf(gctFile, "%c%c%c%c%c%c%c%c", 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); //gct footer
    fclose(gctFile);
    return;
    

}

