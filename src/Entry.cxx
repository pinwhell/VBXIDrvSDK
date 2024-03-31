#include <stdint.h>

extern bool DriverMain(uintptr_t entry);
extern void DriverShootdown();

extern "C" int _load(uintptr_t entry) 
{
    return DriverMain(entry) ? 0 : 1;
}

extern "C" void _unload() 
{
    DriverShootdown();
}