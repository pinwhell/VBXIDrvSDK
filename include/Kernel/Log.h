#pragma once

#include <Kernel/Decls.h>
#include <Kernel/File.h>
#include <fslc_string.h>

#ifdef VBXIDRVSDK_LOGFILE_ENABLE
constexpr auto VBXIDRVSDK_LOGPATH = VBXIDRVSDK_LOGFOLDER "/" VBXIDRVSDK_LOGFILENAME;
#endif
constexpr auto VBXIDRVSDK_MAXFMTLEN = 256;

DECL(int) printk(const char* format, ...);

template<typename... Args>
inline void KLOG_PRINT(const char* format, Args&&... args) {
	char fmtTmp[VBXIDRVSDK_MAXFMTLEN + 2];
#ifdef VBXIDRVSDK_SYSLOG_ENABLE
	{
		char* pntkLogFmt = fmtTmp;

		fslc_memcpy(pntkLogFmt, KERN_INFO, 2);
		fslc_strncpy(pntkLogFmt + 2, format, VBXIDRVSDK_MAXFMTLEN);

		printk(pntkLogFmt, args...);
	}
#endif

#ifdef VBXIDRVSDK_LOGFILE_ENABLE
	{
		char* sdkFileLogFmt = fmtTmp;
		extern File* gSDKLog;

		fslc_strncpy(sdkFileLogFmt, format, VBXIDRVSDK_MAXFMTLEN - 2);
		fslc_strcat(sdkFileLogFmt, "\n");

		FilePrint(gSDKLog, sdkFileLogFmt, args...);
	}
#endif
	};

#ifdef BITS32
#define PTRFMT "%08x"
#else
#define PTRFMT "%llx"
#endif