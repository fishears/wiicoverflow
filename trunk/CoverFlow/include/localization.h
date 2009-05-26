//////////////////////#include <gctypes.h>
#ifndef _LOCALIZATION_H_
#define _LOCALIZATION_H_

#include "coverflow.h"

char *localStr ( char *CODE, char * defaultStr );
void langLoad();

struct LANGUAGE
{
	
	char iniNet[40];				//M001
	char iniNetErr[40];				//M002
	char error[20];					//M003
	char iniNetReady[40];			//M004	
	char checkPresence[40];			//M005
	char noDownload[40];			//M006
	char getting[20];				//M007
	char done[20];					//M008
	char someErrors[40];			//M009
	char checkNextCover[40];		//M010
	char opFinished[40];			//M011
	char pressA[40];				//M012
	char configError[100];			//M013
	char errorsaveGL[50];			//M014
	char errorcreateGL[50];			//M015
	char consoleDefault[40];		//M016
	char Japanese[30];				//M017
	char German[30];				//M018
	char English[30];				//M019
	char French[30];				//M020
	char Spanish[30];				//M021
	char Italian[30];				//M022
	char Dutch[30];					//M023
	char SChinese[30];				//M024
	char TChinese[30];				//M025
	char Korean[30];				//M026
	char gameDefault[40];			//M027
	char automatic[40];				//M028
    char forcePAL50[40];			//M029
	char forcePAL60[40];			//M030
	char forceNTSC[40];				//M031
	char neverPlayed[50];			//M032
	char played[50];				//M033
	char size[30];					//M034
	char installing[40];			//M035
	char setting[40];				//M036
	char hook[40];				    //M037
	char language[40];				//M038
	char videoMode[40];				//M039
	char patchVIDTV[40];			//M040
	char errorConnect[100];			//M041
	char URLnoBegin[100];			//M042
	char URLnoPath[100];			//M043
	char URLnoDomain[100];			//M044
	char errorDomain[100];			//M045
	char errEstablishConn[60];		//M046
	char HTTPnoFile[60];			//M047
	char noMemCopy[80];				//M048
	char graphicSetting[40];		//M049
	char zoom[20];					//M050
	char spacing[20];				//M051
	char angle[20];					//M052
	char drawWindow[30];			//M053
	char gameTitle[30];				//M054
	char coverDownload[40];			//M055
	char opNoCancel[100];			//M056
	char cflowSettings[40];			//M057
	char graphics[40];				//M058
	char missingCovers[40];			//M059
	char theme[30];					//M060
	char oneClickLaunch[30];		//M061
	char rumble[30];				//M062
	char sound[30];					//M063
	char installNewGame[50];		//M064
	char diskInDrive[50];			//M065
	char initDVD[40];				//M066
	char pleaseWait[40];			//M067
	char errorReadDisc[50];			//M068
	char errorOpenDisc[50];			//M069
	char notWiiDisc[50];			//M070
	char insertWiiDisc[50];			//M071
	char alreadyInstalled[50];		//M072
	char noHDD[50];					//M073
	char installingGame[40];		//M074
	char install[30];				//M075
	char gameSize[40];				//M076
	char gameSizeSpace[60];			//M077
	char errorInstall[40];			//M078
	char successInstall[40];		//M079
	char askDelete[50];				//M080
	char errorDelete[40];			//M081
	char cantDelete[40];			//M082
	char successDelete[40];			//M083
	char pressOkContinue[50];		//M084
	char noLocalIP[50];				//M085
	char addError[40];				//M086
	char errorCIOS[80];				//M087
	char loadTextures[40];			//M088
	char initUSB[40];				//M089
	char errInitUSBFS[60];			//M090
	char initWBFS[40];				//M091
	char errorNoUSBDrv[60];			//M092
	char pressAB[40];				//M093
	char connectUSBDrv[80];			//M094
	char errorNoWBFS[80];			//M095
	char hold12B[80];				//M096
	char findPartitions[50];		//M097
	char partition[40];				//M098
	char partitionNoFormat[60];		//M099
	char askFormat[80];				//M100
	char errorFormat[40];			//M101
	char successFormat[40];			//M102
	char formatComplete[50];		//M103
	char initBuffer[50];			//M104
	char setBackground[50];			//M105
	char freeTextures[50];			//M106
	char parentalCtrl[50];			//M107
	char enableParentCtrl[100];		//M108
	char errorDIP[100];				//M109
	char errorReadDir[100];			//M110
	char errorCreateDir[100];		//M111
	char Sun[10];					//M112
	char Mon[10];					//M113
	char Tue[10];					//M114
	char Wed[10];					//M115
	char Thu[10];					//M116
	char Fri[10];					//M117
	char Sat[10];					//M118
	char Jan[10];					//M119
	char Feb[10];					//M120
	char Mar[10];					//M121
	char Apr[10];					//M122
	char May[10];					//M123
	char Jun[10];					//M124
	char Jul[10];					//M125
	char Aug[10];					//M126
	char Sep[10];					//M127
	char Oct[10];					//M128
	char Nov[10];					//M129
	char Dec[10];					//M130
	char currentCIOS[100];			//M131
	char cIOSupgrade[60];			//M132
	char noFreeSpace[60];			//M133
	char formatPartition[40];		//M134
// Buttons	
	char addB[15];					//B01
	char deleteB[15];				//B02
	char cancelB[15];				//B03
	char yesB[15];					//B04
	char noB[15];					//B05
	char okB[15];					//B06
	char resetB[15];				//B07
	char downloadB[15];				//B08
	char loadB[15];					//B09
	char backB[15];					//B10	
	char toggleOnB[15];				//B11
	char toggleOffB[15];			//B12
	char whiteB[15];				//B13
	char blackB[15];				//B14
	char homeMenuTopB[15];			//B15
	char wiimenuB[15];				//B16	
	char loaderB[15];				//B17
};

extern struct LANGUAGE TX;

void languageDefault();
void languageLoad();
void languageSet(char *name, char *val);
char* strcopy(char *dest, char *src, int size);
bool checkfile(char * path);

#endif
