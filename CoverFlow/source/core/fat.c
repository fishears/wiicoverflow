#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ogcsys.h>
#include <fat.h>
#include <sys/stat.h>
#include <utils.h>
#include "TrackedMemoryManager.h"

/* Constants */
#define SDHC_MOUNT	"sd:"
#define CACHE 4
#define SECTORS 64
/* Disc interfaces */
extern const DISC_INTERFACE __io_sdhc;

s32 Fat_ReadFile(const char *filepath, void **outbuf)
{
	FILE *fp     = NULL;
	void *buffer = NULL;

	struct stat filestat;
	u32         filelen;

	s32 ret;

	/* Get filestats */
	stat(filepath, &filestat);

	/* Get filesize */
	filelen = filestat.st_size;

	/* Allocate memory */
	buffer = CFMemAlign(32, filelen);
	if (!buffer)
		goto err;

	/* Open file */
	fp = fopen(filepath, "rb");
	if (!fp)
		goto err;

	/* Read file */
	ret = fread(buffer, 1, filelen, fp);	
	if (ret != filelen)
		goto err;

	/* Set pointer */
	*outbuf = buffer;

	goto out;

err:
	/* Free memory */
	if (buffer)
		CFFree(buffer);

	/* Error code */
	ret = -1;

out:
	/* Close file */
	if (fp)
		fclose(fp);

	return ret;
}

s32 Fat_ReadFileToBuffer(const char *filepath, void *outbuf, int maxsize)
{
	#ifdef DEBTXT_FAT
		char txt[1024];
		sprintf(txt, "Fat_ReadFileToBuffer %s %d", filepath, maxsize);
		DebTxt(txt);
	#endif

	FILE *fp     = NULL;

	struct stat filestat;
	u32         filelen;

	s32 ret;

	/* Get filestats */
	stat(filepath, &filestat);

	/* Get filesize */
	filelen = filestat.st_size;

	if (filelen>maxsize){

		#ifdef DEBTXT_FAT
			DebTxt(" Err: filelen > maxsize");
		#endif

		goto err;
	}
	

	/* Open file */
	fp = fopen(filepath, "rb");
	if (!fp){

		#ifdef DEBTXT_FAT
			DebTxt(" Err: cannot open");
		#endif

		goto err;
	}

	/* Read file */
	ret = fread(outbuf, 1, filelen, fp);	
	if (ret != filelen){

		#ifdef DEBTXT_FAT
			DebTxt(" Err: ret != filelen");
		#endif

		goto err;
	}

	goto out;

err:

	/* Error code */
	#ifdef DEBTXT_FAT
		sprintf(txt, " last message: %s", strerror(errno));
	#endif

	ret = -1;

out:
	/* Close file */
	if (fp)
		fclose(fp);

	return ret;
}
