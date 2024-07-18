#include <Kernel/Log.h>
#include <Kernel/File.h>
#include <etl/memory.h>

#ifdef VBXIDRVSDK_LOGFILE_ENABLE
File* gSDKLog;
#endif

int SDKLogInitialize()
{
#ifdef VBXIDRVSDK_LOGFILE_ENABLE
	return (gSDKLog = FileOpen(VBXIDRVSDK_LOGPATH, "w", true, true)) == nullptr ? -1 : 0;
#else
	return 0;
#endif
}

void SDKLogShootdown()
{
#ifdef VBXIDRVSDK_LOGFILE_ENABLE
	FileClose(gSDKLog);
#endif
}