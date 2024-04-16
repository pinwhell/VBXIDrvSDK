#include <cstdint>
#include <Kernel/Syms.h>

// SDK Initialization

extern int SDKKallsymsInit();

int SDKInit(uintptr_t entry)
{
	if (int r = SDKKallsymsInit())
		return r;

	return 0;
}