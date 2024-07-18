#include <cstdint>
#include <Kernel/Syms.h>

// SDK Shootdown

extern void SDKLogShootdown();

void SDKDeinit()
{
	SDKLogShootdown();
}