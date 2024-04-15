#include <cstdint>
#include <Kernel/Syms.h>
#include <TBS.hpp>

using namespace TBS;

// SDK Initialization

int SDKInit(uintptr_t entry)
{
	KLOG_PRINT("SDKInitialized()");

	return 0;
}