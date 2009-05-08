#include "gameSettings.h"

s_gameSettings gameSetting;

void getGameSettings(char* gameCode, s_gameSettings* gs){

	FILE *fp;
	mxml_node_t *xml;

	fp = fopen("wiicoverflow.xml", "r");
  
	if(fp != NULL){
		
		xml = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK); 
		fclose(fp);
		
		if(xml != NULL){
			
			mxml_node_t *node;
			mxml_node_t *next_n;
			
			node = mxmlFindElement(xml, xml, "wiicoverflow", NULL, NULL, MXML_DESCEND); 
			
			if(node != NULL){ //INIZIO

				for(next_n = mxmlFindElement(node, node, "game", NULL, NULL, MXML_DESCEND);
					next_n != NULL;
					next_n = mxmlFindElement(next_n, node, "game", NULL, NULL, MXML_DESCEND)
				)
				{
					//if(){ //we find TEH game, and fill TEH struct yeah
					//      //too late for coding
					//}
					
					/* portii
					 float x = atof(mxmlElementGetAttr(next_n,"x"));
					 float y = atof(mxmlElementGetAttr(next_n,"y"));
					
					 int index = m_cakeSprites.size();
					
					 m_cakeSprites.push_back(new Sprite());
					 m_cakeSprites[index]->SetImage(images[Builder::CAKE]);
					 m_cakeSprites[index]->SetPosition(x, y);
					 */
				}
				
			}//FINE
		}
		else
			gs = NULL;
	}
	else gs = NULL;
}

void setGameSettings(char* gameCode, s_gameSettings* gs){
	
}


