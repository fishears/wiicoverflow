
//Code taken for the most part from USB Loader GX... Thanks!

#ifndef _XML_H_
#define _XML_H_


#ifdef __cplusplus
extern "C"
{
#endif

// open database, close database, load info for a game
int OpenXMLDatabase(char* xmlfilepath, char* argdblang, int argJPtoEN, int openfile, int loadtitles, int keepopen);
void CloseXMLDatabase();
int LoadGameInfoFromXML(char* gameid);


#define XML_ELEMMAX 15

struct gameXMLinfo
{	
	char id[7];
	char version[50];
	char region[7];
	char title[100];
	char synopsis[2000];
	char title_EN[100];
	char synopsis_EN[2000];
	char locales[XML_ELEMMAX+1][3];
	int localeCnt;
	char developer[75];
	char publisher[75];
	char publisherfromid[75];
	char year[5];
	char month[3];
	char day[3];
	char genre[75];
	char genresplit[XML_ELEMMAX+1][20];
	int genreCnt;
	char ratingtype[5];
	char ratingvalue[5];
	char ratingdescriptors[XML_ELEMMAX+1][40];
	int descriptorCnt;
	char ratingvalueCERO[5];
	char ratingvalueESRB[5];
	char ratingvaluePEGI[5];
	char wifiplayers[4];
	char wififeatures[XML_ELEMMAX+1][20];
	int wifiCnt;
	char players[4];
	char accessories[XML_ELEMMAX+1][20];
	int accessoryCnt;
	char accessoriesReq[XML_ELEMMAX+1][20];
	int accessoryReqCnt;
	char iso_crc[9];
	char iso_md5[33];
	char iso_sha1[41];
} ;

struct gameXMLinfo gameinfo;
struct gameXMLinfo gameinfo_reset;

int OpenXMLFile(char* filename);
void LoadTitlesFromXML(char *langcode, int forcejptoen);
void GetPublisherFromGameid(char *idtxt, char *dest, int destsize);
char *ConvertLangTextToCode(char *langtext);
void ConvertRating(char *ratingvalue, char *fromrating, char *torating, char *destvalue, int destsize);
void PrintGameInfo(int showfullinfo);


void title_set(char *id, char *title);
char* trimcopy(char *dest, char *src, int size);

#ifdef __cplusplus
}
#endif

#endif

