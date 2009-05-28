#include "capp_dol.h"
#include "app_dol.h"
#include "app.h"

extern "C" {

int CAPPDOL_Launch(const void * dol, const char * CommandLine)
{
	C_App_dol* capp = new C_App_dol();
	return capp->Launch(dol, CommandLine);
}

#ifdef __cplusplus
}
#endif
