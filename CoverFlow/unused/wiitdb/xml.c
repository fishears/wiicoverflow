/*
Load game information from XML - Lustar
 - Mini-XML by Michael Sweet
 - MiniZip adapted by Tantric
*/

#include <malloc.h>
#include "mxml.h"
#include "unzip.h"
//#include "settings/cfg.h"
#include "xml.h"
//#include "cfg.h"
//#include "xml.h"

#include "gfx.h"

//Code taken for the most part from USB Loader GX... Thanks!


/* config */
//static int xmldebug = 0;
static char xmlcfg_filename[100] = "wiitdb.zip";

extern struct SSettings Settings; // for loader GX

static char langlist[11][22] =
{{"Console Default"},
{"Japanese"},
{"English"},
{"German"},
{"French"},
{"Spanish"},
{"Italian"},
{"Dutch"},
{"S. Chinese"},
{"T. Chinese"},
{"Korean"}};

static char langcodes[11][3] =
{{""},
{"JA"},
{"EN"},
{"DE"},
{"FR"},
{"ES"},
{"IT"},
{"NL"},
{"ZH"},
{"ZH"},
{"KO"}};

static char element_text[5000];
static mxml_node_t *nodetree=NULL;
static mxml_node_t *nodedata=NULL;
static mxml_node_t *nodeid=NULL;
static mxml_node_t *nodeidtmp=NULL;
static mxml_node_t *nodefound=NULL;
static mxml_index_t *nodeindex=NULL;
static mxml_index_t *nodeindextmp=NULL;
int xmlloadtime = 0;
static char * get_text(mxml_node_t *node, char *buffer, int buflen);
int xml_loaded = 0;


/* load renamed titles from proper names and game info XML, needs to be after cfg_load_games */
int OpenXMLDatabase(char* xmlfilepath, char* argdblang, int argJPtoEN, int openfile, int loadtitles, int keepopen)
{
	if (!xml_loaded) {
		int opensuccess = 0;
		char pathname[200];
		snprintf(pathname, sizeof(pathname), "%s", xmlfilepath);
		if (xmlfilepath[strlen(xmlfilepath) - 1] != '/') snprintf(pathname, sizeof(pathname), "%s/",pathname);
		snprintf(pathname, sizeof(pathname), "%s%s", pathname, xmlcfg_filename);
		if (openfile) opensuccess = OpenXMLFile(pathname);
		if (!opensuccess) {
			snprintf(pathname, sizeof(pathname), "%swiitdb.zip", pathname);
		if (openfile) opensuccess = OpenXMLFile(pathname);
			}
		if (!opensuccess) {
			CloseXMLDatabase();
			return 0;
		}
		if (loadtitles) LoadTitlesFromXML(argdblang, argJPtoEN); 						
		if (!keepopen) CloseXMLDatabase();
	}
	return 1;
}

void CloseXMLDatabase()
{
    /* free memory */
	if (xml_loaded) {	
		mxmlDelete(nodedata);
		mxmlDelete(nodetree);
		xml_loaded = 0;
	}
}	
	

void GetTextFromNode(mxml_node_t *currentnode, mxml_node_t *topnode, char *nodename, 
			char *attributename, char *value, int descend, char *dest, int destsize)
{
	*element_text = 0;
    nodefound = mxmlFindElement(currentnode, topnode, nodename, attributename, value, descend);
	if (nodefound != NULL)	{
		if (attributename != NULL) {
			strlcpy(dest,mxmlElementGetAttr(nodefound, attributename),destsize);
		} else {
			get_text(nodefound, element_text, sizeof(element_text));
			strlcpy(dest,element_text,destsize);
		}
	} else {
		strcpy(dest,"");
	}
}	


int OpenXMLFile(char *filename)
{
	//if (xmldebug) dbg_time1();
		
	if (xml_loaded) 
		 return 0;
	
	gameinfo = gameinfo_reset;
	nodedata=NULL;
	nodetree=NULL;
	nodeid=NULL;
	nodeidtmp=NULL;
	nodefound=NULL;

	char* strresult = strstr(filename,".zip");
    if (strresult == NULL) {
		/* Load XML file */
		FILE *filexml;
		filexml = fopen(filename, "rb");
		if (!filexml)
			return 0;
		
		nodetree = mxmlLoadFile(NULL, filexml, MXML_NO_CALLBACK);
		fclose(filexml);

	} else {
		/* load zipped XML file */
		unzFile unzfile = unzOpen(filename);
		if (unzfile == NULL)
			return 0;
			
		unzOpenCurrentFile(unzfile);
		
		unz_file_info zipfileinfo;
		unzGetCurrentFileInfo(unzfile, &zipfileinfo, NULL, 0, NULL, 0, NULL, 0);	
		int zipfilebuffersize = zipfileinfo.uncompressed_size;
		char * zipfilebuffer = malloc(zipfilebuffersize);
		memset(zipfilebuffer, 0, zipfilebuffersize);
		
		if (zipfilebuffer == NULL) {
			unzCloseCurrentFile(unzfile);
			unzClose(unzfile);
			return 0;
		}
		
		unzReadCurrentFile(unzfile, zipfilebuffer, zipfilebuffersize);
		unzCloseCurrentFile(unzfile);
		unzClose(unzfile);
		
		
		
		nodetree = mxmlLoadString(NULL, zipfilebuffer, MXML_NO_CALLBACK);
		free(zipfilebuffer);
	}

	if (nodetree == NULL)
	{
		WindowPrompt("Failed to load wiitdb", "Check available memory; need at least 2 MB", &okButton, 0);
		return 0;
	}
	
    nodedata = mxmlFindElement(nodetree, nodetree, "datafile", NULL, NULL, MXML_DESCEND);
   	if (nodedata == NULL) {
	    return 0;
	} else {
		//if (xmldebug)	xmlloadtime = dbg_time2(NULL);
		xml_loaded = 1;
	WindowPrompt("Debug g", "g", &okButton, 0);
		return 1;
	}
}


/*
char *GetLangSettingFromGame(char *gameid)
{
	int langcode;
	struct Game_CFG *game_cfg = NULL;
	game_cfg = CFG_get_game_opt((u8*)gameid);
	if (game_cfg) {
		langcode = game_cfg->language;
	} else {
		//langcode = CFG.language; // for Configurable Loader
		langcode = Settings.language; // for Loader GX
	}
	char *langtxt = langlist[langcode];
	return langtxt;
}
*/

/* convert language text into ISO 639 two-letter language code */
char *ConvertLangTextToCode(char *languagetxt)
{
	int i;
	for (i=0;i<=10;i++)
	{
		if (!strcasecmp(languagetxt,langlist[i])) // case insensitive comparison
			return langcodes[i];
	}
	return "";
}


char ConvertRatingToIndex(char *ratingtext)
{
	int type = -1;
	if (!strcmp(ratingtext,"CERO")) { type = 0; }
	if (!strcmp(ratingtext,"ESRB")) { type = 1; }
	if (!strcmp(ratingtext,"PEGI")) { type = 2; }
	return type;
}

void ConvertRating(char *ratingvalue, char *fromrating, char *torating, char *destvalue, int destsize)
{
	if(!strcmp(fromrating,torating)) {
		strlcpy(destvalue,ratingvalue,destsize);
		return;
	}

	strcpy(destvalue,"");
	int type = -1;
	int desttype = -1;

	type = ConvertRatingToIndex(fromrating);
	desttype = ConvertRatingToIndex(torating);
	if (type == -1 || desttype == -1)
		return;
	
	/* rating conversion table */
	/* the list is ordered to pick the most likely value first: */
	/* EC and AO are less likely to be used so they are moved down to only be picked up when converting ESRB to PEGI or CERO */
	/* the conversion can never be perfect because ratings can differ between regions for the same game */
	char ratingtable[12][3][4] =
	{
		{{"A"},{"E"},{"3"}},
		{{"A"},{"E"},{"4"}},
		{{"A"},{"E"},{"6"}},
		{{"A"},{"E"},{"7"}},
		{{"A"},{"EC"},{"3"}},
		{{"A"},{"E10+"},{"7"}},
		{{"B"},{"T"},{"12"}},
		{{"D"},{"M"},{"18"}},
		{{"D"},{"M"},{"16"}},
		{{"C"},{"T"},{"16"}},
		{{"C"},{"T"},{"15"}},
		{{"Z"},{"AO"},{"18"}},
	};
	
	int i;
	for (i=0;i<=11;i++)
	{
		if (!strcmp(ratingtable[i][type],ratingvalue)) {
			strlcpy(destvalue,ratingtable[i][desttype],destsize);
			return;
		}
	}
}


void LoadTitlesFromXML(char *langtxt, int forcejptoen)
/* langtxt: set to "English","French","German", to force language for all titles, or "" to load title depending on each game's setting */
/* forcejptoen: set to true to load English title instead of Japanese title when game is set to Japanese */
{
	if (nodedata == NULL)
	    return;
                
	int forcelang = 0;
	if (strcmp(langtxt,""))
		forcelang = 1;	

	char langcode[10] = "";
	if (forcelang)
		strcpy(langcode,ConvertLangTextToCode(langtxt)); /* convert language text into ISO 639 two-letter language code */
	
	/* create index of <id> elements */
    nodeindex = mxmlIndexNew(nodedata,"id", NULL);	
    nodeid = mxmlIndexReset(nodeindex);
    *element_text = 0;
	char id_text[10];
	char title_text[100] = "";
	char title_text_EN[100] = "";
	
	/* search index of id elements, load all id/titles text */
    while (nodeid != NULL)
    {
        nodeid = mxmlIndexFind(nodeindex,"id", NULL);
	    if (nodeid != NULL) {	
			strcpy(title_text,"");
			strcpy(title_text_EN,"");
			
			get_text(nodeid, element_text, sizeof(element_text));
			snprintf(id_text, 7, "%s",element_text);
			
			// if language is not forced, use game language setting from config
			//if (!forcelang) {
			//	langtxt = GetLangSettingFromGame(id_text);
			//		strcpy(langcode,ConvertLangTextToCode(langtxt));
			//}

			/* if enabled, force English title for all games set to Japanese */
			if (forcejptoen && (!strcmp(langcode,"JA")))
				strcpy(langcode,"EN");
	
			/* load title from nodes */
			nodefound = mxmlFindElement(nodeid, nodedata, "locale", "lang", "EN", MXML_NO_DESCEND);
			if (nodefound != NULL) {
				GetTextFromNode(nodefound, nodedata, "title", NULL, NULL, MXML_DESCEND, title_text_EN,sizeof(title_text_EN));
			}
			nodefound = mxmlFindElement(nodeid, nodedata, "locale", "lang", langcode, MXML_NO_DESCEND);
			if (nodefound != NULL) {
				GetTextFromNode(nodefound, nodedata, "title", NULL, NULL, MXML_DESCEND, title_text,sizeof(title_text));
			}

			/* fall back to English title if prefered language was not found */
			if (!strcmp(title_text,"")) {
				strcpy(title_text,title_text_EN);
			}
									
			snprintf(id_text, 5, "%s",id_text);
			title_set(id_text, title_text);
	    }
    }
	
	// free memory
	mxmlIndexDelete(nodeindex);

    //if (xmldebug) xmlloadtime = dbg_time2(NULL);
}


void GetPublisherFromGameid(char *idtxt, char *dest, int destsize) 
{
	/* guess publisher from company list using last two characters from game id */
	nodeindextmp = mxmlIndexNew(nodedata,"company", NULL);
	nodeidtmp = mxmlIndexReset(nodeindextmp);

    *element_text=0;
	char publishercode[3];
	sprintf(publishercode,"%c%c", idtxt[4],idtxt[5]);

    while (strcmp(element_text,publishercode) != 0)
    {
	    nodeidtmp = mxmlIndexFind(nodeindextmp,"company", NULL);
	    if (nodeidtmp != NULL) {
			strlcpy(element_text,mxmlElementGetAttr(nodeidtmp, "code"),sizeof(element_text));
	    } else {
	        break;
	    }
    }
	if (!strcmp(element_text,publishercode)) {
		strlcpy(dest,mxmlElementGetAttr(nodeidtmp, "name"),destsize);
	} else {
		strcpy(dest,"");
	}
	
	// free memory
	mxmlIndexDelete(nodeindextmp);
}



int LoadGameInfoFromXML(char* gameid)
/* gameid: full game id */
/* langtxt: "English","French","German" */
{	
	int exist=0;
	if (!xml_loaded || nodedata == NULL) 		 
		return exist;
		
	// load game info using forced language, or game individual setting, or main language setting
	char langcode[100] = "";
	//if (!strcmp(langtxt,""))
	//	langtxt = GetLangSettingFromGame(gameid);
	//strcpy(langcode,ConvertLangTextToCode(langtxt));
	
	/* reset all game info */
	gameinfo = gameinfo_reset;
	
    /* index all IDs */
    nodeindex = mxmlIndexNew(nodedata,"id", NULL);	
    nodeid = mxmlIndexReset(nodeindex);
	*element_text = 0;
	/* search for game matching gameid */
    while (1)
    {
        nodeid = mxmlIndexFind(nodeindex,"id", NULL);
	    if (nodeid != NULL) {
			get_text(nodeid, element_text, sizeof(element_text));
			if (!strcmp(element_text,gameid)) {
				break;
			}
	    } else {
			break;
	    }
    }
	
    if (!strcmp(element_text,gameid)) {
		/* text from elements */
		strlcpy(gameinfo.id,element_text,sizeof(gameinfo.id));
		GetTextFromNode(nodeid, nodedata, "region", NULL, NULL, MXML_NO_DESCEND, gameinfo.region,sizeof(gameinfo.region));
		GetTextFromNode(nodeid, nodedata, "version", NULL, NULL, MXML_NO_DESCEND, gameinfo.version,sizeof(gameinfo.version));
		GetTextFromNode(nodeid, nodedata, "genre", NULL, NULL, MXML_NO_DESCEND, gameinfo.genre,sizeof(gameinfo.genre));
		GetTextFromNode(nodeid, nodedata, "developer", NULL, NULL, MXML_NO_DESCEND, gameinfo.developer,sizeof(gameinfo.developer));
		GetTextFromNode(nodeid, nodedata, "publisher", NULL, NULL, MXML_NO_DESCEND, gameinfo.publisher,sizeof(gameinfo.publisher));
		GetPublisherFromGameid(gameid,gameinfo.publisherfromid,sizeof(gameinfo.publisherfromid));
		
		/* text from attributes */
		GetTextFromNode(nodeid, nodedata, "date", "year", NULL, MXML_NO_DESCEND, gameinfo.year,sizeof(gameinfo.year));
		GetTextFromNode(nodeid, nodedata, "date", "month", NULL,MXML_NO_DESCEND, gameinfo.month,sizeof(gameinfo.month));
		GetTextFromNode(nodeid, nodedata, "date", "day", NULL, MXML_NO_DESCEND, gameinfo.day,sizeof(gameinfo.day));
		GetTextFromNode(nodeid, nodedata, "rating", "type", NULL, MXML_NO_DESCEND, gameinfo.ratingtype,sizeof(gameinfo.ratingtype));
		GetTextFromNode(nodeid, nodedata, "rating", "value", NULL, MXML_NO_DESCEND, gameinfo.ratingvalue,sizeof(gameinfo.ratingvalue));
		GetTextFromNode(nodeid, nodedata, "rom", "crc", NULL, MXML_NO_DESCEND, gameinfo.iso_crc,sizeof(gameinfo.iso_crc));
		GetTextFromNode(nodeid, nodedata, "rom", "md5", NULL, MXML_NO_DESCEND, gameinfo.iso_md5,sizeof(gameinfo.iso_md5));
		GetTextFromNode(nodeid, nodedata, "rom", "sha1", NULL, MXML_NO_DESCEND, gameinfo.iso_sha1,sizeof(gameinfo.iso_sha1));
			
		/* text from child elements */
		nodefound = mxmlFindElement(nodeid, nodedata, "locale", "lang", "EN", MXML_NO_DESCEND);
		if (nodefound != NULL) {
			GetTextFromNode(nodefound, nodedata, "title", NULL, NULL, MXML_DESCEND, gameinfo.title_EN,sizeof(gameinfo.title_EN));
			GetTextFromNode(nodefound, nodedata, "synopsis", NULL, NULL, MXML_DESCEND, gameinfo.synopsis_EN,sizeof(gameinfo.synopsis_EN));
		}
		nodefound = mxmlFindElement(nodeid, nodedata, "locale", "lang", langcode, MXML_NO_DESCEND);
		if (nodefound != NULL) {
			GetTextFromNode(nodefound, nodedata, "title", NULL, NULL, MXML_DESCEND, gameinfo.title,sizeof(gameinfo.title));
			GetTextFromNode(nodefound, nodedata, "synopsis", NULL, NULL, MXML_DESCEND, gameinfo.synopsis,sizeof(gameinfo.synopsis));
		}
		// fall back to English title and synopsis if prefered language was not found
		if (!strcmp(gameinfo.title,"")) {
			strlcpy(gameinfo.title,gameinfo.title_EN,sizeof(gameinfo.title));
		}
		if (!strcmp(gameinfo.synopsis,"")) {
			strlcpy(gameinfo.synopsis,gameinfo.synopsis_EN,sizeof(gameinfo.synopsis));
		}

		/* list locale lang attributes */
		nodefound = mxmlFindElement(nodeid, nodedata, "locale", "lang", NULL, MXML_NO_DESCEND);
		if (nodefound != NULL) {
			int incr = 0;
			while (nodefound != NULL)
			{
				++incr;
				strlcpy(gameinfo.locales[incr],mxmlElementGetAttr(nodefound, "lang"),sizeof(gameinfo.locales[incr]));
				nodefound = mxmlWalkNext(nodefound, nodedata, MXML_NO_DESCEND);
				if (nodefound != NULL) {
					nodefound = mxmlFindElement(nodefound, nodedata, "locale", "lang", NULL, MXML_NO_DESCEND);
				}
			}
		}

		/* unbounded child elements */
		GetTextFromNode(nodeid, nodedata, "wi-fi", "players", NULL, MXML_NO_DESCEND, gameinfo.wifiplayers,sizeof(gameinfo.wifiplayers));
		nodefound = mxmlFindElement(nodeid, nodedata, "wi-fi", NULL, NULL, MXML_NO_DESCEND);
		if (nodefound != NULL) {
			gameinfo.wifiCnt = 0;
			nodeindextmp = mxmlIndexNew(nodefound,"feature", NULL);
			nodeidtmp = mxmlIndexReset(nodeindextmp);
			while (nodeidtmp != NULL)
			{
				nodeidtmp = mxmlIndexFind(nodeindextmp,"feature", NULL);
				if (nodeidtmp != NULL) {
					++gameinfo.wifiCnt;
					GetTextFromNode(nodeidtmp, nodedata, "feature", NULL, NULL, MXML_DESCEND, gameinfo.wififeatures[gameinfo.wifiCnt],
																					sizeof(gameinfo.wififeatures[gameinfo.wifiCnt]));
					gameinfo.wififeatures[gameinfo.wifiCnt][0] = (char)toupper((int)gameinfo.wififeatures[gameinfo.wifiCnt][0]);
					if (gameinfo.wifiCnt == XML_ELEMMAX)
						break;
				}
			}
			mxmlIndexDelete(nodeindextmp); // placed after each mxmlIndexNew to prevent memory leak
		}

		nodefound = mxmlFindElement(nodeid, nodedata, "rating", NULL, NULL, MXML_NO_DESCEND);
		if (nodefound != NULL) {
			gameinfo.descriptorCnt=0;
			nodeindextmp = mxmlIndexNew(nodefound,"descriptor", NULL);
			nodeidtmp = mxmlIndexReset(nodeindextmp);
			while (nodeidtmp != NULL)
			{
				nodeidtmp = mxmlIndexFind(nodeindextmp,"descriptor", NULL);
				if (nodeidtmp != NULL) {
					++gameinfo.descriptorCnt;
					GetTextFromNode(nodeidtmp, nodedata, "descriptor", NULL, NULL, MXML_DESCEND,
						gameinfo.ratingdescriptors[gameinfo.descriptorCnt], sizeof(gameinfo.ratingdescriptors[gameinfo.descriptorCnt]));
					if (gameinfo.descriptorCnt == XML_ELEMMAX)
						break;
				}
			}
			mxmlIndexDelete(nodeindextmp);
		}

		GetTextFromNode(nodeid, nodedata, "input", "players", NULL, MXML_NO_DESCEND, gameinfo.players,sizeof(gameinfo.players));
		nodefound = mxmlFindElement(nodeid, nodedata, "input", NULL, NULL, MXML_NO_DESCEND);
		if (nodefound != NULL) {
			gameinfo.accessoryCnt=0;
			gameinfo.accessoryReqCnt=0;
			nodeindextmp = mxmlIndexNew(nodefound,"control", NULL);
			nodeidtmp = mxmlIndexReset(nodeindextmp);
			while (nodeidtmp != NULL)
			{
				nodeidtmp = mxmlIndexFind(nodeindextmp,"control", NULL);
				if (nodeidtmp != NULL) {
					if (!strcmp(mxmlElementGetAttr(nodeidtmp, "required"),"true")  && gameinfo.accessoryReqCnt < XML_ELEMMAX)	{
						++gameinfo.accessoryReqCnt;
						strlcpy(gameinfo.accessoriesReq[gameinfo.accessoryReqCnt],mxmlElementGetAttr(nodeidtmp, "type"),
															sizeof(gameinfo.accessoriesReq[gameinfo.accessoryReqCnt]));
					} else if (gameinfo.accessoryCnt < XML_ELEMMAX) {
						++gameinfo.accessoryCnt;
						strlcpy(gameinfo.accessories[gameinfo.accessoryCnt],mxmlElementGetAttr(nodeidtmp, "type"),
															sizeof(gameinfo.accessories[gameinfo.accessoryCnt]));
					}
				}
			}
			mxmlIndexDelete(nodeindextmp);
		}

		/* convert rating value */
		ConvertRating(gameinfo.ratingvalue, gameinfo.ratingtype, "CERO",gameinfo.ratingvalueCERO,sizeof(gameinfo.ratingvalueCERO));
		ConvertRating(gameinfo.ratingvalue, gameinfo.ratingtype, "ESRB",gameinfo.ratingvalueESRB,sizeof(gameinfo.ratingvalueESRB));
		ConvertRating(gameinfo.ratingvalue, gameinfo.ratingtype, "PEGI",gameinfo.ratingvaluePEGI,sizeof(gameinfo.ratingvaluePEGI));

		/* provide genre as an array: gameinfo.genresplit */
		if (strcmp(gameinfo.genre,"") != 0){
			gameinfo.genreCnt=0;
			const char *delimgenre = ",;";
			char genretxt[200];
			strlcpy(genretxt,gameinfo.genre,sizeof(genretxt));
			char *splitresult;
			splitresult = strtok(genretxt, delimgenre);
			if (splitresult != NULL) {
				++gameinfo.genreCnt;
				trimcopy(splitresult,splitresult,strlen(splitresult)+1);
				strlcpy(gameinfo.genresplit[gameinfo.genreCnt],splitresult,sizeof(gameinfo.genresplit[gameinfo.genreCnt]));
				gameinfo.genresplit[gameinfo.genreCnt][0] = (char)toupper((int)gameinfo.genresplit[gameinfo.genreCnt][0]);
				while (splitresult != NULL)
				{
					splitresult = strtok(NULL, delimgenre);
					if (splitresult != NULL && strcmp(splitresult,"")!=0) {
						++gameinfo.genreCnt;
						trimcopy(splitresult,splitresult,strlen(splitresult)+1);
						strlcpy(gameinfo.genresplit[gameinfo.genreCnt],splitresult,sizeof(gameinfo.genresplit[gameinfo.genreCnt]));
						gameinfo.genresplit[gameinfo.genreCnt][0] = (char)toupper((int)gameinfo.genresplit[gameinfo.genreCnt][0]);
						if (gameinfo.genreCnt == XML_ELEMMAX)
							break;
					}
				}
			}
			
		}
		
		exist=1;
    } else {
	    /*game not found */
		exist=0;
	}

	// if game was not found or info is missing
	// guess publisher from game id in case it is missing
	if (!strcmp(gameinfo.publisher,"")) {
		GetPublisherFromGameid(gameid,gameinfo.publisherfromid,sizeof(gameinfo.publisherfromid));
		strlcpy(gameinfo.publisher,gameinfo.publisherfromid,sizeof(gameinfo.publisher));
	}

	// if missing, get region from game ID
	if (!strcmp(gameinfo.region,"")) {
		if (gameid[3] == 'E') strlcpy(gameinfo.region,"NTSC-U",sizeof(gameinfo.region));
		if (gameid[3] == 'J') strlcpy(gameinfo.region,"NTSC-J",sizeof(gameinfo.region));
		if (gameid[3] == 'K') strlcpy(gameinfo.region,"NTSC-K",sizeof(gameinfo.region));
		if (gameid[3] == 'P') strlcpy(gameinfo.region,"PAL",sizeof(gameinfo.region));
		if (gameid[3] == 'X') strlcpy(gameinfo.region,"PAL-X",sizeof(gameinfo.region)); // does X appear in any original game ID?
		// the following codes do not exist and should only appear on modified game IDs
		if (gameid[3] == 'D') strlcpy(gameinfo.region,"PAL",sizeof(gameinfo.region));
		if (gameid[3] == 'G') strlcpy(gameinfo.region,"PAL",sizeof(gameinfo.region));
		if (gameid[3] == 'F') strlcpy(gameinfo.region,"PAL",sizeof(gameinfo.region));
		if (gameid[3] == 'S') strlcpy(gameinfo.region,"PAL",sizeof(gameinfo.region));
		if (gameid[3] == 'I') strlcpy(gameinfo.region,"PAL",sizeof(gameinfo.region));
		if (gameid[3] == 'N') strlcpy(gameinfo.region,"PAL",sizeof(gameinfo.region));
	}
	
	// free memory
	mxmlIndexDelete(nodeindex);

	return exist;
}




/*-------------------------------------------------------------------------------------*/
/* get_text() - Get the text for a node, taken from mini-mxml example mxmldoc.c */
static char * get_text(mxml_node_t *node, char *buffer, int buflen) /* O - Text in node, I - Node to get, I - Buffer, I - Size of buffer */	
{
  char *ptr, *end;	/* Pointer into buffer, End of buffer */	
  int len;	/* Length of node */
  mxml_node_t *current;	/* Current node */
  ptr = buffer;
  end = buffer + buflen - 1;
  for (current = node->child; current && ptr < end; current = current->next)
  {
    if (current->type == MXML_TEXT) {
      if (current->value.text.whitespace)
		*ptr++ = ' ';
      len = (int)strlen(current->value.text.string);
      if (len > (int)(end - ptr))
        len = (int)(end - ptr);
      memcpy(ptr, current->value.text.string, len);
      ptr += len;
    } else if (current->type == MXML_OPAQUE) {
      len = (int)strlen(current->value.opaque);
      if (len > (int)(end - ptr))
        len = (int)(end - ptr);
      memcpy(ptr, current->value.opaque, len);
      ptr += len;
    }
  }
  *ptr = '\0';
  return (buffer);
}

