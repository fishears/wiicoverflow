#include <ogc/machine/processor.h> 
  
 #include "updater.h" 
 #include "TrackedMemoryManager.h" 
  
 static lwp_t networkthread = LWP_THREAD_NULL; 
  
 extern s_self self; 
 extern s_settings settings; 
 extern s_path dynPath; 
  
 s32 connection; 
 static s32 socket; 
  
 static bool waitforanswer = false; 
  
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
  
 s32 network_request(const char * request) { 
     char buf[1024]; 
     char *ptr = NULL; 
  
     u32 cnt, size; 
     s32 ret; 
  
     /* Send request */ 
     ret = net_send(connection, request, strlen(request), 0); 
     if (ret < 0) 
         return ret; 
  
     /* Clear buffer */ 
     memset(buf, 0, sizeof(buf)); 
  
     /* Read HTTP header */ 
     for (cnt = 0; !strstr(buf, "\r\n\r\n"); cnt++) 
         if (net_recv(connection, buf + cnt, 1, 0) <= 0) 
             return -1; 
  
     /* HTTP request OK? */ 
     if (!strstr(buf, "HTTP/1.1 200 OK")) 
         return -1; 
     /* Retrieve content size */ 
     ptr = strstr(buf, "Content-Length:"); 
     if (!ptr) 
         return -1; 
  
     sscanf(ptr, "Content-Length: %u", &size); 
     return size; 
 } 
  
 s32 network_read(u8 *buf, u32 len) { 
     u32 read = 0; 
     s32 ret = -1; 
  
     /* Data to be read */ 
     while (read < len) { 
         /* Read network data */ 
         ret = net_read(connection, buf + read, len - read); 
         if (ret < 0) 
             return ret; 
  
         /* Read finished */ 
         if (!ret) 
             break; 
  
         /* Increment read variable */ 
         read += ret; 
     } 
  
     return read; 
 } 
  
 /**************************************************************************** 
  * Download request 
  ***************************************************************************/ 
 s32 download_request(const char * url) { 
  
     //Check if the url starts with "http://", if not it is not considered a valid url 
     if (strncmp(url, "http://", strlen("http://")) != 0) { 
         return -1; 
     } 
  
     //Locate the path part of the url by searching for '/' past "http://" 
     char *path = strchr(url + strlen("http://"), '/'); 
  
     //At the very least the url has to end with '/', ending with just a domain is invalid 
     if (path == NULL) { 
         return -1; 
     } 
  
     //Extract the domain part out of the url 
     int domainlength = path - url - strlen("http://"); 
  
     if (domainlength == 0) { 
         return -1; 
     } 
  
     char domain[domainlength + 1]; 
     strncpy(domain, url + strlen("http://"), domainlength); 
     domain[domainlength] = '\0'; 
  
     connection = GetConnection(domain); 
     if (connection < 0) { 
         return -1; 
     } 
  
     //Form a nice request header to send to the webserver 
     char header[strlen(path)+strlen(domain)+100]; 
     sprintf(header, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, domain); 
  
     s32 filesize = network_request(header); 
  
     return filesize; 
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
         char* url = "http://wiicoverflow.googlecode.com/svn/trunk/CoverFlow/Current_Release/current.txt"; 
         char serverRevision[10]; 
         int rev; 
         char buff[255]; 
          
         sprintf(buff,"%s/current.txt", dynPath.dir_usb_loader); 
         unlink(buff); 
         //unlink(USBLOADER_PATH "/current.txt"); 
         file = downloadfile(url); 
          
         if(file.data != NULL){ 
                 saveFile(buff, file); 
                 //saveFile(USBLOADER_PATH "/current.txt", file); 
                 CFFree(file.data); 
                  
                 FILE* fp; 
                 fp = fopen(buff, "r"); 
                 //fp = fopen(USBLOADER_PATH "/current.txt", "r"); 
                 fgets(serverRevision, sizeof(serverRevision), fp); 
                 fclose(fp); 
                  
                 rev = atoi(serverRevision); 
                  
                 if(rev > SVN_VERSION){ 
                         return true; 
                 } 
         } 
          
         return false; 
 } 
  
 bool downloadUpdate(){ 
  
     s32 filesize = download_request("http://wiicoverflow.googlecode.com/svn/trunk/CoverFlow/Current_Release/current.dol"); 
          
         char progtxt[100]; 
         int percent = 0; 
          
         char dolpath[255]; 
         sprintf(dolpath,"%s/current.dol", dynPath.dir_usb_loader); 
          
         char dolpathsuccess[255]; 
         sprintf(dolpathsuccess,"%s/boot.dol", dynPath.dir_usb_loader); 
  
         int failed = 1; 
         s32 i  = 0; 
         s32 ret = 0; 
          
         if (filesize > 0) { 
                 FILE * pfile; 
                 pfile = fopen(dolpath, "wb"); 
                  
                 u8 * blockbuffer = TrackedMalloc(BLOCKSIZE); 
                  
                 for (i = 0; i < filesize; i += BLOCKSIZE) { 
                         usleep(100); 
                          
                         percent = (int)(i * 100.0) / filesize; 
          
                         sprintf(progtxt, "Downloading Update... (%d%%)", percent);  
                          
                         Paint_Progress_Generic(i, filesize, progtxt);  
  
                         u32 blksize; 
                         blksize = (u32)(filesize - i); 
                         if (blksize > BLOCKSIZE) 
                                 blksize = BLOCKSIZE; 
  
                         ret = network_read(blockbuffer, blksize); 
                          
                         if (ret != (s32) blksize) { 
                                 failed = -1; 
                                 ret = -1; 
                                 fclose(pfile); 
                                 remove(dolpath); 
                                 break; 
                         } 
                          
                         fwrite(blockbuffer,1,blksize, pfile); 
                 } 
                  
                 fclose(pfile); 
                  
                 if(blockbuffer) 
                 { 
                         TrackedFree(blockbuffer); 
                 } 
                  
                 if (!failed) { 
                         //remove old 
                         if (existFile(dolpathsuccess)) { 
                                 remove(dolpathsuccess); 
                         } 
                          
                         //rename new to old 
                         rename(dolpath, dolpathsuccess); 
                          
                         CloseConnection(); 
                          
                         return true; 
                 } 
                  
                 CloseConnection(); 
                  
                 return false; 
         } 
          
         return false; 
 } 
  

 bool existFile(char * path) 
 { 
     FILE * f; 
     f = fopen(path,"r"); 
     if(f) { 
     fclose(f); 
     return true; 
     } 
 return false; 
 } 
  
  
 bool promptForUpdate(){ 
          
         FILE* fp; 
         int i; 
         char line[256]; 
         char message[1024]; 
         char buff[255]; 
          
         sprintf(buff,"%s/current.txt", dynPath.dir_usb_loader); 
         fp = fopen(buff, "r"); 
         //fp = fopen(USBLOADER_PATH "/current.txt", "r"); 
          
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
         unlink(buff); 
         //unlink(USBLOADER_PATH "/current.txt"); 
         return WindowPrompt("Update Available!", message, &okButton, &cancelButton); 
 } 
  
 #ifdef NEWS_READER  
 bool checkForNews(){ 
         char nowDate[7]; 
         struct block file; 
         char* url = "http://wiicoverflow.googlecode.com/svn/trunk/CoverFlow/CFNS/CoverFloader.news"; 
         char buff[255]; 
          
         sprintf(buff,"%s/CoverFloader.news", dynPath.dir_usb_loader); 
          
         setNewsDate(nowDate); 
         if ( atoi(nowDate) > atoi(settings.newsDate) ) 
                 {        
                  unlink(buff); 
                  //unlink(USBLOADER_PATH "/CoverFloader.news"); 
                  file = downloadfile(url); 
                  
                  if(file.data != NULL){ 
                         saveFile(buff, file); 
                         //saveFile(USBLOADER_PATH "/CoverFloader.news", file); 
                         strcpy(settings.newsDate, nowDate); 
                         CFFree(file.data); 
                         return true; 
                         } 
                 } 
         return false; 
 } 
 #endif 
  
  
 void CloseConnection() { 
  
     net_close(connection); 
  
     if (waitforanswer) { 
         net_close(socket); 
         waitforanswer = false; 
     } 
 }


//**********************************************
 #ifdef NEWS_READER 
bool getNewsFile()
{
 char nowDate[7]; 
 struct block file; 
 char* url = "http://wiicoverflow.googlecode.com/svn/trunk/CoverFlow/CFNS/CoverFloader.news"; 
 char buff[255]; 
  
 sprintf(buff,"%s/CoverFloader.news", dynPath.dir_usb_loader); 
  
 setNewsDate(nowDate); 
 if ( (atoi(nowDate) > atoi(settings.newsDate)) || !newsFileExist()) 
		 {        
		  if(networkInit(self.ipAddress))
			{
			  unlink(buff); 
			  file = downloadfile(url); 
			  
			  if(file.data != NULL)
			  { 
				 saveFile(buff, file); 
				 strcpy(settings.newsDate, nowDate); 
				 CFFree(file.data); 
				 return true; 
			  }
			}
		 } 
 return false; 
}
#endif

