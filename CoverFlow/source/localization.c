/**************************************************************
 **  localization.c - Loads international messages from file **
 **  Authors: Alex Antao - alexcarlosantao@gmail.com         **
 **           LoudBob11                                      **
 **************************************************************/

#include "localization.h"

extern s_settings settings;
static mxml_node_t *MSGs = NULL;
bool missingFile=false;

char *localStr ( char *CODE, char * defaultStr )
{
return defaultStr;
   //mxml_node_t *root;
   mxml_node_t *node;
   mxml_node_t *lang_n;
   mxml_node_t *msg_n;

   if (MSGs == NULL || missingFile) {
	   FILE *fp;

	   fp = fopen(USBLOADER_PATH "/localization.xml", "r");

	   if(fp == NULL)
	   {
			missingFile=true;
			return defaultStr;
	   }

	   MSGs = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
	   fclose(fp);
   }
      
   // If root is null, then return defaultStr
   if (MSGs == NULL)
		return defaultStr;

   // else, search inside the XML three for the message asked.
   // Return default message if it did not found either.
   node = mxmlFindElement(MSGs,MSGs, "wiicoverflow", NULL, NULL, MXML_DESCEND); 
   if(node == NULL)
		return defaultStr;
		
   // Try to search the localLanguage asked
   lang_n = mxmlFindElement(node,node, settings.localLanguage, NULL, NULL, MXML_DESCEND); 
   if (lang_n == NULL) 
		return defaultStr; // Did not found the language especified ? Return the default...

   // So it finally found the Language...
   // Try to find the MESSAGE CODE asked
   msg_n = mxmlFindElement(lang_n,lang_n, CODE, NULL, NULL, MXML_DESCEND); 
   if (msg_n == NULL)
		return defaultStr; // If the CODE was not found, then return the default...
  
  	//Get VAL attribute	
   if (mxmlElementGetAttr(msg_n, "val") != NULL)
        return (char*)(mxmlElementGetAttr(msg_n, "val"));
   else
		return defaultStr;
}
