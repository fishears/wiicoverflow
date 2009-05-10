#include "storage.h"


extern s_self self;

bool init_usbfs()
{    
   // __Disc_SetLowMem();
	s32 ret;

	/* Initialize system */
	Sys_Init();

	///* Initialize subsystems */
	Wpad_Init();

	/* Mount SDHC */
	Fat_MountSDHC();
	
	self.progress+=0.05;
	Paint_Progress(self.progress, NULL);
	
	/* Initialize DIP module */
	ret = Disc_Init();
	self.progress+=0.05;
	Paint_Progress(self.progress, NULL);
	
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Could not initialize DIP module! (ret = %d)\n", ret);

		return false;
	}

	return true;
}

bool reinit_usbfs()
{
	u32 ret;
	
	/* UnMount SDHC */
	Fat_UnmountSDHC();
	
	sleep(1);
	
	/* Mount SDHC */
	Fat_MountSDHC();
				
	/* Initialize DIP module */
	ret = Disc_Init();
	
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Could not initialize DIP module! (ret = %d)\n", ret);

		return false;
	}

	return true;
}

void saveFile(char* imgPath, struct block file){
			
	/* save png to sd card for future use*/
			
	FILE *f;
	f = fopen(imgPath, "wb");
	if (f)
	{
		fwrite(file.data,1,file.size,f);
		fclose (f);
	}
}

void checkDirs(){
	
	int result = 0;
	
	DIR_ITER* dir = diropen(USBLOADER_PATH);
	if (dir == NULL) {
		
		mkdir(USBLOADER_PATH, S_ISVTX);
		//int result = chdir("SD:/usb-loader/");
		result = chdir(USBLOADER_PATH);
		
		if(result == 0){
			//WindowPrompt("Cover download","result = 0", &okButton, NULL);
			mkdir("disks", S_ISVTX);
			//WindowPrompt("Cover download","result = 0", &okButton, NULL);
			mkdir("covers", S_ISVTX);
			mkdir("games", S_ISVTX);
		}
		else{
			WindowPrompt("ERROR!","Can't create directories. Covers will not be saved.", &okButton, NULL);
		}
	}
	else{
	
	//	WindowPrompt("Cover download","Closing dir", &okButton, NULL);
		dirclose(dir);
		
		result = chdir(USBLOADER_PATH);
		
		if(result == 0){
			dir = diropen("disks");
	//		WindowPrompt("Cover download",USBLOADER_PATH "/disks/", &okButton, NULL);
			if(dir == NULL) {
				mkdir("disks", S_ISVTX);
			}
			else{
				dirclose(dir);
			}
			
			dir = diropen("covers");
	//		WindowPrompt("Cover download",USBLOADER_PATH "/disks/", &okButton, NULL);	
			if(dir == NULL) {
				mkdir("covers", S_ISVTX);
			}
			else{
				dirclose(dir);
			}
			
			dir = diropen("games");
	//		WindowPrompt("Cover download",USBLOADER_PATH "/disks/", &okButton, NULL);	
			if(dir == NULL) {
				mkdir("games", S_ISVTX);
			}
			else{
				dirclose(dir);
			}
		}
		
		else{
			WindowPrompt("ERROR!","Can't create directories. Covers will not be saved.", &okButton, NULL);
		}
	}
}

void checkFiles(){
	
	FILE* fp;
	
	fp = fopen(USBLOADER_PATH "/gamelist.xml", "r");
	
	if(fp == NULL)
		createEmptyGameSettingsFile();
	else
		fclose(fp);
}
