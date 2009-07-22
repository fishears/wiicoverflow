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
    // call with mode=1 to suppress messages, mode=0 for verbose mode
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
    //calls download_txt with every titleID in the list
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
    //TODO: save a txt file containing a list of cheat lines enabled
    //TODO: parse back in if found so enabled cheats are remembered

    WPAD_Rumble(0,0); //sometimes rumble remain active
    CHEAT cheat;
    char buffer[128]; //dummy line for tests
    char lastline[128]; //hold the game name (which also appears at end of file)
    char filename[10];
    char titleID[7];
    char path[50];
    int i, codecounter = 0,ret;
    bool lastiscode = false;
    bool tamper = false;
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
            //WindowPrompt("lastline",lastline,&okButton,0);
        while(!feof(txtfile) && strcmp(lastline,buffer)!=0 && i<MAX_CHEATS) //parse the rest of the txt file
        {
            fgets(buffer,sizeof(buffer),txtfile); //get a line into buffer
            lastiscode = false;
            ret=is_code(buffer);
            if(ret == 0 && strlen(buffer)!=1) //if its not a code and not a blank line
            {
                memset(cheat[i].title, 0, LINE_LENGTH);
                sprintf(cheat[i].title,buffer); //write a title line
                if(strlen(buffer)>LINE_LENGTH)
                    cheat[i].title[LINE_LENGTH-1]='\0'; //get rid of the end of line
                else
                    cheat[i].title[strlen(cheat[i].title)-1]='\0';
                    //WindowPrompt("title1",cheat[i].title,&okButton,0);
                cheat[i].codelines = 0; //set new title codelines to zero
                cheat[i].editable = false;
                if(i>0) //only write codelines if this isn't the first title
                {
                    cheat[i-1].codelines = codecounter; //write the number of codelines for the previous title
                    if(cheat[i-1].codelines<1) //if previous title has no codelines then it wasn't a title after all
                    {
                            i--;
                            memset(cheat[i].title, 0, LINE_LENGTH);
                            sprintf(cheat[i].title,buffer); //write THIS title over THAT title
                            if(strlen(buffer)>LINE_LENGTH)
                                cheat[i].title[LINE_LENGTH-1]='\0'; //get rid of the end of line
                            else
                                cheat[i].title[strlen(cheat[i].title)-1]='\0';
                                //WindowPrompt("title2",cheat[i].title,&okButton,0);
                            cheat[i].codelines = 0; //set new title codelines to zero
                            cheat[i].editable = false;
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
                    if(cheat[i-1].editable==false && ret == 2)
                        cheat[i-1].editable=true;

                    cheat[i-1].codelines = codecounter; //write the number of codelines
                    lastiscode = true;
            }
            memset(buffer, 0, 128);
        }
        memset(buffer, 0, 128);
        memset(&lastline,0,128);
        if(lastiscode) //we reached feof but the last line was a code NOT the game title
        {
            i++;
        }

        char tTemp[LINE_LENGTH];
        int pages = 0;
        int currpage = 1;
        int display;
        int n;
        int maxlines = i;
        for(n=0;n<maxlines;n++) //set all lines to disabled
        {
            cheat[n].enabled=false;
        }
        //get stored cheat status
        char ming[4];
        sprintf(titleID,"%s",header->id);
        sprintf(filename, "%sx.txt", titleID);
        chdir("/");
        sprintf(path,"%s%s",USBLOADER_PATH,TXT_PATH);
        chdir(path);
        FILE *txtfile=NULL;
        txtfile = fopen(filename, "r");
        if(txtfile)
        {
            int n;
            while(!feof(txtfile))
            {
                fgets(ming,4,txtfile);
                n=atoi(ming);
                cheat[n].enabled =true;
            }
        }
        fclose(txtfile);
        for(n=0;n<LINES_PER_PAGE;n++) //create the buttons
        {
            Duplicate_Button(&cheatEnabled[n], cheatEnabled[0],84,148+(n*28));
            Duplicate_Button(&cheatDisabled[n],cheatDisabled[0],84,148+(n*28));
            Duplicate_Button(&cheatEditButton[n],cheatEditButton[0],108,148+(n*28));
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
                   if(cheat[n].enabled==true  && tamper) //check for any enabled cheats to use, & have we changed any
                   {
                    if(WindowPrompt(TX.ocarina, TX.useCodes, &yesButton, &noButton))
                       {
                            create_gct(cheat, maxlines, gameList, id, maxlines); //go and make the gct file for the enabled cheats
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
                    {
                        cheat[buttcheck].enabled = (cheat[buttcheck].enabled) ? false : true;
                        tamper = true;
                    }
                    else if(Button_Select(&cheatEditButton[n], pointer.p_x, pointer.p_y) && cheat[buttcheck].editable==true)
                        edit_codes(cheat,buttcheck);
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
                      if(cheat[n].enabled==true && tamper) //check for any enabled cheats to use, and have we changed any
                      {
                           if(WindowPrompt(TX.ocarina, TX.useCodes,&yesButton,&noButton))
                            {
                                create_gct(cheat, maxlines, gameList, id, maxlines); //go and make the gct file for the enabled cheats
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
                        tamper = true;
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
                    memset(tTemp,0,LINE_LENGTH);
                    sprintf(tTemp,"%s",cheat[display].title);
                    //sprintf(tTemp,"%s:%d",cheat[display].title, cheat[display].codelines);
                    
                    CFreeTypeGX_DrawText(ttf14pt, 135, 164+step, tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
                    step +=28;
                    if(cheat[display].editable)
                        Button_Paint(&cheatEditButton[i]);
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
            for(n=0;n<=display;n++) //hover the selection buttons
            {
                if(cheat[n+((currpage-1)*LINES_PER_PAGE)].editable)
                    Button_Hover(&cheatEditButton[n], pointer.p_x, pointer.p_y);
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
int is_code(char* line)
{
    //checks the line to see if it's a code line
    //returns
    //0 if NOT a codeline
    //1 if it is a codeline
    //2 if it is an editable codeline
    if(strlen(line)>16) //don't mess about if it's just too short to be a code then it't NOT a code
    {
        char tempCode[17];
        int x;
        memset(tempCode, 0, sizeof(tempCode));
        bool checkFlag = true, editable = false;
        char* pch;
        char* msg = malloc(strlen(line)*sizeof(char));
        sprintf(msg, line);
        pch = strtok(msg, " ");
        strcat(tempCode,pch);
        while(pch!=NULL)
        {
            if(strlen(pch) == 8) //test for a block of 8 characters to SPACE
            {
                //have we REALLY got a code line??
                for (x = 0; x < 8; x++) //check for characters outside the scope of a pukka code in 1st block
                {
                    if (((tempCode[x] >= 'g') && (tempCode[x] <= 'z')) ||((tempCode[x] >= 'G') && (tempCode[x] <= 'Z')))
                    {
                        //exclude known code variables
                        //if(tempCode[x] !='x' && tempCode[x] !='X' && tempCode[x] !='R' && tempCode[x] !='G' && tempCode[x] !='Y' && tempCode[x] !='y')
                            checkFlag=false;
                    }
                }
                
                pch = strtok(NULL," \r\n");
                if(strlen(pch)==8) //test for second block of 8 characters to SPACE or NEWLINE
                {
                    strcat(tempCode,pch);
                    for (x = 0; x < 8; x++) //check for characters outside the scope of a pukka code
                    {
                        //known code variables
                        if(pch[x] =='x' || pch[x] =='X' || pch[x] =='R' || pch[x] =='G' || pch[x] =='Y' || pch[x] =='y' 
                                || (pch[x-1] =='b' && pch[x] =='b') || (pch[x] =='b' && pch[x+1] =='b') //sneaky B
                                || (pch[x-1] =='B' && pch[x] =='B') || (pch[x] =='B' && pch[x+1] =='B')) // B is used in LOZTP
                            editable = true;
                    }
                    if(checkFlag)
                    {
                        free(msg);
                        if(editable)
                        {
                            return 2; //it's a code line (editable)
                        }
                        else
                        {
                            return 1; //it's a code line (not editable)
                        }
                    }
                }
            }
            free(msg);
            return 0;
        }
    }
    return 0;
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

void create_gct(CHEAT cheat,int cheatcount, struct discHdr *gameList, int id, int cheatCount)
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
                    char* msg = malloc(strlen(cheat[i].codes[n])*sizeof(char));
                    sprintf(msg, cheat[i].codes[n]);

                    pch = strtok(msg, " |\r\n"); //chomp out the space from the middle
                    //while (pch != NULL)
                    //{
                            strcat(tempCode,pch); //part one
                            pch  = strtok(NULL, " |\r\n");
                            strcat(tempCode,pch); //part two
                    //}
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
                            //set known code variables to zero until they can be edited in a future release
                            if(tempCode[x] =='x' || tempCode[x] =='X' || tempCode[x] =='R' || tempCode[x] =='G' || tempCode[x] =='Y' || tempCode[x] =='y')
                                currentChar = 0;
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
    //store enabled cheat data
    char path[50];
    char filename[11];
    char ming[3];
    sprintf(titleID,"%s",header->id);
    sprintf(filename, "%sx.txt", titleID);
    chdir("/");
    sprintf(path,"%s%s",USBLOADER_PATH,TXT_PATH);
    chdir(path);
    FILE *txtfile=NULL;
    txtfile = fopen(filename, "w");

    for(n=0;n<cheatCount;n++)
    {
        if(cheat[n].enabled ==true)
        {
            sprintf(ming,"%d\n",n);
            fputs(ming,txtfile);
        }
    }
    fclose(txtfile);
    return;
    

}

void edit_codes(CHEAT cheat, int cheatNum)
{
    //allows cheat lines with variables to be edited and saved for the current cheat
    //TODO add hex keyboard
    int currpage = 1;
    int pages=0;
    int i,n;
    char tTemp[128];
    if(cheat[cheatNum].codelines<=LINES_PER_PAGE)
        pages = 1;
    else
        pages = (cheat[cheatNum].codelines/LINES_PER_PAGE)+1;

    int ret,count=0;
    for(i=0;i<cheat[cheatNum].codelines;i++)
    {
        ret=is_code(cheat[cheatNum].codes[i]);
        if(ret==2) //line IS a cheat code and IS editable
        {
            Duplicate_Button(&varEditButton[i],varEditButton[0],108,148+(count*28)); //make the buttons
            count++;
        }
    }
    while(1) //edit codes GUI loop
        {
            WPAD_ScanPads();
            GetWiimoteData();
            if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)) //b or home to exit
            {
                for(i=0;i<cheat[cheatNum].codelines;i++)
                {
                    ret=is_code(cheat[cheatNum].codes[i]);
                    if(ret==2) //line IS a cheat code and IS editable
                    {
                        FreeButtonResources(&varEditButton[i]);
                    }
                }
                return;
            }
            else if((WPAD_ButtonsDown(0) & WPAD_BUTTON_A)||(PAD_ButtonsDown(0) & PAD_BUTTON_A))
            {
                if(Button_Select(&varDoneButton, pointer.p_x, pointer.p_y))
                {
                    pointer.p_x = 0;
                    return;
                }
                for(n=0;n<cheat[cheatNum].codelines;n++) //test the cheat buttons
                {
                    if(Button_Select(&varEditButton[n], pointer.p_x, pointer.p_y) && is_code(cheat[cheatNum].codes[n])==2)
                    {
                        sprintf(tTemp,"%s",cheat[cheatNum].codes[n]);
                        tTemp[17]='\0';
                        edit_variables(tTemp);
                    }
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
            CFreeTypeGX_DrawText(ttf18pt, 320, 100, "Edit Variables", (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_CENTER);
            CFreeTypeGX_DrawText(ttf14pt, 320, 136, cheat[cheatNum].title, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_CENTER);
            if(pages>1)
            {
                sprintf(tTemp,"%d/%d",currpage,pages); //report page x of xx
                CFreeTypeGX_DrawText(ttf14pt, 520, 100, tTemp, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_JUSTIFY_LEFT);
            }
            for(i=0;i<cheat[cheatNum].codelines;i++)
            {
                ret=is_code(cheat[cheatNum].codes[i]);
                if(ret==2) //line IS a cheat code and IS editable
                {
                    Button_Paint(&varEditButton[i]);
                    Button_Hover(&varEditButton[i], pointer.p_x, pointer.p_y);
                    sprintf(tTemp,"%s",cheat[cheatNum].codes[i]);
                    tTemp[17]='\0';
                    CFreeTypeGX_DrawText(ttf14pt, 135, 164+step,tTemp, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
                    step +=28;
                }
            }
            if(pages>1)
            {
                Button_Paint(&pageUpButton);
                Button_Paint(&pageDownButton);
                Button_Hover(&pageDownButton, pointer.p_x, pointer.p_y);
                Button_Hover(&pageUpButton, pointer.p_x, pointer.p_y);
            }
            Button_TTF_Paint(&varDoneButton);
            Button_Hover(&varDoneButton, pointer.p_x, pointer.p_y);
            DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF); //HAND!
            GRRLIB_Render();
        }
    return;
}

void edit_variables(char* line)
{
    //lets you edit the variable values found in the codeline
    //char hexlist[17] = {'X','0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    int editable[8] = {0,0,0,0,0,0,0,0}; //holds 1 if character is editable
    int x;
    char editline[8]; //holds the second block of codeline (the bit with variables)
    char* screen;
    char* pch;
    char* msg = malloc(strlen(line)*sizeof(char));
    sprintf(msg, line);
    pch = strtok(msg, " "); //locate & ignore the first 8 chars of the codeline
    pch = strtok(NULL," \r\n\0");
    if(strlen(pch)==8) //test for second block of 8 characters to SPACE or NEWLINE
    {
        strncpy(editline,pch,8);
        for (x = 0; x < 8; x++)
        {
            //look for known code variables
            if(pch[x] =='x' || pch[x] =='X' || pch[x] =='R' || pch[x] =='G' || pch[x] =='Y' || pch[x] =='y'
                    || (pch[x-1] =='b' && pch[x] =='b') || (pch[x] =='b' && pch[x+1] =='b') //sneaky B
                    || (pch[x-1] =='B' && pch[x] =='B') || (pch[x] =='B' && pch[x+1] =='B')) // B is used in LOZTP
                editable[x]=1; //found a variable so store it's position
        }
    }
    free(msg);
    WindowPrompt("EDIT Coming Soon",editline,&okButton,0);
    return;

    while(1) //edit variables GUI loop
    {
        WPAD_ScanPads();
        GetWiimoteData();
        if((WPAD_ButtonsDown(0) & WPAD_BUTTON_B) || (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)) //b or home to exit
        {
            return;
        }
        else if((WPAD_ButtonsDown(0) & WPAD_BUTTON_A)||(PAD_ButtonsDown(0) & PAD_BUTTON_A))
        {
             return;
        }

        draw_covers();
        GRRLIB_Rectangle(150, 156, 350, 206, 0xffffffdd, true); //draw a big boring box
        GRRLIB_Rectangle(152, 158, 352, 202, 0x737373FF, true);

        int step = 0;
        for(x=0;x<8;x++)
        {
            sprintf(screen,"%c",editline[x]);
            CFreeTypeGX_DrawText(ttf14pt, 210+step, 190,screen, (GXColor){0x00, 0x00, 0x00, 0xff}, FTGX_JUSTIFY_LEFT);
            step +=24;
        }


        DrawCursor(0, pointer.p_x, pointer.p_y, pointer.p_ang, 1, 1, 0xFFFFFFFF); //HAND!
        GRRLIB_Render();
    }
    return;
}
