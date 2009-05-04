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
	else if (!strcmp(name, "graphics"))
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
	SETTING_coverZoom    = -2.0;
	SETTING_coverAngle   = 90;
	SETTING_coverSpacing = 3.1;
	SETTING_drawWindow   = 7;
	SETTING_coverText    = 1;
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
	  
	  //Add cake
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
		 SETTING_coverText   = atoi(mxmlElementGetAttr(next_n,"coverText"));
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
