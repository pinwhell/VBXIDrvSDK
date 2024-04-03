#include <stdint.h>

extern bool DriverMain(uintptr_t entry);
extern void DriverShootdown();

__attribute__((visibility("default"))) __attribute__((cdecl)) extern "C" int _load(uintptr_t entry) 
{
    return DriverMain(entry) ? 0 : 1;
}

__attribute__((visibility("default"))) __attribute__((cdecl)) extern "C" void _unload() 
{
    DriverShootdown();
}