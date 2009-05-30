#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <ogcsys.h>
#include "fileParser.h"
#include "localization.h"

extern s_settings settings;
static char *cfg_name, *cfg_val;
char** languages;
char** vidmodes;



///////////////////////////////////
// Later set via cfg
char langFile[20] = "TestLanguage";
///////////////////////////////////

char *localStr ( char *CODE, char * defaultStr )
{
   return defaultStr;	
}


bool checkfile(char * path)
{
    FILE * f;
    f = fopen(path,"r");
    if(f)
	{
    fclose(f);
    return true;
    }
    fclose(f);
return false;
}

void languageDefault()
{
	int i;
	
	languages =  (char**)malloc(11*sizeof(char*));
	
	for (i=0;i<11;i++)
	{
	 languages[i] =  (char*)malloc(30*sizeof(char));
	}

	vidmodes =  (char**)malloc(6*sizeof(char*));
	
	for (i=0;i<6;i++)
	{
	 vidmodes[i] =  (char*)malloc(30*sizeof(char));
	}

	snprintf(TX.iniNet, sizeof(TX.iniNet), "Initializing Network");   					//M001
	snprintf(TX.iniNetErr, sizeof(TX.iniNetErr), "Error Initializing Network");   		//M002
	snprintf(TX.error, sizeof(TX.error), "ERROR!");   									//M003
	snprintf(TX.iniNetReady, sizeof(TX.iniNetReady), "Network Initialized");   		//M004
	strcopy(TX.checkPresence, "Checking presence of %s", sizeof(TX.checkPresence));
	strcopy(TX.noDownload, "%s present, not downloading", sizeof(TX.noDownload));
	strcopy(TX.getting, "Getting %s", sizeof(TX.getting));
	snprintf(TX.done, sizeof(TX.done), "done");   										//M008
	snprintf(TX.someErrors, sizeof(TX.someErrors), "some error occurred");   			//M009
	strcopy(TX.checkNextCover, "Checking next cover...%s", sizeof(TX.checkNextCover));
	snprintf(TX.opFinished, sizeof(TX.opFinished), "Operation finished!");   			//M011
	snprintf(TX.pressA, sizeof(TX.pressA), "Press A to continue");   					//M012
	snprintf(TX.configError, sizeof(TX.configError), "Configuration error, MAX_DNS_ENTRIES reached while the list is empty\n"); //M013
	snprintf(TX.errorsaveGL, sizeof(TX.errorsaveGL), "Can't save gamelist.xml");   	//M014
	snprintf(TX.errorcreateGL, sizeof(TX.errorcreateGL), "Cannot create gamelist file!");	//M015
	
	snprintf(TX.consoleDefault, sizeof(TX.consoleDefault), "Console Default");   		//M016
	snprintf(TX.Japanese, sizeof(TX.Japanese), "Japanese");   							//M017
	snprintf(TX.German, sizeof(TX.German),     "German");   							//M018
	snprintf(TX.English, sizeof(TX.English),   "English");   							//M019
	snprintf(TX.French, sizeof(TX.French),     "French");     							//M020
	snprintf(TX.Spanish, sizeof(TX.Spanish),   "Spanish");   							//M021
	snprintf(TX.Italian, sizeof(TX.Italian),   "Italian");   							//M022
	snprintf(TX.Dutch, sizeof(TX.Dutch),       "Dutch");   							//M023
	snprintf(TX.SChinese, sizeof(TX.SChinese), "S. Chinese");   						//M024
	snprintf(TX.TChinese, sizeof(TX.TChinese), "T. Chinese");   						//M025
	snprintf(TX.Korean, sizeof(TX.Korean),     "Korean");   							//M026
	snprintf(TX.gameDefault, sizeof(TX.gameDefault), "Game Default");   				//M027
	snprintf(TX.automatic, sizeof(TX.automatic),     "Automatic");   					//M028
	snprintf(TX.forcePAL50, sizeof(TX.forcePAL50),   "Force PAL50");   				//M029
	snprintf(TX.forcePAL60, sizeof(TX.forcePAL60),   "Force PAL60");   				//M030
	snprintf(TX.forceNTSC, sizeof(TX.forceNTSC),     "Force NTSC"); 
	
	snprintf(languages[0],  sizeof(TX.consoleDefault), "%s", TX.consoleDefault);   		
	snprintf(vidmodes[5],   sizeof(TX.consoleDefault), "%s", TX.consoleDefault);   		
	snprintf(languages[1],  sizeof(TX.Japanese),   "   %s",  TX.Japanese);  
	snprintf(languages[2],  sizeof(TX.English),    "    %s", TX.English);  
	snprintf(languages[3],  sizeof(TX.German),     "    %s", TX.German);  
	snprintf(languages[4],  sizeof(TX.French),     "    %s", TX.French);  
	snprintf(languages[5],  sizeof(TX.Spanish),    "    %s", TX.Spanish);  
	snprintf(languages[6],  sizeof(TX.Italian),    "    %s", TX.Italian);  
	snprintf(languages[7],  sizeof(TX.Dutch),      "     %s",TX.Dutch);  
	snprintf(languages[8],  sizeof(TX.SChinese),   "   %s",  TX.SChinese);  
	snprintf(languages[9],  sizeof(TX.TChinese),   "   %s",  TX.TChinese);  
	snprintf(languages[10], sizeof(TX.Korean),     "    %s", TX.Korean);  
	snprintf(vidmodes[0],   sizeof(TX.gameDefault),"  %s",   TX.gameDefault);  
	snprintf(vidmodes[1],   sizeof(TX.automatic),  "   %s",  TX.automatic);  
	snprintf(vidmodes[2],   sizeof(TX.forcePAL50), "   %s",  TX.forcePAL50);  
	snprintf(vidmodes[3],   sizeof(TX.forcePAL60), "   %s",  TX.forcePAL60);  
	snprintf(vidmodes[4],   sizeof(TX.forceNTSC),  "   %s",  TX.forceNTSC);  

	snprintf(TX.neverPlayed, sizeof(TX.neverPlayed), "Never played before");   		//M032
	strcopy(TX.played, "Played %s", sizeof(TX.played));
	strcopy(TX.size, "Size:    %.2fGB", sizeof(TX.size));
	strcopy(TX.installing, "Installing... (%0.2f%%) Time left: %d:%02d:%02d", sizeof(TX.installing));
	strcopy(TX.setting, "Settings", sizeof(TX.setting));
	snprintf(TX.hook, sizeof(TX.hook), "Hook:"); 										//M037
  	snprintf(TX.ocarina, sizeof(TX.ocarina), "Ocarina:"); 								//M135
	snprintf(TX.language, sizeof(TX.language), "Language:");   						//M038
	snprintf(TX.videoMode, sizeof(TX.videoMode), "Video mode:");   					//M039
	snprintf(TX.patchVIDTV, sizeof(TX.patchVIDTV), "VIDTV patch:");   					//M040
    strcopy(TX.errorConnect, "Connection error from net_read()  Errorcode: %i\n", sizeof(TX.errorConnect));
	strcopy(TX.URLnoBegin, "URL '%s' doesn't start with 'http://'\n", sizeof(TX.URLnoBegin));
	strcopy(TX.URLnoPath, "URL '%s' has no PATH part\n", sizeof(TX.URLnoPath));
	strcopy(TX.URLnoDomain, "No domain part in URL '%s'\n", sizeof(TX.URLnoDomain));
	strcopy(TX.errorDomain, "\ndomain %s could not be resolved", sizeof(TX.errorDomain));
	snprintf(TX.errEstablishConn, sizeof(TX.errEstablishConn), "Error establishing connection"); //M046
	snprintf(TX.HTTPnoFile, sizeof(TX.HTTPnoFile), "HTTP Response was without a file\n"); //M047	
	snprintf(TX.noMemCopy, sizeof(TX.noMemCopy),"No more memory to copy file from HTTP response\n"); //M048
	snprintf(TX.graphicSetting, sizeof(TX.graphicSetting), "Graphics Settings");   	//M049
	snprintf(TX.zoom, sizeof(TX.zoom), "Zoom:");   									//M050
	snprintf(TX.spacing, sizeof(TX.spacing), "Spacing:");   							//M051
	snprintf(TX.angle, sizeof(TX.angle), "Angle:");   									//M052
	snprintf(TX.falloff, sizeof(TX.falloff), "Falloff:");   									//M052	
	snprintf(TX.drawWindow, sizeof(TX.drawWindow), "Draw Window:" );   				//M053
	snprintf(TX.gameTitle, sizeof(TX.gameTitle), "Game Title:");   					//M054
	snprintf(TX.coverDownload, sizeof(TX.coverDownload), "Cover download");   			//M055
	snprintf(TX.opNoCancel, sizeof(TX.opNoCancel), "This operation can't be canceled, continue?"); //M056
	snprintf(TX.cflowSettings, sizeof(TX.cflowSettings), "Coverflow Settings");   		//M057
	snprintf(TX.graphics, sizeof(TX.graphics), "Graphics:");   						//M058
	snprintf(TX.missingCovers, sizeof(TX.missingCovers), "Missing Covers?:");   		//M059
	snprintf(TX.theme, sizeof(TX.theme), "Theme:");   									//M060
	snprintf(TX.oneClickLaunch, sizeof(TX.oneClickLaunch), "1-Click Launch:");			//M061
	snprintf(TX.rumble, sizeof(TX.rumble), "Rumble:");   								//M062
	snprintf(TX.sound, sizeof(TX.sound), "Sound:"); 									//M063
	snprintf(TX.installNewGame, sizeof(TX.installNewGame), "Install new Game?");   	//M064
	snprintf(TX.diskInDrive, sizeof(TX.diskInDrive), "Place disk in drive and hit ok."); //M065
	snprintf(TX.initDVD, sizeof(TX.initDVD), "Initializing DVD Drive");   				//M066
	snprintf(TX.pleaseWait, sizeof(TX.pleaseWait), "Please Wait..." );   				//M067
	snprintf(TX.errorReadDisc, sizeof(TX.errorReadDisc), "Error reading Disc");   		//M068
	snprintf(TX.errorOpenDisc, sizeof(TX.errorOpenDisc), "Could not open Disc" );   	//M069
	snprintf(TX.notWiiDisc, sizeof(TX.notWiiDisc), "Not a Wii Disc");   				//M070
	snprintf(TX.insertWiiDisc, sizeof(TX.insertWiiDisc), "Insert a Wii Disc!" );   	//M071
	snprintf(TX.alreadyInstalled, sizeof(TX.alreadyInstalled), "Game is already installed:"); //M072
	snprintf(TX.noHDD, sizeof(TX.noHDD), "No HDD found!");   							//M073
	strcopy(TX.installingGame, "Installing game %.2fGB:", sizeof(TX.installingGame));
	strcopy(TX.install, "Installing game %.2fGB:" , sizeof(TX.install));
	strcopy(TX.gameSize, "Game Size: %.2fGB", sizeof(TX.gameSize));
	strcopy(TX.gameSizeSpace, "Game Size: %.2fGB, Free Space: %.2fGB", sizeof(TX.gameSizeSpace));
	snprintf(TX.errorInstall, sizeof(TX.errorInstall), "Install error!");   			//M078
	snprintf(TX.successInstall, sizeof(TX.successInstall), "Successfully installed:"); //M079
	snprintf(TX.askDelete, sizeof(TX.askDelete), "Do you want to delete:" );   		//M080
	snprintf(TX.errorDelete, sizeof(TX.errorDelete),"Delete Failed.");   				//M081
	snprintf(TX.cantDelete, sizeof(TX.cantDelete), "Could not delete game.");   		//M082
	snprintf(TX.successDelete, sizeof(TX.successDelete), "Successfully deleted."); 	//M083
	snprintf(TX.pressOkContinue, sizeof(TX.pressOkContinue), "Press Ok to Continue."); //M084
	snprintf(TX.noLocalIP, sizeof(TX.noLocalIP), "Cannot get local IP address.");   	//M085
	snprintf(TX.addError, sizeof(TX.addError), "[+] ERROR:");   						//M086
	strcopy(TX.errorCIOS, "Custom IOS could not be loaded! (ret = %d)\n", sizeof(TX.errorCIOS));
	snprintf(TX.loadTextures, sizeof(TX.loadTextures), "Loading textures");   			//M088
	snprintf(TX.initUSB, sizeof(TX.initUSB), "Init USB");   							//M089
	snprintf(TX.errInitUSBFS, sizeof(TX.errInitUSBFS), "Cannot init USBFS, quitting.");//M090
	snprintf(TX.initWBFS, sizeof(TX.initWBFS), "Initializing WBFS");   				//M091
	snprintf(TX.errorNoUSBDrv, sizeof(TX.errorNoUSBDrv), "USB Error - Drive not found"); //M092
	snprintf(TX.pressAB, sizeof(TX.pressAB), "Press A to Retry, B to Exit");   		//M093
	snprintf(TX.connectUSBDrv, sizeof(TX.connectUSBDrv), "Attempt to connect to USB Drive"); //M094
	snprintf(TX.errorNoWBFS, sizeof(TX.errorNoWBFS), "USB Error - NO WBFS Parition."); //M095
	snprintf(TX.hold12B, sizeof(TX.hold12B), "Hold 1 And 2 to Format, B to Exit"); 	//M096
	snprintf(TX.findPartitions, sizeof(TX.findPartitions), "Finding Partitions...");   //M097
	strcopy(TX.partition, "Partition %d: %.2fGB", sizeof(TX.partition));
	strcopy(TX.partitionNoFormat, "Partition %d: (Can't be formatted)", sizeof(TX.partitionNoFormat));
	snprintf(TX.askFormat, sizeof(TX.askFormat), "Do you want to format:");   			//M100
	snprintf(TX.errorFormat, sizeof(TX.errorFormat), "Failed formatting!");   			//M101
	snprintf(TX.successFormat, sizeof(TX.successFormat), "Success:");   				//M102
	snprintf(TX.formatComplete, sizeof(TX.formatComplete), "Format Complete.");   		//M103
	snprintf(TX.initBuffer, sizeof(TX.initBuffer), "Initializing Threaded Image Buffer..."); //M104
	snprintf(TX.setBackground, sizeof(TX.setBackground), "Setting background theme..."); //M105
	snprintf(TX.freeTextures, sizeof(TX.freeTextures), "Freeing unused textures...");  //M106
	snprintf(TX.parentalCtrl, sizeof(TX.parentalCtrl), "Parental Control");   			//M107
	snprintf(TX.enableParentCtrl, sizeof(TX.enableParentCtrl), "Would you like to enable parental controls?"); //M108
	strcopy(TX.errorDIP, "Could not initialize DIP module! (ret = %d)\n", sizeof(TX.errorDIP));
	snprintf(TX.errorReadDir, sizeof(TX.errorReadDir), "Can't read directories. Covers will not be saved."); //M110
	snprintf(TX.errorCreateDir, sizeof(TX.errorCreateDir), "Can't create directories. Covers will not be saved."); //M111
	snprintf(TX.Sun, sizeof(TX.Sun), "Sun");   										//M112
	snprintf(TX.Mon, sizeof(TX.Mon), "Mon");   										//M113
	snprintf(TX.Tue, sizeof(TX.Tue), "Tue");   										//M114
	snprintf(TX.Wed, sizeof(TX.Wed), "Wed");   										//M115
	snprintf(TX.Thu, sizeof(TX.Thu), "Thu");   										//M116
	snprintf(TX.Fri, sizeof(TX.Fri), "Fri");   										//M117
	snprintf(TX.Sat, sizeof(TX.Sat), "Sat");   										//M118
	snprintf(TX.Jan, sizeof(TX.Jan), "Jan");   										//M119
	snprintf(TX.Feb, sizeof(TX.Feb), "Feb");   										//M120
	snprintf(TX.Mar, sizeof(TX.Mar), "Mar");   										//M121
	snprintf(TX.Apr, sizeof(TX.Apr), "Apr");   										//M122
	snprintf(TX.May, sizeof(TX.May), "May");   										//M123
	snprintf(TX.Jun, sizeof(TX.Jun), "Jun");   										//M124
	snprintf(TX.Jul, sizeof(TX.Jul), "Jul");   										//M125
	snprintf(TX.Aug, sizeof(TX.Aug), "Aug");   										//M126
	snprintf(TX.Sep, sizeof(TX.Sep), "Sep");   										//M127
	snprintf(TX.Oct, sizeof(TX.Oct), "Oct");   										//M128
	snprintf(TX.Nov, sizeof(TX.Nov), "Nov");   										//M129
	snprintf(TX.Dec, sizeof(TX.Dec), "Dec");   										//M130
	strcopy(TX.currentCIOS, "Current: cIOS Rev%d  Required: cIOS Rev%d", sizeof(TX.currentCIOS));
	snprintf(TX.cIOSupgrade, sizeof(TX.cIOSupgrade), "cIOS Upgrade Required");   		//M132
	snprintf(TX.noFreeSpace, sizeof(TX.noFreeSpace), "Not enough free space!" );  		//M133
	strcopy(TX.formatPartition, "Formatting Partition %s", sizeof(TX.formatPartition));
	snprintf(TX.addAGame, sizeof(TX.addAGame), "Please add a game..." );  		
	snprintf(TX.getAddData, sizeof(TX.getAddData), "Get additional data" );  		
	snprintf(TX.errTitles, sizeof(TX.errTitles), "Error downloading Titles.txt\nAn error occurred while downloading/saving file." );
	strcopy(TX.successTitles, "Succesfully imported %d titles!", sizeof(TX.successTitles));
	snprintf(TX.errNetTitles, sizeof(TX.errNetTitles), "Error initializing network\nTitles.txt can't be downloaded." );  		
	snprintf(TX.Success, sizeof(TX.Success), "Success!" );  		
	snprintf(TX.welcomeMsg, sizeof(TX.welcomeMsg), "Welcome to CoverFloader" );
	strcopy(TX.batteryUnit, "P%d", sizeof(TX.batteryUnit));

// Buttons	
	snprintf(TX.addB, sizeof(TX.addB), "add");     				//B01
	snprintf(TX.deleteB, sizeof(TX.deleteB), "delete");     		//B02
	snprintf(TX.cancelB, sizeof(TX.cancelB), "cancel");     		//B03
	snprintf(TX.yesB, sizeof(TX.yesB), "yes");     				//B04
	snprintf(TX.noB, sizeof(TX.noB), "no");     					//B05
	snprintf(TX.okB, sizeof(TX.okB), "ok");     					//B06
	snprintf(TX.resetB, sizeof(TX.resetB), "reset");     			//B07
	snprintf(TX.downloadB, sizeof(TX.downloadB), "download");     	//B08
	snprintf(TX.loadB, sizeof(TX.loadB), "load");     				//B09
	snprintf(TX.backB, sizeof(TX.backB), "back");     				//B10
	snprintf(TX.toggleOnB, sizeof(TX.toggleOnB), "on");     		//B11
	snprintf(TX.toggleOffB, sizeof(TX.toggleOffB), "off");     	//B12
	snprintf(TX.whiteB, sizeof(TX.whiteB), "white");     			//B13
	snprintf(TX.blackB, sizeof(TX.blackB), "black");     			//B14
	snprintf(TX.homeMenuTopB, sizeof(TX.homeMenuTopB), "exit");    //B15
	snprintf(TX.wiimenuB, sizeof(TX.wiimenuB), "Wii menu");        //B16
	snprintf(TX.loaderB, sizeof(TX.loaderB), "Loader");     		//B17
};


struct LANGUAGE TX;

void languageSet(char *name, char *val)
{
	cfg_name = name;
	cfg_val = val;

	if (strcmp(name, "iniNet") == 0) {
		strcopy(TX.iniNet, val, sizeof(TX.iniNet));
		return;
	}
	if (strcmp(name, "iniNetErr") == 0) {
		strcopy(TX.iniNetErr, val, sizeof(TX.iniNetErr));
		return;
	}
	if (strcmp(name, "error") == 0) {
		strcopy(TX.error, val, sizeof(TX.error));
		return;
	}
	if (strcmp(name, "iniNetReady") == 0) {
		strcopy(TX.iniNetReady, val, sizeof(TX.iniNetReady));
		return;
	}
	if (strcmp(name, "checkPresence") == 0) {
		strcopy(TX.checkPresence, val, sizeof(TX.checkPresence));
		return;
	}
	if (strcmp(name, "noDownload") == 0) {
		strcopy(TX.noDownload, val, sizeof(TX.noDownload));
		return;
	}	
	if (strcmp(name, "getting") == 0) {
		strcopy(TX.getting, val, sizeof(TX.getting));
		return;
	}	
	if (strcmp(name, "done") == 0) {
		strcopy(TX.done, val, sizeof(TX.done));
		return;
	}	
	if (strcmp(name, "someErrors") == 0) {
		strcopy(TX.someErrors, val, sizeof(TX.someErrors));
		return;
	}	
	if (strcmp(name, "checkNextCover") == 0) {
		strcopy(TX.checkNextCover, val, sizeof(TX.checkNextCover));
		return;
	}	
	if (strcmp(name, "opFinished") == 0) {
		strcopy(TX.opFinished, val, sizeof(TX.opFinished));
		return;
	}	
	if (strcmp(name, "pressA") == 0) {
		strcopy(TX.pressA, val, sizeof(TX.pressA));
		return;
	}	
	if (strcmp(name, "configError") == 0) {
		strcopy(TX.configError, val, sizeof(TX.configError));
		return;
	}	
	if (strcmp(name, "errorsaveGL") == 0) {
		strcopy(TX.errorsaveGL, val, sizeof(TX.errorsaveGL));
		return;
	}	
	if (strcmp(name, "errorcreateGL") == 0) {
		strcopy(TX.errorcreateGL, val, sizeof(TX.errorcreateGL));
		return;
	}	
	if (strcmp(name, "consoleDefault") == 0) {
		strcopy(TX.consoleDefault, val, sizeof(TX.consoleDefault));
		snprintf(languages[0],  sizeof(TX.consoleDefault), "%s", TX.consoleDefault);   		
		snprintf(vidmodes[5],   sizeof(TX.consoleDefault), "%s", TX.consoleDefault);   		
		return;
	}	
	if (strcmp(name, "Japanese") == 0) {
		strcopy(TX.Japanese, val, sizeof(TX.Japanese));
		snprintf(languages[1],  sizeof(TX.Japanese), "   %s", TX.Japanese);  
		return;
	}	
	if (strcmp(name, "English") == 0) {
		strcopy(TX.English, val, sizeof(TX.English));
		snprintf(languages[2],  sizeof(TX.English), "    %s", TX.English);  
		return;
	}
	if (strcmp(name, "German") == 0) {
		strcopy(TX.German, val, sizeof(TX.German));
		snprintf(languages[3],  sizeof(TX.German), "    %s", TX.German);  
		return;
	}	
	if (strcmp(name, "French") == 0) {
		strcopy(TX.French, val, sizeof(TX.French));
		snprintf(languages[4],  sizeof(TX.French), "    %s", TX.French);  
		return;
	}	
	if (strcmp(name, "Spanish") == 0) {
		strcopy(TX.Spanish, val, sizeof(TX.Spanish));
		snprintf(languages[5],   sizeof(TX.Spanish), "    %s", TX.Spanish);  
		return;
	}	
	if (strcmp(name, "Italian") == 0) {
		strcopy(TX.Italian, val, sizeof(TX.Italian));
		snprintf(languages[6],   sizeof(TX.Italian), "    %s", TX.Italian);  
		return;
	}	
	if (strcmp(name, "Dutch") == 0) {
		strcopy(TX.Dutch, val,  sizeof(TX.Dutch));
		snprintf(languages[7],  sizeof(TX.Dutch), "     %s",TX.Dutch);  
		return;
	}	
	if (strcmp(name, "SChinese") == 0) {
		strcopy(TX.SChinese, val, sizeof(TX.SChinese));
		snprintf(languages[8],    sizeof(TX.SChinese), "   %s", TX.SChinese);  
		return;
	}	
	if (strcmp(name, "TChinese") == 0) {
		strcopy(TX.TChinese, val, sizeof(TX.TChinese));
		snprintf(languages[9],    sizeof(TX.TChinese), "   %s", TX.TChinese);  
		return;
	}	
	if (strcmp(name, "Korean") == 0) {
		strcopy(TX.Korean, val, sizeof(TX.Korean));
		snprintf(languages[10], sizeof(TX.Korean), "    %s", TX.Korean);  
		return;
	}	
	if (strcmp(name, "gameDefault") == 0) {
		strcopy(TX.gameDefault, val, sizeof(TX.gameDefault));
		snprintf(vidmodes[0],  sizeof(TX.gameDefault),"  %s", TX.gameDefault);  
		return;
	}	
	if (strcmp(name, "automatic") == 0) {
		strcopy(TX.automatic, val, sizeof(TX.automatic));
		snprintf(vidmodes[1],  sizeof(TX.automatic), "   %s", TX.automatic);  
		return;
	}	
	if (strcmp(name, "forcePAL50") == 0) {
		strcopy(TX.forcePAL50, val, sizeof(TX.forcePAL50));
		snprintf(vidmodes[2],  sizeof(TX.forcePAL50), "   %s", TX.forcePAL50);  
		return;
	}	
	if (strcmp(name, "forcePAL60") == 0) {
		strcopy(TX.forcePAL60, val, sizeof(TX.forcePAL60));
		snprintf(vidmodes[3],  sizeof(TX.forcePAL60), "   %s", TX.forcePAL60);  
		return;
	}	
	if (strcmp(name, "forceNTSC") == 0) {
		strcopy(TX.forceNTSC, val, sizeof(TX.forceNTSC));
		snprintf(vidmodes[4],  sizeof(TX.forceNTSC), "   %s", TX.forceNTSC);  
		return;
	}	
	if (strcmp(name, "neverPlayed") == 0) {
		strcopy(TX.neverPlayed, val, sizeof(TX.neverPlayed));
		return;
	}	
	if (strcmp(name, "played") == 0) {
		strcopy(TX.played, val, sizeof(TX.played));
		return;
	}	
	if (strcmp(name, "size") == 0) {
		strcopy(TX.size, val, sizeof(TX.size));
		return;
	}	
	if (strcmp(name, "installing") == 0) {
		strcopy(TX.installing, val, sizeof(TX.installing));
		return;
	}	
	if (strcmp(name, "setting") == 0) {
		strcopy(TX.setting, val, sizeof(TX.setting));
		return;
	}	
	if (strcmp(name, "hook") == 0) {
		strcopy(TX.hook, val, sizeof(TX.hook));
		return;
	}
	if (strcmp(name, "ocarina") == 0) {
		strcopy(TX.ocarina, val, sizeof(TX.ocarina));
		return;
	}
	if (strcmp(name, "language") == 0) {
		strcopy(TX.language, val, sizeof(TX.language));
		return;
	}	
	if (strcmp(name, "videoMode") == 0) {
		strcopy(TX.videoMode, val, sizeof(TX.videoMode));
		return;
	}	
	if (strcmp(name, "patchVIDTV") == 0) {
		strcopy(TX.patchVIDTV, val, sizeof(TX.patchVIDTV));
		return;
	}	
	if (strcmp(name, "errorConnect") == 0) {
		strcopy(TX.errorConnect, val, sizeof(TX.errorConnect));
		return;
	}	
	if (strcmp(name, "URLnoBegin") == 0) {
		strcopy(TX.URLnoBegin, val, sizeof(TX.URLnoBegin));
		return;
	}	
	if (strcmp(name, "URLnoPath") == 0) {
		strcopy(TX.URLnoPath, val, sizeof(TX.URLnoPath));
		return;
	}	
	if (strcmp(name, "URLnoDomain") == 0) {
		strcopy(TX.URLnoDomain, val, sizeof(TX.URLnoDomain));
		return;
	}	
	if (strcmp(name, "errorDomain") == 0) {
		strcopy(TX.errorDomain, val, sizeof(TX.errorDomain));
		return;
	}	
	if (strcmp(name, "errEstablishConn") == 0) {
		strcopy(TX.errEstablishConn, val, sizeof(TX.errEstablishConn));
		return;
	}	
	if (strcmp(name, "HTTPnoFile") == 0) {
		strcopy(TX.HTTPnoFile, val, sizeof(TX.HTTPnoFile));
		return;
	}	
	if (strcmp(name, "noMemCopy") == 0) {
		strcopy(TX.noMemCopy, val, sizeof(TX.noMemCopy));
		return;
	}	
	if (strcmp(name, "graphicSetting") == 0) {
		strcopy(TX.graphicSetting, val, sizeof(TX.graphicSetting));
		return;
	}	
	if (strcmp(name, "zoom") == 0) {
		strcopy(TX.zoom, val, sizeof(TX.zoom));
		return;
	}	
	if (strcmp(name, "spacing") == 0) {
		strcopy(TX.spacing, val, sizeof(TX.spacing));
		return;
	}	
	if (strcmp(name, "angle") == 0) {
		strcopy(TX.angle, val, sizeof(TX.angle));
		return;
	}	
	if (strcmp(name, "falloff") == 0) {
		strcopy(TX.falloff, val, sizeof(TX.falloff));
		return;
	}	
	if (strcmp(name, "drawWindow") == 0) {
		strcopy(TX.drawWindow, val, sizeof(TX.drawWindow));
		return;
	}	
	if (strcmp(name, "gameTitle") == 0) {
		strcopy(TX.gameTitle, val, sizeof(TX.gameTitle));
		return;
	}	
	if (strcmp(name, "coverDownload") == 0) {
		strcopy(TX.coverDownload, val, sizeof(TX.coverDownload));
		return;
	}	
	if (strcmp(name, "opNoCancel") == 0) {
		strcopy(TX.opNoCancel, val, sizeof(TX.opNoCancel));
		return;
	}	
	if (strcmp(name, "cflowSettings") == 0) {
		strcopy(TX.cflowSettings, val, sizeof(TX.cflowSettings));
		return;
	}	
	if (strcmp(name, "graphics") == 0) {
		strcopy(TX.graphics, val, sizeof(TX.graphics));
		return;
	}	
	if (strcmp(name, "missingCovers") == 0) {
		strcopy(TX.missingCovers, val, sizeof(TX.missingCovers));
		return;
	}	
	if (strcmp(name, "theme") == 0) {
		strcopy(TX.theme, val, sizeof(TX.theme));
		return;
	}	
	if (strcmp(name, "oneClickLaunch") == 0) {
		strcopy(TX.oneClickLaunch, val, sizeof(TX.oneClickLaunch));
		return;
	}	
	if (strcmp(name, "rumble") == 0) {
		strcopy(TX.rumble, val, sizeof(TX.rumble));
		return;
	}	
	if (strcmp(name, "sound") == 0) {
		strcopy(TX.sound, val, sizeof(TX.sound));
		return;
	}	
	if (strcmp(name, "installNewGame") == 0) {
		strcopy(TX.installNewGame, val, sizeof(TX.installNewGame));
		return;
	}	
	if (strcmp(name, "diskInDrive") == 0) {
		strcopy(TX.diskInDrive, val, sizeof(TX.diskInDrive));
		return;
	}	
	if (strcmp(name, "initDVD") == 0) {
		strcopy(TX.initDVD, val, sizeof(TX.initDVD));
		return;
	}	
	if (strcmp(name, "pleaseWait") == 0) {
		strcopy(TX.pleaseWait, val, sizeof(TX.pleaseWait));
		return;
	}	
	if (strcmp(name, "errorReadDisc") == 0) {
		strcopy(TX.errorReadDisc, val, sizeof(TX.errorReadDisc));
		return;
	}	
	if (strcmp(name, "errorOpenDisc") == 0) {
		strcopy(TX.errorOpenDisc, val, sizeof(TX.errorOpenDisc));
		return;
	}	
	if (strcmp(name, "notWiiDisc") == 0) {
		strcopy(TX.notWiiDisc, val, sizeof(TX.notWiiDisc));
		return;
	}	
	if (strcmp(name, "insertWiiDisc") == 0) {
		strcopy(TX.insertWiiDisc, val, sizeof(TX.insertWiiDisc));
		return;
	}	
	if (strcmp(name, "alreadyInstalled") == 0) {
		strcopy(TX.alreadyInstalled, val, sizeof(TX.alreadyInstalled));
		return;
	}	
	if (strcmp(name, "noHDD") == 0) {
		strcopy(TX.noHDD, val, sizeof(TX.noHDD));
		return;
	}	
	if (strcmp(name, "installingGame") == 0) {
		strcopy(TX.installingGame, val, sizeof(TX.installingGame));
		return;
	}	
	if (strcmp(name, "install") == 0) {
		strcopy(TX.install, val, sizeof(TX.install));
		return;
	}	
	if (strcmp(name, "gameSize") == 0) {
		strcopy(TX.gameSize, val, sizeof(TX.gameSize));
		return;
	}	
	if (strcmp(name, "gameSizeSpace") == 0) {
		strcopy(TX.gameSizeSpace, val, sizeof(TX.gameSizeSpace));
		return;
	}	
	if (strcmp(name, "errorInstall") == 0) {
		strcopy(TX.errorInstall, val, sizeof(TX.errorInstall));
		return;
	}	
	if (strcmp(name, "successInstall") == 0) {
		strcopy(TX.successInstall, val, sizeof(TX.successInstall));
		return;
	}	
	if (strcmp(name, "askDelete") == 0) {
		strcopy(TX.askDelete, val, sizeof(TX.askDelete));
		return;
	}	
	if (strcmp(name, "errorDelete") == 0) {
		strcopy(TX.errorDelete, val, sizeof(TX.errorDelete));
		return;
	}	
	if (strcmp(name, "cantDelete") == 0) {
		strcopy(TX.cantDelete, val, sizeof(TX.cantDelete));
		return;
	}	
	if (strcmp(name, "successDelete") == 0) {
		strcopy(TX.successDelete, val, sizeof(TX.successDelete));
		return;
	}	
	if (strcmp(name, "pressOkContinue") == 0) {
		strcopy(TX.pressOkContinue, val, sizeof(TX.pressOkContinue));
		return;
	}	
	if (strcmp(name, "noLocalIP") == 0) {
		strcopy(TX.noLocalIP, val, sizeof(TX.noLocalIP));
		return;
	}	
	if (strcmp(name, "addError") == 0) {
		strcopy(TX.addError, val, sizeof(TX.addError));
		return;
	}	
	if (strcmp(name, "errorCIOS") == 0) {
		strcopy(TX.errorCIOS, val, sizeof(TX.errorCIOS));
		return;
	}	
	if (strcmp(name, "loadTextures") == 0) {
		strcopy(TX.loadTextures, val, sizeof(TX.loadTextures));
		return;
	}	
	if (strcmp(name, "initUSB") == 0) {
		strcopy(TX.initUSB, val, sizeof(TX.initUSB));
		return;
	}	
	if (strcmp(name, "errInitUSBFS") == 0) {
		strcopy(TX.errInitUSBFS, val, sizeof(TX.errInitUSBFS));
		return;
	}	
	if (strcmp(name, "initWBFS") == 0) {
		strcopy(TX.initWBFS, val, sizeof(TX.initWBFS));
		return;
	}	
	if (strcmp(name, "errorNoUSBDrv") == 0) {
		strcopy(TX.errorNoUSBDrv, val, sizeof(TX.errorNoUSBDrv));
		return;
	}	
	if (strcmp(name, "pressAB") == 0) {
		strcopy(TX.pressAB, val, sizeof(TX.pressAB));
		return;
	}	
	if (strcmp(name, "connectUSBDrv") == 0) {
		strcopy(TX.connectUSBDrv, val, sizeof(TX.connectUSBDrv));
		return;
	}	
	if (strcmp(name, "errorNoWBFS") == 0) {
		strcopy(TX.errorNoWBFS, val, sizeof(TX.errorNoWBFS));
		return;
	}	
	if (strcmp(name, "hold12B") == 0) {
		strcopy(TX.hold12B, val, sizeof(TX.hold12B));
		return;
	}	
	if (strcmp(name, "findPartitions") == 0) {
		strcopy(TX.findPartitions, val, sizeof(TX.findPartitions));
		return;
	}	
	if (strcmp(name, "partition") == 0) {
		strcopy(TX.partition, val, sizeof(TX.partition));
		return;
	}	
	if (strcmp(name, "partitionNoFormat") == 0) {
		strcopy(TX.partitionNoFormat, val, sizeof(TX.partitionNoFormat));
		return;
	}	
	if (strcmp(name, "askFormat") == 0) {
		strcopy(TX.askFormat, val, sizeof(TX.askFormat));
		return;
	}	
	if (strcmp(name, "errorFormat") == 0) {
		strcopy(TX.errorFormat, val, sizeof(TX.errorFormat));
		return;
	}	
	if (strcmp(name, "successFormat") == 0) {
		strcopy(TX.successFormat, val, sizeof(TX.successFormat));
		return;
	}	
	if (strcmp(name, "formatComplete") == 0) {
		strcopy(TX.formatComplete, val, sizeof(TX.formatComplete));
		return;
	}	
	if (strcmp(name, "initBuffer") == 0) {
		strcopy(TX.initBuffer, val, sizeof(TX.initBuffer));
		return;
	}	
	if (strcmp(name, "setBackground") == 0) {
		strcopy(TX.setBackground, val, sizeof(TX.setBackground));
		return;
	}	
	if (strcmp(name, "freeTextures") == 0) {
		strcopy(TX.freeTextures, val, sizeof(TX.freeTextures));
		return;
	}	
	if (strcmp(name, "parentalCtrl") == 0) {
		strcopy(TX.parentalCtrl, val, sizeof(TX.parentalCtrl));
		return;
	}	
	if (strcmp(name, "enableParentCtrl") == 0) {
		strcopy(TX.enableParentCtrl, val, sizeof(TX.enableParentCtrl));
		return;
	}	
	if (strcmp(name, "errorDIP") == 0) {
		strcopy(TX.errorDIP, val, sizeof(TX.errorDIP));
		return;
	}	
	if (strcmp(name, "errorReadDir") == 0) {
		strcopy(TX.errorReadDir, val, sizeof(TX.errorReadDir));
		return;
	}	
	if (strcmp(name, "errorCreateDir") == 0) {
		strcopy(TX.errorCreateDir, val, sizeof(TX.errorCreateDir));
		return;
	}	
	if (strcmp(name, "Sun") == 0) {
		strcopy(TX.Sun, val, sizeof(TX.Sun));
		return;
	}	
	if (strcmp(name, "Mon") == 0) {
		strcopy(TX.Mon, val, sizeof(TX.Mon));
		return;
	}	
	if (strcmp(name, "Tue") == 0) {
		strcopy(TX.Tue, val, sizeof(TX.Tue));
		return;
	}	
	if (strcmp(name, "Wed") == 0) {
		strcopy(TX.Wed, val, sizeof(TX.Wed));
		return;
	}	
	if (strcmp(name, "Thu") == 0) {
		strcopy(TX.Thu, val, sizeof(TX.Thu));
		return;
	}	
	if (strcmp(name, "Fri") == 0) {
		strcopy(TX.Fri, val, sizeof(TX.Fri));
		return;
	}	
	if (strcmp(name, "Sat") == 0) {
		strcopy(TX.Sat, val, sizeof(TX.Sat));
		return;
	}	
	if (strcmp(name, "Jan") == 0) {
		strcopy(TX.Jan, val, sizeof(TX.Jan));
		return;
	}	
	if (strcmp(name, "Feb") == 0) {
		strcopy(TX.Feb, val, sizeof(TX.Feb));
		return;
	}	
	if (strcmp(name, "Mar") == 0) {
		strcopy(TX.Mar, val, sizeof(TX.Mar));
		return;
	}	
	if (strcmp(name, "Apr") == 0) {
		strcopy(TX.Apr, val, sizeof(TX.Apr));
		return;
	}	
	if (strcmp(name, "May") == 0) {
		strcopy(TX.May, val, sizeof(TX.May));
		return;
	}	
	if (strcmp(name, "Jun") == 0) {
		strcopy(TX.Jun, val, sizeof(TX.Jun));
		return;
	}	
	if (strcmp(name, "Jul") == 0) {
		strcopy(TX.Jul, val, sizeof(TX.Jul));
		return;
	}	
	if (strcmp(name, "Aug") == 0) {
		strcopy(TX.Aug, val, sizeof(TX.Aug));
		return;
	}	
	if (strcmp(name, "Sep") == 0) {
		strcopy(TX.Sep, val, sizeof(TX.Sep));
		return;
	}	
	if (strcmp(name, "Oct") == 0) {
		strcopy(TX.Oct, val, sizeof(TX.Oct));
		return;
	}	
	if (strcmp(name, "Nov") == 0) {
		strcopy(TX.Nov, val, sizeof(TX.Nov));
		return;
	}	
	if (strcmp(name, "Dec") == 0) {
		strcopy(TX.Dec, val, sizeof(TX.Dec));
		return;
	}	
	if (strcmp(name, "currentCIOS") == 0) {
		strcopy(TX.currentCIOS, val, sizeof(TX.currentCIOS));
		return;
	}	
	if (strcmp(name, "cIOSupgrade") == 0) {
		strcopy(TX.cIOSupgrade, val, sizeof(TX.cIOSupgrade));
		return;
	}	
	if (strcmp(name, "noFreeSpace") == 0) {
		strcopy(TX.noFreeSpace, val, sizeof(TX.noFreeSpace));
		return;
	}	
	if (strcmp(name, "formatPartition") == 0) {
		strcopy(TX.formatPartition, val, sizeof(TX.formatPartition));
		return;
	}
	if (strcmp(name, "addAGame") == 0) {
		strcopy(TX.addAGame, val, sizeof(TX.addAGame));
		return;
	}
	if (strcmp(name, "getAddData") == 0) {
		strcopy(TX.getAddData, val, sizeof(TX.getAddData));
		return;
	}
	if (strcmp(name, "errTitles") == 0) {
		strcopy(TX.errTitles, val, sizeof(TX.errTitles));
		return;
	}
	if (strcmp(name, "successTitles") == 0) {
		strcopy(TX.successTitles, val, sizeof(TX.successTitles));
		return;
	}
	if (strcmp(name, "errNetTitles") == 0) {
		strcopy(TX.errNetTitles, val, sizeof(TX.errNetTitles));
		return;
	}
	if (strcmp(name, "Success") == 0) {
		strcopy(TX.Success, val, sizeof(TX.Success));
		return;
	}
	if (strcmp(name, "welcomeMsg") == 0) {
		strcopy(TX.welcomeMsg, val, sizeof(TX.welcomeMsg));
		return;
	}
	if (strcmp(name, "batteryUnit") == 0) {
		strcopy(TX.batteryUnit, val, sizeof(TX.batteryUnit));
		return;
	}

//////////////// Buttons //////////////

	
	if (strcmp(name, "addB") == 0) {
		strcopy(TX.addB, val, sizeof(TX.addB));
		return;
	}	
	if (strcmp(name, "deleteB") == 0) {
		strcopy(TX.deleteB, val, sizeof(TX.deleteB));
		return;
	}	
	if (strcmp(name, "cancelB") == 0) {
		strcopy(TX.cancelB, val, sizeof(TX.cancelB));
		return;
	}	
	if (strcmp(name, "yesB") == 0) {
		strcopy(TX.yesB, val, sizeof(TX.yesB));
		return;
	}	
	if (strcmp(name, "noB") == 0) {
		strcopy(TX.noB, val, sizeof(TX.noB));
		return;
	}	
	if (strcmp(name, "okB") == 0) {
		strcopy(TX.okB, val, sizeof(TX.okB));
		return;
	}	
	if (strcmp(name, "resetB") == 0) {
		strcopy(TX.resetB, val, sizeof(TX.resetB));
		return;
	}	
	if (strcmp(name, "downloadB") == 0) {
		strcopy(TX.downloadB, val, sizeof(TX.downloadB));
		return;
	}	
	if (strcmp(name, "loadB") == 0) {
		strcopy(TX.loadB, val, sizeof(TX.loadB));
		return;
	}	
	if (strcmp(name, "backB") == 0) {
		strcopy(TX.backB, val, sizeof(TX.backB));
		return;
	}	
	if (strcmp(name, "toggleOnB") == 0) {
		strcopy(TX.toggleOnB, val, sizeof(TX.toggleOnB));
		return;
	}	
	if (strcmp(name, "toggleOffB") == 0) {
		strcopy(TX.toggleOffB, val, sizeof(TX.toggleOffB));
		return;
	}	
	if (strcmp(name, "whiteB") == 0) {
		strcopy(TX.whiteB, val, sizeof(TX.whiteB));
		return;
	}	
	if (strcmp(name, "blackB") == 0) {
		strcopy(TX.blackB, val, sizeof(TX.blackB));
		return;
	}	
	if (strcmp(name, "homeMenuTopB") == 0) {
		strcopy(TX.homeMenuTopB, val, sizeof(TX.homeMenuTopB));
		return;
	}	
	if (strcmp(name, "wiimenuB") == 0) {
		strcopy(TX.wiimenuB, val, sizeof(TX.wiimenuB));
		return;
	}	
	if (strcmp(name, "loaderB") == 0) {
		strcopy(TX.loaderB, val, sizeof(TX.loaderB));
		return;
	}	

}



void languageLoad()
{
	char pathname[200];

    sprintf(pathname, USBLOADER_PATH "/languages/%s", langFile);
	cfg_parsefile(pathname, &languageSet);
}


char* strcopy(char *dest, char *src, int size)
{
	strncpy(dest,src,size);
	dest[size-1] = 0;
	return dest;
}

