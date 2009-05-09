#include "gameSettings.h"

bool getGameSettings(char* gameCode, s_gameSettings* gs){

	FILE *fp;
	mxml_node_t *xml;
	//bool settingsFound = false;
	
	fp = fopen("wiicoverflow.xml", "r");
  
	if(fp != NULL){
		
		xml = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
		fclose(fp);
		
		if(xml != NULL){
			
			mxml_node_t *node;
			mxml_node_t *next_n;
			
			node = mxmlFindElement(xml, xml, "wiicoverflow", NULL, NULL, MXML_DESCEND);
			
			if(node != NULL){
				
				for(next_n = mxmlFindElement(node, node, "game", NULL, NULL, MXML_DESCEND);
					next_n != NULL;
					next_n = mxmlFindElement(next_n, node, "game", NULL, NULL, MXML_DESCEND)
				)
				{
					if((strncmp(mxmlElementGetAttr(next_n, "id"), gameCode, 6) == 0)){
						
						if(mxmlElementGetAttr(next_n,"ocarina"))
							gs->ocarina = atoi(mxmlElementGetAttr(next_n,"ocarina"));
						else
							gs->ocarina = -1;
						if(mxmlElementGetAttr(next_n,"hooktype"))
							gs->hooktype = atoi(mxmlElementGetAttr(next_n,"hooktype"));
						else
							gs->hooktype = -1;
						if(mxmlElementGetAttr(next_n,"language"))
							gs->language = atoi(mxmlElementGetAttr(next_n,"language"));
						else
							gs->language = -1;
						if(mxmlElementGetAttr(next_n,"video"))
							gs->video = atoi(mxmlElementGetAttr(next_n,"video"));
						else
							gs->video = -1;
						if(mxmlElementGetAttr(next_n,"vipatch"))
							gs->vipatch = atof(mxmlElementGetAttr(next_n,"vipatch"));
						else
							gs->vipatch = -1;
						if(mxmlElementGetAttr(next_n,"lastPlayed"))
							strcpy(gs->lastPlayed, mxmlElementGetAttr(next_n,"vipatch"));
						else
							strcpy(gs->vipatch, "Never played before");
						//settingsFound = true;
						return true;
						//break;
					}
				}
				
			}
			
			//WindowPrompt ("CALLED", "Nodo NULL", 0, &cancelButton);
		}
		//else{}
			//WindowPrompt ("XML = NULL", "succa", 0, &cancelButton);
	}
	//else{}
		//WindowPrompt ("Cannot open file", "ssss", 0, &cancelButton);
	//}
	
	/*
	if(!settingsFound){
		
		gs->ocarina = -1;
		gs->hooktype = -1;
		gs->language = -1;
		gs->video = -1;
		gs->vipatch = -1;
	}
	*/
	return false;
}

void setGameSettings(char* gameCode, s_gameSettings* gs){
	
}

void setLastPlayed(char* gameCode, s_gameSettings* gs){

	getGameSettings(gameCode, gs);
	gs->
}
