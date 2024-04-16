#include <stdint.h>

extern bool SDKInit(uintptr_t entry);
extern bool DriverMain(uintptr_t entry);
extern void DriverShootdown();

__attribute__((visibility("default"))) extern "C" int _load(uintptr_t entry) 
{
    if (int result = SDKInit(entry))
        return result;

    return DriverMain(entry) ? 0 : 1;
}

__attribute__((visibility("default"))) extern "C" void _unload() 
{
    DriverShootdown();
}