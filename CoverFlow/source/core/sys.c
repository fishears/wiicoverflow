#include <stdio.h>
#include <ogcsys.h>

#include "sys.h"
#include "wiipad.h"
#include "fatmounter.h"
#include "mload.h"
#include "wdvd.h"
#include "usbstorage.h"
#include "wbfs.h"
#include "disc.h"

/* Constants */
#define CERTS_LEN	0x280

/* Variables */
static const char certs_fs[] ATTRIBUTE_ALIGN(32) = "/sys/cert.sys";

u8 shutdown = 0;
u8 reset    = 0;

void __Sys_ResetCallback(void)
{
	/* Reboot console */
	reset = 1;
}

void __Sys_PowerCallback(void)
{
	/* Poweroff console */
	shutdown = 1;
}


void Sys_Init(void)
{
	/* Initialize video subsytem */
	//VIDEO_Init();

	/* Set RESET/POWER button callback */
	SYS_SetResetCallback(__Sys_ResetCallback);
	SYS_SetPowerCallback(__Sys_PowerCallback);
}

static void _ExitApp() {
    //ExitGUIThreads();
    //StopGX();
    //ShutdownAudio();

    SDCard_deInit();
    USBDevice_deInit();
    mload_set_ES_ioctlv_vector(NULL);
    mload_close();
}

void Sys_Reboot(void)
{
	/* Restart console */
	_ExitApp();
	STM_RebootSystem();
}

void Sys_Shutdown(void)
{
	Wpad_Disconnect();
	/* Poweroff console */
	if(CONF_GetShutdownMode() == CONF_SHUTDOWN_IDLE) {
		s32 ret;

		/* Set LED mode */
		ret = CONF_GetIdleLedMode();
		if(ret >= 0 && ret <= 2)
			STM_SetLedMode(ret);

		/* Shutdown to idle */
		STM_ShutdownToIdle();
	} else {
		/* Shutdown to standby */
		STM_ShutdownToStandby();
	}
}

void Sys_LoadMenu(void)
{
	_ExitApp();
	/* Return to the Wii system menu */
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
}

s32 Sys_GetCerts(signed_blob **certs, u32 *len)
{
	static signed_blob certificates[CERTS_LEN] ATTRIBUTE_ALIGN(32);

	s32 fd, ret;

	/* Open certificates file */
	fd = IOS_Open(certs_fs, 1);
	if (fd < 0)
		return fd;

	/* Read certificates */
	ret = IOS_Read(fd, certificates, sizeof(certificates));

	/* Close file */
	IOS_Close(fd);

	/* Set values */
	if (ret > 0) {
		*certs = certificates;
		*len   = sizeof(certificates);
	}

	return ret;
}

int Sys_IosReload(int IOS) {
    s32 ret = -1;

    //shutdown SD and USB before IOS Reload in DiscWait
    SDCard_deInit();
    USBDevice_deInit();

    WPAD_Flush(0);
    WPAD_Disconnect(0);
    WPAD_Shutdown();

    WDVD_Close();

    USBStorage_Deinit();

    if (IOS == 249 || IOS == 222 || IOS == 223) {
        int i;
		for (i = 0; i < 10; i++) {
            ret = IOS_ReloadIOS(IOS);
            if (ret < 0) return ret;
            if (IOS == 222 || IOS == 223) load_ehc_module();
            ret = WBFS_Init(WBFS_DEVICE_USB);
            if (!(ret < 0)) break;
            sleep(1);
            USBStorage_Deinit();
        }
        if (ret>=0) {
            ret = Disc_Init();
            if (ret>=0) {
                ret = WBFS_Open();
            }
        } else Sys_BackToLoader();
    }

    PAD_Init();
    Wpad_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL,WPAD_FMT_BTNS_ACC_IR);
//    WPAD_SetVRes(WPAD_CHAN_ALL, screenwidth, screenheight);
    //reinitialize SD and USB
    SDCard_Init();
    USBDevice_Init();

    return ret;
}

void Sys_BackToLoader(void) {
    if (*((u32*) 0x80001800)) {
        _ExitApp();
        exit(0);
    }
    // Channel Version
    Sys_LoadMenu();
}


