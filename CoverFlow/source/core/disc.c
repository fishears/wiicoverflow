#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <ogc/lwp_watchdog.h>

#include "apploader.h"
#include "disc.h"
#include "video.h"
#include "wdvd.h"
#include "wiipad.h"
#include "fst.h"
//#include "cfg.h"
#include "subsystem.h"

#include "settings.h"

extern s_settings settings;


/* Constants */
#define PTABLE_OFFSET	0x40000
#define WII_MAGIC	0x5D1C9EA3

/* Disc pointers */
static u32 *buffer = (u32 *)0x93000000;
static u8  *diskid = (u8  *)0x80000000;
static char gameid[8];

void __Disc_SetLowMem(void)
{

    *(vu32 *)0x80000020 = 0x0D15EA5E;       // Standard Boot Code
    *(vu32 *)0x80000024 = 0x00000001;       // Version

    *(vu32 *)0x80000030 = 0x00000000;       // Arena Low
    *(vu32 *)0x800000F4 = 0x817E5480;       // BI2
    *(vu32 *)0x800000F8 = 0x0E7BE2C0;       // Console Bus Speed
    *(vu32 *)0x800000FC = 0x2B73A840;       // Console CPU Speed

    /* Setup low memory */
    *(vu32 *)0x80000060 = 0x38A00040;
    *(vu32 *)0x800000E4 = 0x80431A80;
    *(vu32 *)0x800000EC = 0x81800000;       // Dev Debugger Monitor Address
    *(vu32 *)0x800000F0 = 0x01800000;       // Simulated Memory Size



    memset(gameid, 0, 8);
    memcpy(gameid, (char*)0x80000000, 6);

    if ((strcmp(gameid,"R3XE6U")==0)||
            (strcmp(gameid,"R3XP6V")==0))/*&&
		(IOS_GetVersion()==249)&&
		((IOS_GetRevision()==10)||(IOS_GetRevision()==13))  I left out the ios check to see if works with other ios versions.*/
    {
        *(vu32*)0x80003184	= 0x80000000;    // Game ID Address
    }

	

	/* Copy disc ID */
	if((settings.loser) || (!settings.ocarina))
		memcpy((void *)0x80003180, (void *)0x80000000, 4);

	/* Flush cache */
	DCFlushRange((void *)0x80000000, 0x3F00);
}

void __Disc_SetVMode(void)
{
	GXRModeObj *vmode = NULL;
	
	u32 progressive, tvmode, vmode_reg = 0;

	/* Get video mode configuration */
	progressive = (CONF_GetProgressiveScan() > 0) && VIDEO_HaveComponentCable();
	tvmode      =  CONF_GetVideo();

	/* Select video mode register */
	switch (tvmode) {
	case CONF_VIDEO_PAL:
		if (CONF_GetEuRGB60() > 0) {
			vmode_reg = 5; //PAL60
			vmode     = (progressive) ? &TVNtsc480Prog : &TVEurgb60Hz480IntDf;
		} else
			vmode_reg = 1; //PAL

		break;

	case CONF_VIDEO_MPAL:
		vmode_reg = 4; //MPAL
		break;
	}

	switch (settings.video) {

	case 0:
	{
		/* Select video mode */
		switch(diskid[3]) {
		/* PAL */
        case 'P':
		case 'D':
        case 'F':
		case 'I':
		case 'S':
		case 'H':
        case 'X':
        case 'Y':
        case 'Z':
			if (tvmode != CONF_VIDEO_PAL) {
				vmode_reg = 5;
				vmode     = (progressive) ? &TVNtsc480Prog : &TVEurgb60Hz480IntDf;
			}
			break;

		/* NTSC or unknown */
		case 'E':
		case 'J':
			if (tvmode != CONF_VIDEO_NTSC) {
				vmode_reg = 0;
				vmode     = (progressive) ? &TVNtsc480Prog : &TVNtsc480IntDf;
			}
			break;
		}
	}
	break;
	
    case 2: // Force PAL50
        vmode_reg = 1;
        progressive = (CONF_GetProgressiveScan() > 0) && VIDEO_HaveComponentCable();
        vmode     = (progressive) ? &TVNtsc480Prog : &TVPal528IntDf;
        break;
    case 3: // Force PAL60
        vmode_reg = 5;
        progressive = (CONF_GetProgressiveScan() > 0) && VIDEO_HaveComponentCable();
        vmode     = (progressive) ? &TVNtsc480Prog : &TVEurgb60Hz480IntDf;
        break;
    case 4: // Force NTSC
        vmode_reg = 0;
        progressive = (CONF_GetProgressiveScan() > 0) && VIDEO_HaveComponentCable();
        vmode     = (progressive) ? &TVNtsc480Prog : &TVNtsc480IntDf;
        break;

    case 5:
         break;
	}
	
	/* Set video mode register */
	*(vu32 *)0x800000CC = vmode_reg;

	/* Set video mode */
	if (vmode) {
		Video_Configure(vmode);
       /* Setup video */
        VIDEO_SetBlack(FALSE);
        VIDEO_Flush();
        VIDEO_WaitVSync();
		}

}

void __Disc_SetTime(void)
{
	/* Extern */
	extern void settime(u64);

	/* Set proper time */
	settime(secs_to_ticks(time(NULL) - 946684800));
}

s32 __Disc_FindPartition(u64 *outbuf)
{
	u64 offset = 0, table_offset = 0;

	u32 cnt, nb_partitions;
	s32 ret;

	/* Read partition info */
	ret = WDVD_UnencryptedRead(buffer, 0x20, PTABLE_OFFSET);
	if (ret < 0)
		return ret;

	/* Get data */
	nb_partitions = buffer[0];
	table_offset  = buffer[1] << 2;

	/* Read partition table */
	ret = WDVD_UnencryptedRead(buffer, 0x20, table_offset);
	if (ret < 0)
		return ret;

	/* Find game partition */
	for (cnt = 0; cnt < nb_partitions; cnt++) {
		u32 type = buffer[cnt * 2 + 1];

		/* Game partition */
		if(!type)
			offset = buffer[cnt * 2] << 2;
	}

	/* No game partition found */
	if (!offset)
		return -1;

	/* Set output buffer */
	*outbuf = offset;

	return 0;
}


s32 Disc_Init(void)
{
	/* Init DVD subsystem */
	return WDVD_Init();
}

s32 Disc_Open(void)
{
	s32 ret;

	/* Reset drive */
	ret = WDVD_Reset();
	if (ret < 0)
		return ret;

	/* Read disc ID */
	return WDVD_ReadDiskId(diskid);
}

s32 Disc_Wait(void)
{
	u32 cover = 0;
	s32 ret;

	/* Wait for disc */
	while (!(cover & 0x2)) {
		/* Get cover status */
		ret = WDVD_GetCoverStatus(&cover);
		if (ret < 0)
			return ret;
	}

	return 0;
}

s32 Disc_SetWBFS(u32 mode, u8 *id)
{
	/* Set WBFS mode */
	return WDVD_SetWBFSMode(mode, id);
}

s32 Disc_SetUSB(u8 *id) 
{
    /* Set USB mode */
    return WDVD_SetUSBMode(id);
}


s32 Disc_ReadHeader(void *outbuf)
{
	/* Read disc header */
	return WDVD_UnencryptedRead(outbuf, sizeof(struct discHdr), 0);
}

s32 Disc_IsWii(void)
{
	struct discHdr *header = (struct discHdr *)buffer;

	s32 ret;

	/* Read disc header */
	ret = Disc_ReadHeader(header);
	if (ret < 0)
		return ret;

	if(header == 0)
		return -1;

	/* Check magic word */
	if (header->magic != WII_MAGIC)
		return -1;

	return 0;
}

s32 Disc_BootPartition(u64 offset)
{
	entry_point p_entry;

	s32 ret;

	/* Open specified partition */
	ret = WDVD_OpenPartition(offset);
	if (ret < 0)
		return ret;

	/* Run apploader */
	ret = Apploader_Run(&p_entry);
	if (ret < 0)
		return ret;

	/* Setup low memory */
	__Disc_SetLowMem();

	/* Set an appropiate video mode */
	__Disc_SetVMode();

	/* Set time */
	__Disc_SetTime();

	/* OCARINA STUFF - FISHEARS*/
	if (settings.ocarina)
	{
		memset(gameid, 0, 8);
		memcpy(gameid, (char*)0x80000000, 6);
		do_sd_code(gameid);
	}
    /* OCARINA STUFF - FISHEARS*/

	/* Close subsystems */
	Subsystem_Close();

	/* Shutdown IOS subsystems */
 	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);

	/* Jump to entry point */
	p_entry();

	return 0;
}

s32 Disc_WiiBoot(void)
{
	u64 offset;
	s32 ret;

	/* Find game partition offset */
	ret = __Disc_FindPartition(&offset);
	if (ret < 0)
		return ret;

	/* Boot partition */
	return Disc_BootPartition(offset);
}


void PatchCountryStrings(void *Address, int Size)
{
    u8 SearchPattern[4]	= { 0x00, 0x00, 0x00, 0x00 };
    u8 PatchData[4]		= { 0x00, 0x00, 0x00, 0x00 };
    u8 *Addr			= (u8*)Address;

    int wiiregion = CONF_GetRegion();

    switch (wiiregion)
    {
        case CONF_REGION_JP:
            SearchPattern[0] = 0x00;
            SearchPattern[1] = 0x4A; // J
            SearchPattern[2] = 0x50; // P
            break;
        case CONF_REGION_EU:
            SearchPattern[0] = 0x02;
            SearchPattern[1] = 0x45; // E
            SearchPattern[2] = 0x55; // U
            break;
        case CONF_REGION_KR:
            SearchPattern[0] = 0x04;
            SearchPattern[1] = 0x4B; // K
            SearchPattern[2] = 0x52; // R
            break;
        case CONF_REGION_CN:
            SearchPattern[0] = 0x05;
            SearchPattern[1] = 0x43; // C
            SearchPattern[2] = 0x4E; // N
            break;
        case CONF_REGION_US:
        default:
            SearchPattern[0] = 0x01;
            SearchPattern[1] = 0x55; // U
            SearchPattern[2] = 0x53; // S
    }

    switch (diskid[3])
    {
        case 'J':
            PatchData[1] = 0x4A; // J
            PatchData[2] = 0x50; // P
            break;

        case 'D':
        case 'F':
        case 'P':
        case 'X':
        case 'Y':
            PatchData[1] = 0x45; // E
            PatchData[2] = 0x55; // U
            break;

        case 'E':
        default:
            PatchData[1] = 0x55; // U
            PatchData[2] = 0x53; // S
    }

    while (Size >= 4)
    {
        if (Addr[0] == SearchPattern[0] && Addr[1] == SearchPattern[1] && Addr[2] == SearchPattern[2] && Addr[3] == SearchPattern[3])
        {
            //*Addr = PatchData[0];
            Addr += 1;
            *Addr = PatchData[1];
            Addr += 1;
            *Addr = PatchData[2];
            Addr += 1;
            //*Addr = PatchData[3];
            Addr += 1;
            Size -= 4;
        } else
        {
            Addr += 4;
            Size -= 4;
        }
    }
}
