#include <cstdint>
#include <Kernel/Syms.h>

// SDK Initialization

extern int SDKKallsymsInit();
extern int SDKOffsetsInit();
extern int SDKLogInitialize();

int SDKInit(uintptr_t entry)
{
	if (int r = SDKKallsymsInit())
		return r;

	if (int r = SDKOffsetsInit())
		return r;

	if (int r = SDKLogInitialize())
		return r;

	return 0;
}