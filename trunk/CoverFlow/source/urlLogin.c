/* 
  *  urlLogin.c 
  * 
  *  Wii CoverFloader 
  *  Copyright 2009 Beardface April 29th, 2009 
  *  Additional coding by: gitkua, scognito, F1SHE4RS, afour98, blackbird399, LoudBob11 
  *  Licensed under the terms of the GNU GPL, version 2 
  *  http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt 
 */ 
 #include "urlLogin.h" 
 #include "fileParser.h" 
  
 #ifdef URLBOXART_PASS
 extern s_self self; 
 extern s_path dynPath; 
  
 bool urlFileExist() 
 { 
	 FILE *fp; 
	 char buff[255];
	 
	 sprintf(buff, "%s/%s", dynPath.dir_usb_loader, URLBOXART_PASS);
	 fp = fopen(buff, "r"); 
	 //fp = fopen(URLBOXART_PASS, "r"); 
	 if(fp != NULL) 
	 { 
		 fclose(fp); 
		 return true; 
	 } 
	 else 
		 return false; 
 } 
  
  
  
 void getURL_LoginData() 
 { 
  char buff[255];
	 
  sprintf(buff, "%s/%s", dynPath.dir_usb_loader, URLBOXART_PASS);
  
  cfg_parsefile(buff, &storeURL_LoginData); 
  //cfg_parsefile(URLBOXART_PASS, &storeURL_LoginData); 
 } 
  
  
 void storeURL_LoginData(char *name, char *val) 
 { 
         if (strcmp(name, "username") == 0) { 
                 strcopy(self.url_username, val, sizeof(self.url_username)); 
                 return; 
         } 
  
         if (strcmp(name, "password") == 0) { 
                 strcopy(self.url_password, val, sizeof(self.url_password)); 
                 return; 
         } 
 } 
   
 #endif 
 
