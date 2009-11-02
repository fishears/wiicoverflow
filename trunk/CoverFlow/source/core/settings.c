#include "settings.h"
#include "mxml.h"

s_settings settings;
gp_settings gpsettings;
extern s_path dynPath;

/* my save callback */
const char* whitespace_cb(mxml_node_t *node, int where)
{
	const char *name;
	name = node->value.element.name;

	if (!strcmp(name, "wiicoverflow"))
		return ("\n");	
	else if (!strcmp(name, "graphics") 	||
			 !strcmp(name, "general")  	||
			 !strcmp(name, "game")		||
			 !strcmp(name, "paths"))
	{
		if (where == MXML_WS_BEFORE_OPEN)
			return ("\t");
		else if (where == MXML_WS_AFTER_OPEN)
			return ("\n");
		if (where == MXML_WS_BEFORE_CLOSE)
			return ("\t");
		else if (where == MXML_WS_AFTER_CLOSE)
			return ("\n");
	}
	
	return (NULL);
}

void SETTINGS_Init()
{
	//Graphics
	settings.coverZoom    = -2.0;
	settings.mainCoverZoom = 0.0;
	settings.coverAngle   = 90;
	settings.coverSpacing = 3.1;
	settings.coverFallOff = 0.0;
	settings.falldist     = 4;
	settings.drawWindow   = 13;
	settings.coverText    = 1;
	settings.autoUpdate   = 1;
	settings.theme		  = 0; // default to black
	settings.covers3d     = 0;
	settings.covers3dThickness = 0; //default to 0=Fat, 1=flat
	settings.hideScroll   = 1;
	//General
	settings.parentalLock = 0;
	settings.volume       = 100;
	settings.sound        = 1;
	settings.music        = 1;
	settings.quickstart   = 0;
	settings.enablepitch  = 0;
	settings.auto_hide    = 1;
	
	strcpy(settings.localLanguage, "default"); // en-US 
 	//Global Game
	settings.ocarina      = 0;
        settings.loser        = 0;
	settings.hooktype     = 0;
	settings.language     = 0;
	settings.video        = 0;
	settings.vipatch      = 0;
	settings.cios		  = 0;
	strcpy(settings.lastplayed,"");
	settings.presetFix	  = 0;
#ifdef NEWS_READER
	strcpy(settings.newsDate, "0");
	strcpy(settings.newsID,   "0");
#endif
};
//called by reset button on graphics settings screen
//button used to reset EVERYTHING
void graphics_SETTINGS_Init()
{
    settings.coverZoom    		= -2.0;
	settings.mainCoverZoom		= 0.0;
    settings.coverCamY    		= 0.0;
    settings.coverCamX   		= 0.0;
	settings.coverAngle   		= 90;
	settings.coverSpacing 		= 3.1;
	settings.coverFallOff 		= 0.0;
	settings.falldist           = 4;
	settings.drawWindow   		= 7;
	settings.coverText    		= 1;
	settings.hideScroll   		= 1;
	settings.covers3dThickness 	= 0;
}


void backup_gpSettings()
{
    gpsettings.coverZoom    	 = settings.coverZoom;
    gpsettings.mainCoverZoom     = settings.mainCoverZoom;
	gpsettings.coverAngle   	 = settings.coverAngle;
	gpsettings.coverSpacing 	 = settings.coverSpacing;
	gpsettings.coverFallOff 	 = settings.coverFallOff;
	gpsettings.falldist          = settings.falldist;
	gpsettings.drawWindow   	 = settings.drawWindow;
	gpsettings.coverText    	 = settings.coverText;
	gpsettings.hideScroll   	 = settings.hideScroll;
	gpsettings.covers3dThickness = settings.covers3dThickness;
}

void restore_gpSettings()
{
    settings.coverZoom    		= gpsettings.coverZoom;
    settings.mainCoverZoom    	= gpsettings.mainCoverZoom;
	settings.coverAngle   		= gpsettings.coverAngle;
	settings.coverSpacing 		= gpsettings.coverSpacing;
	settings.coverFallOff 		= gpsettings.coverFallOff;
	settings.falldist           = gpsettings.falldist;
	settings.drawWindow   		= gpsettings.drawWindow;
	settings.coverText    		= gpsettings.coverText;
	settings.hideScroll   		= gpsettings.hideScroll;
	settings.covers3dThickness  = gpsettings.covers3dThickness;
}


int SETTINGS_Load()
{
  FILE *fp;
  mxml_node_t *xml;
  char buffer[255];
	
  sprintf(buffer,"%s/wiicoverflow.xml", dynPath.dir_usb_loader);
  fp = fopen( buffer, "r");
  //fp = fopen(USBLOADER_PATH "/wiicoverflow.xml", "r");
  
  if(fp == NULL)
     return -1;
  
  xml = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
 
  fclose(fp);
		
  if(xml != NULL)
  {
	  mxml_node_t *node;
	  mxml_node_t *next_n;
		
	  node = mxmlFindElement(xml,xml, "wiicoverflow", NULL, NULL, MXML_DESCEND); 
	  if(node == NULL) return -1;
	  
	  next_n = mxmlFindElement(node, node, "graphics", NULL, NULL, MXML_DESCEND); 
	  
	  if(next_n != NULL)
	  {
		  if(mxmlElementGetAttr(next_n,"spacing"))
			  settings.coverSpacing = atof(mxmlElementGetAttr(next_n,"spacing"));
		  if(mxmlElementGetAttr(next_n,"angle"))
			  settings.coverAngle   = atof(mxmlElementGetAttr(next_n,"angle"));
		  if(mxmlElementGetAttr(next_n,"coverzoom"))
			  settings.mainCoverZoom  = atof(mxmlElementGetAttr(next_n,"coverzoom"));
		  if(mxmlElementGetAttr(next_n,"zoom"))
			  settings.coverZoom    = atof(mxmlElementGetAttr(next_n,"zoom"));
		  if(mxmlElementGetAttr(next_n,"falloff"))
			  settings.coverFallOff = atof(mxmlElementGetAttr(next_n,"falloff"));
		  if(mxmlElementGetAttr(next_n,"falldist"))
			  settings.falldist = atoi(mxmlElementGetAttr(next_n,"falldist"));
		  if(mxmlElementGetAttr(next_n,"drawWindow"))
			  settings.drawWindow   = atoi(mxmlElementGetAttr(next_n,"drawWindow"));
		  if(mxmlElementGetAttr(next_n,"coverText"))
			  settings.coverText    = atoi(mxmlElementGetAttr(next_n,"coverText"));
		  if(mxmlElementGetAttr(next_n,"theme"))
			  settings.theme		   = atoi(mxmlElementGetAttr(next_n,"theme"));
		  if(mxmlElementGetAttr(next_n,"autohide"))
			  settings.auto_hide 	   = atoi(mxmlElementGetAttr(next_n,"autohide"));
		  if(mxmlElementGetAttr(next_n,"enablepitch"))
			  settings.enablepitch  = atoi(mxmlElementGetAttr(next_n,"enablepitch"));
		  if(mxmlElementGetAttr(next_n,"c3d"))
			  settings.covers3d  = atoi(mxmlElementGetAttr(next_n,"c3d"));
		  if(mxmlElementGetAttr(next_n,"c3dThickness"))
			  settings.covers3dThickness  = atoi(mxmlElementGetAttr(next_n,"c3dThickness"));
		  if(mxmlElementGetAttr(next_n,"hideScroll"))
			  settings.hideScroll  = atoi(mxmlElementGetAttr(next_n,"hideScroll"));
	  }
	  else
	  {
		 return -1;
	  }
	  
	  next_n = mxmlFindElement(node, node, "general", NULL, NULL, MXML_DESCEND); 
	  
	  if(next_n != NULL)
	  {
		  if(mxmlElementGetAttr(next_n,"sound"))
			  settings.sound = atoi(mxmlElementGetAttr(next_n,"sound"));
		  if(mxmlElementGetAttr(next_n,"music"))
			  settings.music = atoi(mxmlElementGetAttr(next_n,"music"));
		  if(mxmlElementGetAttr(next_n,"rumble"))
			  settings.rumble   = atof(mxmlElementGetAttr(next_n,"rumble"));
		  if(mxmlElementGetAttr(next_n,"lock"))
			  settings.parentalLock   = atof(mxmlElementGetAttr(next_n,"lock"));
		  if(mxmlElementGetAttr(next_n,"quickstart"))
			  settings.quickstart     = atof(mxmlElementGetAttr(next_n,"quickstart"));
		  if(mxmlElementGetAttr(next_n,"enablepitch"))
			  settings.enablepitch     = atof(mxmlElementGetAttr(next_n,"enablepitch"));
		  if(mxmlElementGetAttr(next_n,"localizeLang"))
			  strcpy(settings.localLanguage, mxmlElementGetAttr(next_n,"localizeLang"));
#ifdef NEWS_READER	  
		  if(mxmlElementGetAttr(next_n,"newsID"))
			  strcpy(settings.newsID, mxmlElementGetAttr(next_n,"newsID"));
		  if(mxmlElementGetAttr(next_n,"newsDate"))
			  strcpy(settings.newsDate, mxmlElementGetAttr(next_n,"newsDate"));
#endif	  
		  if(mxmlElementGetAttr(next_n,"autoUpdate"))
			  settings.autoUpdate  = atoi(mxmlElementGetAttr(next_n,"autoUpdate"));
		  if(mxmlElementGetAttr(next_n,"cios"))
			  settings.cios  = atoi(mxmlElementGetAttr(next_n,"cios"));
	  }
	  else
	  {
		 return -1;
	  }
	  
	  next_n = mxmlFindElement(node, node, "game", NULL, NULL, MXML_DESCEND);

	  if(next_n != NULL)
	  {
		  
		  if(mxmlElementGetAttr(next_n,"ocarina"))
			  settings.ocarina = atoi(mxmlElementGetAttr(next_n,"ocarina"));
                  if(mxmlElementGetAttr(next_n,"loser"))
			  settings.loser = atoi(mxmlElementGetAttr(next_n,"loser"));
		  if(mxmlElementGetAttr(next_n,"hooktype"))
			  settings.hooktype = atoi(mxmlElementGetAttr(next_n,"hooktype"));
		  if(mxmlElementGetAttr(next_n,"language"))
			  settings.language   = atof(mxmlElementGetAttr(next_n,"language"));
		  if(mxmlElementGetAttr(next_n,"video"))
			  settings.video   = atof(mxmlElementGetAttr(next_n,"video"));
		  if(mxmlElementGetAttr(next_n,"vipatch"))
			  settings.vipatch     = atof(mxmlElementGetAttr(next_n,"vipatch"));
          if(mxmlElementGetAttr(next_n,"lastplayed"))
			  strcpy(settings.lastplayed, mxmlElementGetAttr(next_n,"lastplayed"));
		  if(mxmlElementGetAttr(next_n,"presetFix"))
		  {
			  settings.presetFix = atoi(mxmlElementGetAttr(next_n,"presetFix"));
			  if ( (settings.presetFix < 0) || (settings.presetFix > (CFG_FIX_COUNT -1)) )
				settings.presetFix = 0;
		  }
	  
	  }
	  else
	  {
		 return -1;
	  }
	  
	  // PATHS
	  next_n = mxmlFindElement(node, node, "paths", NULL, NULL, MXML_DESCEND);

	  if(next_n != NULL)
	  {
		if(mxmlElementGetAttr(next_n,"dir_usb_loader"))
			strcpy(dynPath.dir_usb_loader, mxmlElementGetAttr(next_n,"dir_usb_loader"));
			
		if(mxmlElementGetAttr(next_n,"dir_codes"))
			strcpy(dynPath.dir_codes, mxmlElementGetAttr(next_n,"dir_codes"));
			
		if(mxmlElementGetAttr(next_n,"dir_3dcovers"))
			strcpy(dynPath.dir_3dcovers, mxmlElementGetAttr(next_n,"dir_3dcovers"));
			
		if(mxmlElementGetAttr(next_n,"dir_covers"))
			strcpy(dynPath.dir_covers, mxmlElementGetAttr(next_n,"dir_covers"));
			
		if(mxmlElementGetAttr(next_n,"dir_disks"))
			strcpy(dynPath.dir_disks, mxmlElementGetAttr(next_n,"dir_disks"));
			
		if(mxmlElementGetAttr(next_n,"dir_games"))
			strcpy(dynPath.dir_games, mxmlElementGetAttr(next_n,"dir_games"));
			
		if(mxmlElementGetAttr(next_n,"dir_txtcodes"))
			strcpy(dynPath.dir_txtcodes, mxmlElementGetAttr(next_n,"dir_txtcodes"));
			
		if(mxmlElementGetAttr(next_n,"dir_altdol"))
			strcpy(dynPath.dir_altdol, mxmlElementGetAttr(next_n,"dir_altdol"));
	  }
	  else
	  {
		 return -1;
	  }

	 
	  return 1;
  }
  else
  {
	return -1;
  }
  
  return -1;
}



int SETTINGS_Save()
{
	mxml_node_t *xml;
	mxml_node_t *tree;
	mxml_node_t *node;
	
    char buffer[50];
	char fbuff [255];
	
	xml = mxmlNewXML("1.0");
	
	tree = mxmlNewElement(xml, "wiicoverflow");
	
	//GRAPHIC SETTINGS
	node = mxmlNewElement(tree, "graphics");
		
	sprintf(buffer, "%f", settings.coverSpacing);
	mxmlElementSetAttr(node, "spacing", buffer);
		
	sprintf(buffer, "%f", settings.coverAngle);
	mxmlElementSetAttr(node, "angle", buffer);
	
	sprintf(buffer, "%f", settings.coverZoom);
	mxmlElementSetAttr(node, "zoom", buffer);
	
	sprintf(buffer, "%f", settings.mainCoverZoom);
	mxmlElementSetAttr(node, "coverzoom", buffer);
	
	sprintf(buffer, "%f", settings.coverFallOff);
	mxmlElementSetAttr(node, "falloff", buffer);
	
	sprintf(buffer, "%d", settings.falldist);
	mxmlElementSetAttr(node, "falldist", buffer);
	
	sprintf(buffer, "%d", settings.drawWindow);
	mxmlElementSetAttr(node, "drawWindow", buffer);
	
	sprintf(buffer, "%d", settings.coverText);
	mxmlElementSetAttr(node, "coverText", buffer);

	sprintf(buffer, "%d", settings.theme);
	mxmlElementSetAttr(node, "theme", buffer);

	sprintf(buffer, "%d", settings.auto_hide);
	mxmlElementSetAttr(node, "autohide", buffer);
	
	sprintf(buffer, "%d", settings.covers3d);
	mxmlElementSetAttr(node, "c3d", buffer);

	sprintf(buffer, "%d", settings.covers3dThickness);
	mxmlElementSetAttr(node, "c3dThickness", buffer);
	
	sprintf(buffer, "%d", settings.hideScroll);
	mxmlElementSetAttr(node, "hideScroll", buffer);
	
    //GENERAL SETTINGS
	node = mxmlNewElement(tree, "general");
	sprintf(buffer, "%d", settings.sound);
	mxmlElementSetAttr(node, "sound", buffer);
	
	sprintf(buffer, "%d", settings.autoUpdate);
	mxmlElementSetAttr(node, "autoUpdate", buffer);
			  
	sprintf(buffer, "%d", settings.music);
	mxmlElementSetAttr(node, "music", buffer);
	
	sprintf(buffer, "%d", settings.rumble);
	mxmlElementSetAttr(node, "rumble", buffer);
	
	sprintf(buffer, "%d", settings.parentalLock);
	mxmlElementSetAttr(node, "lock", buffer);

	sprintf(buffer, "%d", settings.quickstart);
	mxmlElementSetAttr(node, "quickstart", buffer);

	sprintf(buffer, "%d", settings.enablepitch);
	mxmlElementSetAttr(node, "enablepitch", buffer);

	mxmlElementSetAttr(node, "localizeLang", settings.localLanguage);
	
	sprintf(buffer, "%d", SVN_VERSION);
	mxmlElementSetAttr(node, "rev", buffer);	

#ifdef NEWS_READER	
	mxmlElementSetAttr(node, "newsDate", settings.newsDate);
	mxmlElementSetAttr(node, "newsID",   settings.newsID);
#endif
	sprintf(buffer, "%d", settings.cios);
	mxmlElementSetAttr(node, "cios", buffer);

    //GLOBAL GAME SETTINGS

	node = mxmlNewElement(tree, "game");
	sprintf(buffer, "%d", settings.ocarina);
	mxmlElementSetAttr(node, "ocarina", buffer);

        sprintf(buffer, "%d", settings.loser);
	mxmlElementSetAttr(node, "loser", buffer);

	sprintf(buffer, "%d", settings.hooktype);
	mxmlElementSetAttr(node, "hooktype", buffer);

	sprintf(buffer, "%d", settings.language);
	mxmlElementSetAttr(node, "language", buffer);

	sprintf(buffer, "%d", settings.video);
	mxmlElementSetAttr(node, "video", buffer);

	sprintf(buffer, "%d", settings.vipatch);
	mxmlElementSetAttr(node, "vipatch", buffer);

	mxmlElementSetAttr(node, "lastplayed", settings.lastplayed);

	sprintf(buffer, "%d", settings.presetFix);
	mxmlElementSetAttr(node, "presetFix", buffer);
	
	//PATHS SETTINGS
	node = mxmlNewElement(tree, "paths");
	mxmlElementSetAttr(node, "dir_usb_loader", 	dynPath.dir_usb_loader);
	mxmlElementSetAttr(node, "dir_codes", 		dynPath.dir_codes);
	mxmlElementSetAttr(node, "dir_3dcovers", 	dynPath.dir_3dcovers);
	mxmlElementSetAttr(node, "dir_covers", 		dynPath.dir_covers);
	mxmlElementSetAttr(node, "dir_disks", 		dynPath.dir_disks);
	mxmlElementSetAttr(node, "dir_games", 		dynPath.dir_games);
	mxmlElementSetAttr(node, "dir_txtcodes", 	dynPath.dir_txtcodes);
	mxmlElementSetAttr(node, "dir_altdol", 		dynPath.dir_altdol);
	
	
	FILE *fp;

	
    sprintf(fbuff,"%s/wiicoverflow.xml", dynPath.dir_usb_loader);
    fp = fopen( fbuff, "w");
	//fp = fopen(USBLOADER_PATH "/wiicoverflow.xml", "w");
        
	if(fp != NULL)
	{
		mxmlSaveFile(xml, fp, whitespace_cb);
	
		fclose(fp);
	
		return 1;
	}
	else
	{
		return -1;
	}
}



void createEmptyWiiCoverFlowFile()
{
	char buf[30];
	mxml_node_t *xml;
	mxml_node_t *tree;
	mxml_node_t *node;
	char fbuff[255];
	FILE *fp;
	
	xml = mxmlNewXML("1.0");
	tree = mxmlNewElement(xml, "wiicoverflow");
	node = mxmlNewElement(tree, "graphics");
	node = mxmlNewElement(tree, "general");
	sprintf(buf, "%d", SVN_VERSION);
	mxmlElementSetAttr(node, "rev", buf);	
	node = mxmlNewElement(tree, "game");
	node = mxmlNewElement(tree, "paths");
	
	sprintf(fbuff,"%s/wiicoverflow.xml", dynPath.dir_usb_loader );
	fp = fopen(fbuff, "w");
//	fp = fopen(USBLOADER_PATH "/wiicoverflow.xml", "w");
        
	if(fp != NULL){
		mxmlSaveFile(xml, fp, whitespace_cb);
		fclose(fp);
	}
	else
		WindowPrompt(TX.error, TX.noCreateWCFXML, 0, &okButton);
}


int getRevXML()
 {
   FILE *fp;
   mxml_node_t *xml;
   int rev;
   char buffer[255];
	
   sprintf(buffer,"%s/wiicoverflow.xml", dynPath.dir_usb_loader);
   fp = fopen( buffer, "r");
   //fp = fopen(USBLOADER_PATH "/wiicoverflow.xml", "r");
   
   if(fp == NULL)
      return -1;
   
   xml = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
  
   fclose(fp);
                 
   if(xml != NULL)
    {
	   mxml_node_t *node;
	   mxml_node_t *next_n;
			 
	   node = mxmlFindElement(xml,xml, "wiicoverflow", NULL, NULL, MXML_DESCEND); 
	   if(node == NULL) return -1;
	   
	   next_n = mxmlFindElement(node, node, "general", NULL, NULL, MXML_DESCEND); 
	   
	   if(next_n != NULL)
	     {
		   if(mxmlElementGetAttr(next_n,"rev"))
			   {
				rev = atoi(mxmlElementGetAttr(next_n,"rev"));
			   }
		   else  
			   {
				rev = 0;  //nil      
			   }         
	     }
	   else
	     {
		   return -1;
	     } 
	   return rev;
    }
   else
    {
      return -1;
    }
   
   return -1;
 }
 


int getLangXML()
{
  FILE *fp;
  mxml_node_t *xml;
  char buffer[255];
	
  sprintf(buffer,"%s/wiicoverflow.xml", dynPath.dir_usb_loader);
  fp = fopen( buffer, "r");
  //fp = fopen(USBLOADER_PATH "/wiicoverflow.xml", "r");
  
  if(fp == NULL)
     return -1;
  
  xml = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
 
  fclose(fp);
		
  if(xml != NULL)
  {
	  mxml_node_t *node;
	  mxml_node_t *next_n;
		
	  node = mxmlFindElement(xml,xml, "wiicoverflow", NULL, NULL, MXML_DESCEND); 
	  if(node == NULL) return -1;
	  
	  next_n = mxmlFindElement(node, node, "general", NULL, NULL, MXML_DESCEND); 
	  
	  if(next_n != NULL)
	  {
		  
		  if(mxmlElementGetAttr(next_n,"localizeLang"))
			  {
			   strcpy(settings.localLanguage, mxmlElementGetAttr(next_n,"localizeLang"));
			   return 1;
			  }
		  else
              {
			   strcpy(settings.localLanguage, "default");
			   return -1;
			  }	
	  }
	  else
	  {
		 return -1;
	  }
  }
  else
  {
	return -1;
  }
  
  return -1;
}


