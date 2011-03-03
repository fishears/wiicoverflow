#include "gameinfo.h"

#include "gfx.h"

extern s_path dynPath;

//Code taken for the most part from USB Loader GX... Thanks!

int GetGameInfo(unsigned char* ID)
{
	//TODO Make this configurable...

    char pathname[100];
	//snprintf(pathname, sizeof(pathname), "%s%s", "SD:/usb-loader/", "wiitdb.zip");
	snprintf(pathname, sizeof(pathname), "%s/wiitdb.zip", dynPath.dir_usb_loader);

    
								
    // database should always be zipped, while it may be convenient detecting and loading from .xml may lead to confusion:
    int fileexists = OpenXMLFile(pathname);

    if(!fileexists) {
	   //snprintf(pathname, sizeof(pathname), "%s%s", "SD:/usb-loader/", "wiitdb.xml");
 	   snprintf(pathname, sizeof(pathname), "%s/wiitdb.xml", dynPath.dir_usb_loader);
       fileexists = OpenXMLFile(pathname);
    }

	
	if(fileexists)
	{

		//Load Game Info
        int ret = LoadGameInfoFromXML((char*)ID);
		//FreeXMLMemory();
		
		//gameinfo struct (see xml.h) now contains the information for the game
		// Use this data from calling function like this:
		
		
		
		/*

		//top of file:
		extern struct gameXMLinfo gameinfo;

        # of players
        if (strcmp(gameinfo.players,"") != 0) {
                playersImgData = new GuiImageData(Wiimote1_png);
                if (atoi(gameinfo.players)>1){
                        1 player
                if (atoi(gameinfo.players)>2){
                        //2 players

        }
		*/
		
		return ret;
	}
	else
	{
		//wiitdb not found; or some other error
		//FreeXMLMemory();
		return 0;
	}

}
