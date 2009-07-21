#include <ogc/machine/processor.h>

#include "updater.h"
#include "TrackedMemoryManager.h"

static lwp_t networkthread = LWP_THREAD_NULL;

extern s_self self;
extern s_settings settings;



bool ShutdownWC24()
{
  	bool onlinefix = true;
	s32 kd_fd, ret;
	STACK_ALIGN(u8, kd_buf, 0x20, 32);

	kd_fd = IOS_Open("/dev/net/kd/request", 0);
	if (kd_fd >= 0) 
	{
		ret = IOS_Ioctl(kd_fd, 7, NULL, 0, kd_buf, 0x20);
		if(ret >= 0)
			onlinefix = false; // fixed no IOS reload needed
		IOS_Close(kd_fd);
	}
	return onlinefix;
}



void * networkinitcallback(void *arg)
{

	self.dummy++;
	if(networkInit(self.ipAddress)){
#ifdef NEWS_READER 
		checkForNews();
#endif		
		if(checkForUpdate())
			self.updateAvailable = true;
		else
			self.updateAvailable = false;
	}

	return NULL;
}

void initNetworkThread()
{
	LWP_CreateThread(&networkthread, networkinitcallback, NULL, NULL, 0, 80);
}

void shutdownNetworkThread()
{
	LWP_JoinThread (networkthread, NULL);
	networkthread = LWP_THREAD_NULL;
}

bool checkForUpdate(){
	
	struct block file;
	char* url = "http://wiicoverflow.googlecode.com/files/current.txt";
	char serverRevision[10];
	int rev;
	
	unlink(USBLOADER_PATH "/current.txt");
	file = downloadfile(url);
	
	if(file.data != NULL){
		saveFile(USBLOADER_PATH "/current.txt", file);
		CFFree(file.data);
		
		FILE* fp;
		fp = fopen(USBLOADER_PATH "/current.txt", "r");
		fgets(serverRevision, sizeof(serverRevision), fp);
		fclose(fp);
		
		rev = atoi(serverRevision);
		
		if(rev > SVN_VERSION){
			return true;
		}
	}
	
	return false;
}


bool promptForUpdate(){
	
	FILE* fp;
	int i;
	char line[256];
	char message[1024];
	
	fp = fopen(USBLOADER_PATH "/current.txt", "r");
	
	if(fp == NULL){
		WindowPrompt("ERROR!", "Cannot open update file!", 0, &okButton);
		return false;
	}
	
	int numLines = getNumLines(fp);
	
	if(numLines <= 0){
		fclose(fp);
		WindowPrompt("ERROR!", "Update file is empty!", 0, &okButton);
		return -2;
	}
	
	rewind(fp);
	
	fgets(line, sizeof(line), fp); // skip revision
	
	strcpy(message, "What's new in ");
	
	fgets(line, sizeof(line), fp); // version name
	if (line[strlen(line)-2]==0x0d) // playing with notepad = stupid titles
			line[strlen(line)-2] = '\0';
	
	strcat(message, line);
	strcat(message, "\n\n\n");
	
	for(i=0; i<numLines-2; i++){ // message
		fgets(line, sizeof(line), fp);
		
		if (line[strlen(line)-2]==0x0d) // playing with notepad = stupid titles
			line[strlen(line)-2] = '\0';
		
		strcat(message, line);
		strcat(message, "\n");		
	}
	
	fclose(fp);
	unlink(USBLOADER_PATH "/current.txt");
	
	return WindowPrompt("Update Available!", message, &okButton, &cancelButton);
}

#ifdef NEWS_READER 
bool checkForNews(){
	char nowDate[7];
	struct block file;
	char* url = "http://wiicoverflow.googlecode.com/files/CoverFloader.news";
	
	setNewsDate(nowDate);
	if ( atoi(nowDate) > atoi(settings.newsDate) )
		{	
		 unlink(USBLOADER_PATH "/CoverFloader.news");
		 file = downloadfile(url);
		
		 if(file.data != NULL){
			saveFile(USBLOADER_PATH "/CoverFloader.news", file);
			strcpy(settings.newsDate, nowDate);
			CFFree(file.data);
			return true;
			}
		}
	return false;
}
#endif



