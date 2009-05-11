#include "settings.h"
#include "mxml.h"


s_settings settings;
/* my save callback */
const char* whitespace_cb(mxml_node_t *node, int where)
{
	const char *name;
	name = node->value.element.name;

	if (!strcmp(name, "wiicoverflow"))
		return ("\n");	
	else if (!strcmp(name, "graphics") ||
			 !strcmp(name, "general"))// ||
			 //!strcmp(name, "game"))
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
	settings.coverAngle   = 90;
	settings.coverSpacing = 3.1;
	settings.drawWindow   = 7;
	settings.coverText    = 1;
	settings.theme		 = 0; // default to black
	settings.fontColor    = 0xFFFFFFFF; // default to white
	
	settings.volume = 100;
	
	//General
	settings.rumble       = 1;
	settings.parentalLock = 0;
	settings.sound        = 1;
	settings.music        = 1;
	settings.quickstart   = 0;
	settings.enablepitch  = 0;

	//Global Game
	settings.ocarina      = 0;
	settings.hooktype     = 0;
	settings.language     = 0;
	settings.video        = 0;
	settings.vipatch      = 0;
};

int SETTINGS_Load()
{
  FILE *fp;
  mxml_node_t *xml;

  fp = fopen("wiicoverflow.xml", "r");
  
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
		  if(mxmlElementGetAttr(next_n,"zoom"))
			  settings.coverZoom    = atof(mxmlElementGetAttr(next_n,"zoom"));
		  if(mxmlElementGetAttr(next_n,"drawWindow"))
			  settings.drawWindow   = atoi(mxmlElementGetAttr(next_n,"drawWindow"));
		  if(mxmlElementGetAttr(next_n,"coverText"))
			  settings.coverText    = atoi(mxmlElementGetAttr(next_n,"coverText"));
		  if(mxmlElementGetAttr(next_n,"theme"))
			  settings.theme		   = atoi(mxmlElementGetAttr(next_n,"theme"));
		  if(mxmlElementGetAttr(next_n,"enablepitch"))
			  settings.enablepitch  = atoi(mxmlElementGetAttr(next_n,"enablepitch"));
		  if (settings.theme)
		  {
			  settings.fontColor = 0x000000FF;
		  }
		  else
		  {
			  settings.fontColor = 0xFFFFFFFF;
		  }
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
			  settings.sound = atoi(mxmlElementGetAttr(next_n,"music"));
		  if(mxmlElementGetAttr(next_n,"rumble"))
			  settings.rumble   = atof(mxmlElementGetAttr(next_n,"rumble"));
		  if(mxmlElementGetAttr(next_n,"lock"))
			  settings.parentalLock   = atof(mxmlElementGetAttr(next_n,"lock"));
		  if(mxmlElementGetAttr(next_n,"quickstart"))
			  settings.quickstart     = atof(mxmlElementGetAttr(next_n,"quickstart"));
		  if(mxmlElementGetAttr(next_n,"enablepitch"))
			  settings.enablepitch     = atof(mxmlElementGetAttr(next_n,"enablepitch"));
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
		  if(mxmlElementGetAttr(next_n,"hooktype"))
			  settings.hooktype = atoi(mxmlElementGetAttr(next_n,"hooktype"));
		  if(mxmlElementGetAttr(next_n,"language"))
			  settings.language   = atof(mxmlElementGetAttr(next_n,"language"));
		  if(mxmlElementGetAttr(next_n,"video"))
			  settings.video   = atof(mxmlElementGetAttr(next_n,"video"));
		  if(mxmlElementGetAttr(next_n,"vipatch"))
			  settings.vipatch     = atof(mxmlElementGetAttr(next_n,"vipatch"));
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
	
	sprintf(buffer, "%d", settings.drawWindow);
	mxmlElementSetAttr(node, "drawWindow", buffer);
	
	sprintf(buffer, "%d", settings.coverText);
	mxmlElementSetAttr(node, "coverText", buffer);

	sprintf(buffer, "%d", settings.theme);
	mxmlElementSetAttr(node, "theme", buffer);

    //GENERAL SETTINGS
	node = mxmlNewElement(tree, "general");
	sprintf(buffer, "%d", settings.sound);
	mxmlElementSetAttr(node, "sound", buffer);
	
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

    //GLOBAL GAME SETTINGS
	
	node = mxmlNewElement(tree, "game");
	sprintf(buffer, "%d", settings.ocarina);
	mxmlElementSetAttr(node, "ocarina", buffer);

	sprintf(buffer, "%d", settings.hooktype);
	mxmlElementSetAttr(node, "hooktype", buffer);

	sprintf(buffer, "%d", settings.language);
	mxmlElementSetAttr(node, "language", buffer);

	sprintf(buffer, "%d", settings.video);
	mxmlElementSetAttr(node, "video", buffer);

	sprintf(buffer, "%d", settings.vipatch);
	mxmlElementSetAttr(node, "vipatch", buffer);
	
	FILE *fp;

	fp = fopen("wiicoverflow.xml", "w");
        
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
