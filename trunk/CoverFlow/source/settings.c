#include "settings.h"
#include "mxml.h"

/* my save callback */
const char *
whitespace_cb(mxml_node_t *node,
			  int where)
{
	const char *name;

	name = node->value.element.name;

	if (!strcmp(name, "wiicoverflow"))
	{
			return ("\n");
	}
	else if (!strcmp(name, "graphics") ||
			 !strcmp(name, "general") ||
			 !strcmp(name, "game"))
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

inline void SETTINGS_Init()
{
	//Graphics
	SETTING_coverZoom    = -2.0;
	SETTING_coverAngle   = 90;
	SETTING_coverSpacing = 3.1;
	SETTING_drawWindow   = 7;
	SETTING_coverText    = 1;
	SETTING_theme		 = 0; // default to black
	SETTING_fontColor    = 0xFFFFFFFF; // default to white
	
	//General
	SETTING_rumble       = 1;
	SETTING_parentalLock = 0;
	SETTING_sound        = 1;
	SETTING_music        = 1;
	SETTING_quickstart   = 0;
	SETTING_enablepitch  = 0;

	//Game
	SETTING_ocarina      = 0;
	SETTING_hooktype     = 0;
	SETTING_language     = 0;
	SETTING_video        = 0;
	SETTING_vipatch      = 0;
}

inline int SETTINGS_Load()
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
			  SETTING_coverSpacing = atof(mxmlElementGetAttr(next_n,"spacing"));
		  if(mxmlElementGetAttr(next_n,"angle"))
			  SETTING_coverAngle   = atof(mxmlElementGetAttr(next_n,"angle"));
		  if(mxmlElementGetAttr(next_n,"zoom"))
			  SETTING_coverZoom    = atof(mxmlElementGetAttr(next_n,"zoom"));
		  if(mxmlElementGetAttr(next_n,"drawWindow"))
			  SETTING_drawWindow   = atoi(mxmlElementGetAttr(next_n,"drawWindow"));
		  if(mxmlElementGetAttr(next_n,"coverText"))
			  SETTING_coverText    = atoi(mxmlElementGetAttr(next_n,"coverText"));
		  if(mxmlElementGetAttr(next_n,"theme"))
			  SETTING_theme		   = atoi(mxmlElementGetAttr(next_n,"theme"));
		  if(mxmlElementGetAttr(next_n,"enablepitch"))
			  SETTING_enablepitch  = atoi(mxmlElementGetAttr(next_n,"enablepitch"));
		  if (SETTING_theme)
		  {
			  SETTING_fontColor = 0x000000FF;
		  }
		  else
		  {
			  SETTING_fontColor = 0xFFFFFFFF;
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
			  SETTING_sound = atoi(mxmlElementGetAttr(next_n,"sound"));
		  if(mxmlElementGetAttr(next_n,"music"))
			  SETTING_sound = atoi(mxmlElementGetAttr(next_n,"music"));
		  if(mxmlElementGetAttr(next_n,"rumble"))
			  SETTING_rumble   = atof(mxmlElementGetAttr(next_n,"rumble"));
		  if(mxmlElementGetAttr(next_n,"lock"))
			  SETTING_parentalLock   = atof(mxmlElementGetAttr(next_n,"lock"));
		  if(mxmlElementGetAttr(next_n,"quickstart"))
			  SETTING_quickstart     = atof(mxmlElementGetAttr(next_n,"quickstart"));
		  if(mxmlElementGetAttr(next_n,"enablepitch"))
			  SETTING_enablepitch     = atof(mxmlElementGetAttr(next_n,"enablepitch"));
	  }
	  else
	  {
		 return -1;
	  }
	  next_n = mxmlFindElement(node, node, "game", NULL, NULL, MXML_DESCEND);

	  if(next_n != NULL)
	  {
		  if(mxmlElementGetAttr(next_n,"ocarina"))
			  SETTING_ocarina = atoi(mxmlElementGetAttr(next_n,"ocarina"));
		  if(mxmlElementGetAttr(next_n,"hooktype"))
			  SETTING_hooktype = atoi(mxmlElementGetAttr(next_n,"hooktype"));
		  if(mxmlElementGetAttr(next_n,"language"))
			  SETTING_language   = atof(mxmlElementGetAttr(next_n,"language"));
		  if(mxmlElementGetAttr(next_n,"video"))
			  SETTING_video   = atof(mxmlElementGetAttr(next_n,"video"));
		  if(mxmlElementGetAttr(next_n,"vipatch"))
			  SETTING_vipatch     = atof(mxmlElementGetAttr(next_n,"vipatch"));
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

inline int SETTINGS_Save()
{
	mxml_node_t *xml;
	mxml_node_t *tree;
	mxml_node_t *node;
	
    char buffer[50];
	
	xml = mxmlNewXML("1.0");
	
	tree = mxmlNewElement(xml, "wiicoverflow");
	
	//GRAPHIC SETTINGS
	node = mxmlNewElement(tree, "graphics");
		
	sprintf(buffer, "%f", SETTING_coverSpacing);
	mxmlElementSetAttr(node, "spacing", buffer);
		
	sprintf(buffer, "%f", SETTING_coverAngle);
	mxmlElementSetAttr(node, "angle", buffer);
	
	sprintf(buffer, "%f", SETTING_coverZoom);
	mxmlElementSetAttr(node, "zoom", buffer);
	
	sprintf(buffer, "%d", SETTING_drawWindow);
	mxmlElementSetAttr(node, "drawWindow", buffer);
	
	sprintf(buffer, "%d", SETTING_coverText);
	mxmlElementSetAttr(node, "coverText", buffer);

	sprintf(buffer, "%d", SETTING_theme);
	mxmlElementSetAttr(node, "theme", buffer);

    //GENERAL SETTINGS
	node = mxmlNewElement(tree, "general");
	sprintf(buffer, "%d", SETTING_sound);
	mxmlElementSetAttr(node, "sound", buffer);
	
	sprintf(buffer, "%d", SETTING_music);
	mxmlElementSetAttr(node, "music", buffer);
	
	sprintf(buffer, "%d", SETTING_rumble);
	mxmlElementSetAttr(node, "rumble", buffer);
	
	sprintf(buffer, "%d", SETTING_parentalLock);
	mxmlElementSetAttr(node, "lock", buffer);

	sprintf(buffer, "%d", SETTING_quickstart);
	mxmlElementSetAttr(node, "quickstart", buffer);

	sprintf(buffer, "%d", SETTING_enablepitch);
	mxmlElementSetAttr(node, "enablepitch", buffer);

    //GAME SETTINGS
	node = mxmlNewElement(tree, "game");
	sprintf(buffer, "%d", SETTING_ocarina);
	mxmlElementSetAttr(node, "ocarina", buffer);

	sprintf(buffer, "%d", SETTING_hooktype);
	mxmlElementSetAttr(node, "hooktype", buffer);

	sprintf(buffer, "%d", SETTING_language);
	mxmlElementSetAttr(node, "language", buffer);

	sprintf(buffer, "%d", SETTING_video);
	mxmlElementSetAttr(node, "video", buffer);

	sprintf(buffer, "%d", SETTING_vipatch);
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
