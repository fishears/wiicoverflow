#include <string.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/mutex.h>
#include <ogc/system.h>
#include "sdhc.h"
#include <ogc/usbstorage.h>
#include <sdcard/wiisd_io.h>

#include "defines.h"
#include "usbstorage.h"

//these are the only stable and speed is good
#define CACHE 32
#define SECTORS 64
#define SD_SECTORS 32

extern bool fatMount (const char* name, const DISC_INTERFACE* interface, sec_t startSector, uint32_t cacheSize, uint32_t SectorsPerPage);
extern void fatUnmount (const char* name);
extern bool fatMountSimple (const char* name, const DISC_INTERFACE* interface);

//extern DISC_INTERFACE __io_sdhc;

int USBDevice_Init()
{
	//closing all open Files write back the cache and then shutdown em!
	fatUnmount("USB:/");
	//right now mounts first FAT-partition
	if (fatMount("USB", &__io_usb2storage, 0, CACHE, SECTORS)) {
		//try first mount with cIOSX
		return 1;
    } else if (fatMount("USB", &__io_wiiums, 0, CACHE, SECTORS)) {
		//try now mount with cIOS
		return 1;
	}
	return -1;
}

void USBDevice_deInit()
{
	//closing all open Files write back the cache and then shutdown em!
	fatUnmount("USB:/");
}

int SDCard_Init()
{
	//closing all open Files write back the cache and then shutdown em!
	fatUnmount("SD:/");
	//right now mounts first FAT-partition
	if (fatMount("SD", &__io_wiisd, 0, CACHE, SD_SECTORS))
		return 1;
//        else if (fatMount("sd:", &__io_sdhc, 0, CACHE, SDHC_SECTOR_SIZE))
//		return 1;
	
	return -1;
}

void SDCard_deInit()
{
	//closing all open Files write back the cache and then shutdown em!
	fatUnmount("SD:/");
}
