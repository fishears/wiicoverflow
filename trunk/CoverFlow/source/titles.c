#include "titles.h"

extern s_self self;

int initTitle(){

	FILE* fp;
	
	fp = fopen(USBLOADER_PATH "/titles.txt", "r");
	
	if(fp == NULL)
		return -1;
		
	int numLines = getNumLines(fp);
	//rewind(fp);
	
	if(numLines <= 0){
		fclose(fp);
		return -2;
	}
	
	fclose(fp);
	return numLines;
}

int getNumLines(FILE* fp){
	
	char line[MAX_TITLE_LEN];
	int lineCount = 0;
	
	while (fgets(line, sizeof(line), fp)) {
		lineCount++;
	}

	return lineCount;
}

//void fillTitleStruct(s_title titleList[], int len){
void fillTitleStruct(s_title* titleList, int len){
	
	int i = 0;
	char line[MAX_TITLE_LEN];
	
	//char sss[123];
	//sprintf(sss, "%d", len);
	//WindowPrompt(sss, "Cuct!", 0, &cancelButton);
	
	FILE* fp;
	fp = fopen(USBLOADER_PATH "/titles.txt", "r");
	
	if(fp == NULL){
		WindowPrompt("ERROR!", "Cannot fill struct!", 0, &cancelButton);
		return;
	}
	
	//malloc();
	
	else
	{
		//while(fgets(line, sizeof(line), fp)){
		for(i=0; i<len; i++){
			fgets(line, sizeof(line), fp);
			snprintf(titleList[i].id, 5, "%s",line);
			sprintf(titleList[i].name, "%s",line+7);
			titleList[i].name[strlen(titleList[i].name)-2] = '\0';
			//i++;
			//break;
		}
		
		fclose(fp);
	}
	
	/*
	else
	{
		//while(fgets(line, sizeof(line), fp)){
		for(i=0; i<1000; i++){
			fgets(line, sizeof(line), fp);
			snprintf(titleList[i].id, 4, "%s",line);
			//sprintf(titleList[i].name, "%s",line+7);
			//titleList[i].name[strlen(titleList[i].name)-2] = '\0';
			//i++;
			//break;
		}
		
		fclose(fp);
	}
	char succa[256];
	sprintf(succa, "%s", titleList[0].id);
	WindowPrompt(succa, titleList[0].id, 0, &cancelButton);
	*/
	
	//char succa[256];
	//sprintf(succa, "%s", titleList[1000].id);
	//WindowPrompt(succa, titleList[1000].name, 0, &cancelButton);
}

void getTitle(s_title* titleList, char* id, char* title){
	
	//WindowPrompt("I'm alive", "yeah still!", 0, &okButton);
	
	int i = 0;
	for(i=0; i<self.titlesTxtSize; i++){
		if((strncmp(id, titleList[i].id, 4)==0)){
			sprintf(title, "%s", titleList[i].name);
		}
	}
}
